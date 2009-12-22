// #include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2009-11-24 dominik.matrixstorm.com
//
// First changes
// 2007-10-22T17:39Z dominik.matrixstorm.com

// to understand what we do here please look into the Quake 1 (QW) source by ID Software!

//
// includes:
//

// HLSDK, qboolean, Vector, vec_3t, ...:

// BEGIN HLSDK includes
#pragma push_macro("HSPRITE")
#define HSPRITE MDTHACKED_HSPRITE
//
#include <hlsdk/multiplayer/cl_dll/wrect.h>
#include <hlsdk/multiplayer/cl_dll/cl_dll.h>
#include <hlsdk/multiplayer/engine/cdll_int.h>
//
#undef HSPRITE
#pragma pop_macro("HSPRITE")
// END HLSDK includes

#include "film_sound.h"
#include "hl_addresses.h"
#include <hooks/shared/detours.h> // detouring funcs


// for debug:
extern cl_enginefuncs_s *pEngfuncs;

//
// defs for Quake 1 (reconstructed form Q1 Source + H-L) structs we use:
//

typedef struct
{
	int left;
	int right;
} portable_samplepair_t;

typedef struct
{
	qboolean		gamealive;
	qboolean		soundalive;
	qboolean		splitbuffer;
	int				channels;
	int				samples;				// mono samples in buffer
	int				submission_chunk;		// don't mix less than this #
	int				samplepos;				// in mono samples
	int				samplebits;
	int				speed;
	unsigned char	*buffer;
} dma_t;


typedef struct
{
	qboolean		gamealive;
	qboolean		soundalive;
	qboolean		splitbuffer;
	int				channels;
	int				samples;				// mono samples in buffer
	int				submission_chunk;		// don't mix less than this #
	int				samplepos;				// in mono samples
	int				samplebits;
	int				Quake_speed;
	int				Valve_speed;			// added by Valve(?)
} dma_HL_t;

typedef void * channel_t;

typedef void (*GetSoundtime_t)(void);
typedef void (*S_PaintChannels_t)(int endtime);
typedef void (*S_TransferPaintBuffer_t)(int endtime);
typedef channel_t * (*SND_PickChannel_t)(int entnum, int entchannel, int _unknown1, int _unknown2);

GetSoundtime_t detoured_GetSoundtime = NULL;
S_PaintChannels_t detoured_S_PaintChannels = NULL;
S_TransferPaintBuffer_t detoured_S_TransferPaintBuffer = NULL;
SND_PickChannel_t detoured_SND_PickChannel = NULL;

CFilmSound* g_FilmSound = 0;

float g_TargetTime, g_CurrentTime, g_Volume;

bool g_FilmSound_BlockChannels = false;

enum FilmSoundState {
	FSS_IDLE,
	FSS_STARTING,
	FSS_FILMING,
	FSS_STOPPING
} g_FilmSoundState = FSS_IDLE;

void touring_GetSoundtime(void)
{
	if(FSS_FILMING != g_FilmSoundState) {
		// do not update during filming
		detoured_GetSoundtime();
	}
}

void touring_S_PaintChannels(int endtime)
{
	static volatile dma_HL_t *shm;

	if (FSS_STARTING == g_FilmSoundState)
	{
		g_FilmSoundState = FSS_FILMING;
	}

	if (FSS_IDLE != g_FilmSoundState)
	{
		float fDeltaTime;
		int deltaTime;

		shm = *(dma_HL_t **)HL_ADDR_GET(shm);

		fDeltaTime = (
			g_TargetTime - g_CurrentTime
		) * (float)shm->Quake_speed;

		// and override
		if (FSS_STOPPING == g_FilmSoundState)
			deltaTime = (int)ceil(fDeltaTime); // we preffer having too much samples when stopping
		else {
			deltaTime = (int)floor(fDeltaTime); // we preffer having faster updates and therefore less samples during filming
		}

		// we cannot go back in time, so stfu:
		if(deltaTime < 0) deltaTime = 0;

		fDeltaTime = (float)deltaTime / (float)shm->Quake_speed;

		// >> Sound painting
		detoured_S_PaintChannels(*(int *)HL_ADDR_GET(paintedtime) +deltaTime);
		// << Sound painting

		// update Our class's _CurrentTime:
		g_CurrentTime = g_CurrentTime +fDeltaTime;

		if (FSS_STOPPING == g_FilmSoundState) {
			g_FilmSound->Snd_Finished();

			// make soundsystem catch up:
			touring_GetSoundtime();
			*(int *)HL_ADDR_GET(paintedtime) = (*(int *)HL_ADDR_GET(soundtime)) >> 1;
			pEngfuncs->pfnClientCmd("stopsound");
		}

	} else {
		// don't do anything abnormal
		detoured_S_PaintChannels(endtime);
	}

}

