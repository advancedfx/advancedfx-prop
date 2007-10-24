/*
File        : film_sound.cpp
Started     : 2007-10-22T17:39Z
Project     : Mirv Demo Tool
Authors     : Dominik Tugend
Description : see film_sound.h
*/

// to understand what we do here please look into the Quake 1 (QW) source by ID Software!

//
// includes:
//

// HLSDK, qboolean, Vector, vec_3t, ...:
#include "wrect.h" 
#include "cl_dll.h"
#include "cdll_int.h"

#include "film_sound.h"
#include "detours.h" // detouring funcs


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

//
//	the static variables for the class:
//

CFilmSound::GetSoundtime_t CFilmSound::_detoured_GetSoundtime;
CFilmSound::S_PaintChannels_t CFilmSound::_detoured_S_PaintChannels;
CFilmSound::S_TransferPaintBuffer_t CFilmSound::_detoured_S_TransferPaintBuffer;

CFilmSound* CFilmSound::_pFilmSound;
//
// class functions:
//

void CFilmSound::_touring_GetSoundtime(void)
{
	// retrive current state:
	static CFilmSound::FILM_SOUND_STATE eCurrState;
	eCurrState = _pFilmSound->_get_eFilmSoundState();

	if (eCurrState == FSS_IDLE)
	{
		// don't do anything abnormal
		_detoured_GetSoundtime();
	} else {
		// we are filming

		if (eCurrState == FSS_STARTING)
		{
			// starting,get last valid time:

			_detoured_GetSoundtime(); // let it calculate a last engine controlled time

			//
			// I am not sure if we should make some adjustments to our class's flow here or not
			// since we probably drop already calculated sounds now
			//

			//// now we will save that time, so we can controll it our selfs:
			//_loc_paintedtime = *(int *)ADDRESS_paintedtime;

			// go to normal filming mode:
			_pFilmSound->_set_eFilmSoundState(eCurrState = FSS_FILMING);
		}

		// override soundtime:
		*(int *)ADDRESS_soundtime = *(int *)ADDRESS_paintedtime; // we always exactly played what we got painted by H-L (at least for now)
	}
}

void CFilmSound::_touring_S_PaintChannels(int endtime)
{
	// retrive current state:
	static CFilmSound::FILM_SOUND_STATE eCurrState;
	eCurrState = _pFilmSound->_get_eFilmSoundState();

	if (eCurrState != FSS_IDLE)
	{
		// we are filming, adjust endtime (the mixahead for the curren frame) as desired:

		// retrive sound info structure:
		static volatile dma_t *shm;
		shm =*(dma_t **)ADDRESS_p_shm;

		// calculate mix ahead of current position:
		static int currPaintedTime = *(int *)ADDRESS_paintedtime;
		static float fendtime;
		fendtime = (float)(currPaintedTime) + (_pFilmSound->_get_fTargetTime()-_pFilmSound->_get_fCurrentTime()) * (float)shm->speed;

		// and override
		if (eCurrState==FSS_STOPPING)
			endtime = (int)ceil(fendtime); // we preffer having too much samples when stopping
		else
			endtime = (int)floor(fendtime); // we preffer having faster updates and therefore less samples during filming
	

		// call original painting:	
		_detoured_S_PaintChannels(endtime);
		// detoured_S_PaintChannels will set paintedtime = end


		static int newPaintedTime = *(int *)ADDRESS_paintedtime;

		// update Our clas'ss _CurrentTime:
		static float fnewcurr = _pFilmSound->_get_fCurrentTime() + ((float)(newPaintedTime-currPaintedTime) / (float)shm->speed);
		_pFilmSound->_set_fCurrentTime(fnewcurr);

		// handle FSS_STOPPING:
		if (eCurrState==FSS_STOPPING)
			_pFilmSound->_OnStoppingFinished();

	} else {
		// don't do anything abnormal
		_detoured_S_PaintChannels(endtime);
	}

}