void touring_S_TransferPaintBuffer(int endtime)
{
	static volatile dma_HL_t *shm;

	if (FSS_IDLE != g_FilmSoundState)
	{
		// filming

		shm=*(dma_HL_t **)HL_ADDR_GET(shm);
		
		int paintedtime = *(int *)HL_ADDR_GET(paintedtime);

		portable_samplepair_t * paintbuffer = (portable_samplepair_t *)HL_ADDR_GET(paintbuffer);

		int iMyVolume = (int)(g_Volume*256.0f);

		int * snd_p = (int *) paintbuffer;
		int lpaintedtime = paintedtime;

		while (lpaintedtime < endtime)
		{
			for (int i=0;i<2;i++)
			{
				int ilchan;
				int irchan;
				WORD wlchan,wrchan;

				// limiter from Snd_WriteLinearBlastStereo16:
				
				ilchan = (snd_p[0]*iMyVolume)>>8;
				if (ilchan > 0x7fff) wlchan = 0x7fff;
				else if (ilchan < (short)0x8000) wlchan = (short)0x8000;
				else wlchan = ilchan;

				irchan = (snd_p[1]*iMyVolume)>>8;
				if (irchan > 0x7fff) wrchan = 0x7fff;
				else if (irchan < (short)0x8000) wrchan = (short)0x8000;
				else wrchan = irchan;

				g_FilmSound->Snd_Supply(wlchan, wrchan);
				snd_p+=2;
			}

			lpaintedtime++;
		}
	}

	// pass through to sound buffer:
	detoured_S_TransferPaintBuffer(endtime);

}

channel_t * touring_SND_PickChannel(int entnum, int entchannel, int _unknown1, int _unknown2) {

	if(!g_FilmSound_BlockChannels)
		return detoured_SND_PickChannel(entnum, entchannel, _unknown1, _unknown2);

	return 0;
}

void InstallHooks()
{
	// notice the memory allocted here gets never freed o_O
	if(!detoured_GetSoundtime) detoured_GetSoundtime = (GetSoundtime_t) DetourApply((BYTE *)HL_ADDR_GET(GetSoundtime), (BYTE *)touring_GetSoundtime, (int)HL_ADDR_GET(DTOURSZ_GetSoundtime));
	if(!detoured_S_PaintChannels) detoured_S_PaintChannels = (S_PaintChannels_t) DetourApply((BYTE *)HL_ADDR_GET(S_PaintChannels), (BYTE *)touring_S_PaintChannels, (int)HL_ADDR_GET(DTOURSZ_S_PaintChannels));
	if(!detoured_S_TransferPaintBuffer) detoured_S_TransferPaintBuffer = (S_TransferPaintBuffer_t) DetourApply((BYTE *)HL_ADDR_GET(S_TransferPaintBuffer), (BYTE *)touring_S_TransferPaintBuffer, (int)HL_ADDR_GET(DTOURSZ_S_TransferPaintBuffer));
	if(!detoured_SND_PickChannel) detoured_SND_PickChannel = (SND_PickChannel_t)DetourApply((BYTE *)HL_ADDR_GET(SND_PickChannel), (BYTE *)touring_SND_PickChannel, (int)HL_ADDR_GET(DTOURSZ_SND_PickChannel));
}

void FilmSound_BlockChannels(bool block) {
	g_FilmSound_BlockChannels = block;
}


// CFilmSound //////////////////////////////////////////////////////////////////

CFilmSound::CFilmSound()
{
	if(g_FilmSound) throw "err";

	g_FilmSound = this;
}

FILE* CFilmSound::_fBeginWave(char *pszFileName,DWORD dwSamplesPerSec)
{
	memset(&_wave_header,0,sizeof(_wave_header)); // clear header

	_dwWaveSmaplesWritten = 0; // clear written samples num

	FILE *pHandle=fopen(pszFileName, "wb");

	if (!pHandle) return NULL;

	// write temporary header:
	memcpy(_wave_header.riff_hdr.id,"RIFF",4);
	_wave_header.riff_hdr.len=0;

	memcpy(_wave_header.wave_id,"WAVE",4);

	memcpy(_wave_header.fmt_chunk_hdr.id,"fmt ",4);
	_wave_header.fmt_chunk_hdr.len = sizeof(_wave_header.fmt_chunk_pcm);

	_wave_header.fmt_chunk_pcm.wFormatTag = 0x0001; // Microsoft PCM
	_wave_header.fmt_chunk_pcm.wChannels = 2;
	_wave_header.fmt_chunk_pcm.dwSamplesPerSec = dwSamplesPerSec;
	_wave_header.fmt_chunk_pcm.dwAvgBytesPerSec = 2 * dwSamplesPerSec * (16 / 8);
	_wave_header.fmt_chunk_pcm.wBlockAlign = 2 * (16 / 8);
	_wave_header.fmt_chunk_pcm.wBitsPerSample = 16;

	memcpy(_wave_header.data_chunk_hdr.id,"data",4);
	_wave_header.data_chunk_hdr.len = 0;

	fwrite(&_wave_header,sizeof(_wave_header),1,pHandle);

	return pHandle;

}

void CFilmSound::_fWriteWave(FILE *pHandle,WORD leftchan,WORD rightchan)
{
	if (!pHandle) return;
	
	WORD chans[2];

	chans[0]=leftchan;
	chans[1]=rightchan;

	fwrite(chans,sizeof(WORD),2,pHandle);
	_dwWaveSmaplesWritten++;
}

void CFilmSound::_fEndWave(FILE* pHandle)
{
	if (!pHandle) return;

	long lfpos = ftell(pHandle);
	
	fseek(pHandle,0,SEEK_SET);
	
	// we need fo finish the header:
	_wave_header.riff_hdr.len=lfpos-4;
	_wave_header.data_chunk_hdr.len= _dwWaveSmaplesWritten * (_wave_header.fmt_chunk_pcm.wBitsPerSample/8) * _wave_header.fmt_chunk_pcm.wChannels;

	// and write it:
	fwrite(&_wave_header,sizeof(_wave_header),1,pHandle);

	fclose(pHandle);
}

bool CFilmSound::Start(char *pszFileName, float fTargetTime, float fUseVolume)
{
	InstallHooks(); // make sure hooks are installed

	if (g_FilmSoundState == FSS_IDLE)
	{
		// only start when idle
		
		// init time:
		g_TargetTime = fTargetTime;
		g_CurrentTime = 0;

		// set volume:
		g_Volume = fUseVolume;

		// retrive sound info structure (since we need the samples per second value == shm->Valve_speed):
		volatile dma_HL_t *shm=*(dma_HL_t **)HL_ADDR_GET(shm);

		if(!(_pWaveFile=_fBeginWave(pszFileName,shm->Valve_speed))) // we use Quake speed since we capture the internal mixer
			return false; // on fail return false

		g_FilmSoundState = FSS_STARTING; // switch to filming mode

		return true;
	} else
		return false;
}

void CFilmSound::AdvanceFrame(float fTargetTime)
{
	// update frame time:
	g_TargetTime = fTargetTime;
}

void CFilmSound::Stop()
{
	if (g_FilmSoundState != FSS_IDLE)
		g_FilmSoundState = FSS_STOPPING; // we cannot stop instantly
}


void CFilmSound::Snd_Supply(WORD leftchan, WORD rightchan) {
	_fWriteWave(_pWaveFile, leftchan, rightchan);
}

void CFilmSound::Snd_Finished() {
	_fEndWave(_pWaveFile); // finish the wave file
	g_FilmSoundState = FSS_IDLE; // we will be idle again
	
	pEngfuncs->Con_Printf("Sound system finished stopping.\n");
}