void CFilmSound::_touring_S_TransferPaintBuffer(int endtime)
{
	// retrive current state:
	static CFilmSound::FILM_SOUND_STATE eCurrState;
	eCurrState = _pFilmSound->_get_eFilmSoundState();
	
	if (eCurrState == FSS_IDLE)
	{
		// don't do anything abnormal
		_detoured_S_TransferPaintBuffer(endtime);
	} else {
		// filming

		// retrive globals:
		static int paintedtime = *(int *)ADDRESS_paintedtime; //this should be == _loc_paintedtime
		static volatile dma_t *shm=*(dma_t **)ADDRESS_p_shm;
		static portable_samplepair_t *paintbuffer = (portable_samplepair_t *)ADDRESS_paintbuffer;

		int		lpaintedtime;

		int *snd_p;

		snd_p = (int *) paintbuffer;
		lpaintedtime = paintedtime;

		while (lpaintedtime < endtime)
		{
			int ilchan;
			int irchan;
			WORD wlchan,wrchan;

			// limiter from Snd_WriteLinearBlastStereo16:
			
			ilchan = snd_p[0];
			if (ilchan > 0x7fff) wlchan = 0x7fff;
			else if (ilchan < (short)0x8000) wlchan = (short)0x8000;
			else wlchan = ilchan;

			irchan = snd_p[1];
			if (irchan > 0x7fff) wrchan = 0x7fff;
			else if (irchan < (short)0x8000) wrchan = (short)0x8000;
			else wrchan = irchan;


			//pEngfuncs->Con_Printf("%i %i ",snd_p[0],snd_p[1]);
	
			_pFilmSound->_fWriteWave(_pFilmSound->_get_pWaveFile(),wlchan,wrchan); // write pair to Wave

			lpaintedtime++;
			snd_p+=2;
		}

	}
}

void CFilmSound::_InstallHooks()
{
	// notice the memory allocted here gets never freed o_O
	if (!_detoured_GetSoundtime) _detoured_GetSoundtime = (GetSoundtime_t) DetourApply((BYTE *)ADDRESS_GetSoundtime, (BYTE *)_touring_GetSoundtime, (int)DETOURSIZE_GetSoundtime);
	if (!_detoured_S_PaintChannels) _detoured_S_PaintChannels = (S_PaintChannels_t) DetourApply((BYTE *)ADDRESS_S_PaintChannels, (BYTE *)_touring_S_PaintChannels, (int)DETOURSIZE_S_PaintChannels);
	if (!_detoured_S_TransferPaintBuffer) _detoured_S_TransferPaintBuffer = (S_TransferPaintBuffer_t) DetourApply((BYTE *)ADDRESS_S_TransferPaintBuffer, (BYTE *)_touring_S_TransferPaintBuffer, (int)DETOURSIZE_S_TransferPaintBuffer);
}

void CFilmSound::_OnStoppingFinished()
{
	_fEndWave(_pWaveFile); // finish the wave file
	_eFilmSoundState = FSS_IDLE; // we will be idle again
}

float CFilmSound::_get_fTargetTime()
{
	return _fTargetTime;
}

float CFilmSound::_get_fCurrentTime()
{
	return _fCurrentTime;
}

void CFilmSound::_set_fCurrentTime(float fNewCurrTime)
{
	_fCurrentTime = fNewCurrTime;
}

CFilmSound::FILM_SOUND_STATE CFilmSound::_get_eFilmSoundState()
{
	return _eFilmSoundState;
}

void CFilmSound::_set_eFilmSoundState(FILM_SOUND_STATE eNewFilmSoundState)
{
	_eFilmSoundState=eNewFilmSoundState;
}

FILE* CFilmSound::_get_pWaveFile()
{
	return _pWaveFile;
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

CFilmSound::CFilmSound()
{
	_pFilmSound = this;
	_detoured_S_TransferPaintBuffer=NULL;
	_detoured_S_PaintChannels=NULL;
	_detoured_GetSoundtime=NULL;

	_eFilmSoundState = FSS_IDLE;
}

bool CFilmSound::Start(char *pszFileName,float fTargetTime)
{
	_InstallHooks(); // make sure hooks are installed

	if (_eFilmSoundState == FSS_IDLE)
	{
		// only start when idle
		
		// init time:
		_fTargetTime = fTargetTime;
		_fCurrentTime = 0;

		// retrive sound info structure (since we need the samples per second value == shm->speed):
		volatile dma_t *shm=*(dma_t **)ADDRESS_p_shm;

		if(!_fBeginWave(pszFileName,shm->speed))
			return false; // on fail return false

		_eFilmSoundState = FSS_STARTING; // switch to filming mode

		return true;
	} else
		return false;
}

void CFilmSound::AdvanceFrame(float fTargetTime)
{
	// update frame time:
	_fTargetTime = fTargetTime;
}

void CFilmSound::Stop()
{
	if (_eFilmSoundState != FSS_IDLE)
		_eFilmSoundState = FSS_STOPPING; // we cannot stop instant
}

CFilmSound::FILM_SOUND_STATE CFilmSound::eFilmSoundState()
{
	return _eFilmSoundState;
}

