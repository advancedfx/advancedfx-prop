#include "windows.h" // we need access to virtualprotect etc.

#include "wrect.h"
#include "cl_dll.h"
#include "cdll_int.h"
#include "r_efx.h"
#include "com_model.h"
#include "r_studioint.h"
#include "pm_defs.h"
#include "cvardef.h"
#include "entity_types.h"
#include "cmdregister.h"

extern cl_enginefuncs_s *pEngfuncs;
extern engine_studio_api_s *pEngStudio;
extern playermove_s* ppmove;

REGISTER_DEBUGCVAR(test_snd_show,"1",0);
REGISTER_DEBUGCVAR(test_snd_export,"0",0);

#include "detours.h"

//
// see Quake 1 Source snd_dma.c for more info
//

#define ADDRESS_S_Update 0x01da4200
#define DETOURSIZE_S_Update 0x07

#define ADDRESS_CALL_S_Update_ (ADDRESS_S_Update+0x160)

#define ADDRESS_S_Update_ 0x01da4490
#define DETOURSIZE_S_Update_ 0x06

#define ADDRESS_S_PaintChannels 0x01da6910
#define DETOURSIZE_S_PaintChannels 0x06

#define ADDRESS_S_TransferPaintBuffer 0x01da6120
#define DETOURSIZE_S_TransferPaintBuffer 0x05

// those brackets (adr - adr) are very improtant, never forget them! since the result of the experssion might be converted to another type and +x might mean +x*sizeof(type_being_pointed_too).
#define ADDRESS_total_channels 0x027a8ecc
#define ADDRESS_channels (0x027a8f04-0x04)
#define ADDRESS_paintedtime 0x027a7a40
#define ADDRESS_p_shm 0x024ab538
#define ADDRESS_paintbuffer 0x02791734

//QW/bothdefs.h:
#define	MAX_QPATH		64			// max length of a quake game pathname

// QW/zone.h:
// typedef struct cache_user_s
// {
//	void	*data;
// } cache_user_t;
// defined in hlsdk/common/com_model.h

//>>QW/snd_dma.c:

typedef struct
{
	int left;
	int right;
} portable_samplepair_t;


typedef struct sfx_s
{
	char 	name[MAX_QPATH];
	cache_user_t	cache;
} sfx_t;

typedef struct
{
	sfx_t	*sfx;			// sfx number
	int		leftvol;		// 0-255 volume
	int		rightvol;		// 0-255 volume
	int		end;			// end time in global paintsamples
	int 	pos;			// sample position in sfx
	int		looping;		// where to loop, -1 = no looping
	int		entnum;			// to allow overriding a specific sound
	int		entchannel;		//
	vec3_t	origin;			// origin of sound effect
	vec_t	dist_mult;		// distance multiplier (attenuation/clipK)
	int		master_vol;		// 0-255 master volume

	DWORD	unknown0;
	DWORD	unknown1;
	DWORD	unknown2;
	DWORD	unknown3;
	DWORD	unknown4;
} channel_t;

//<<QW/snd_dma.c:

channel_t channels;

//>>snd_mix.c:
//#define	PAINTBUFFER_SIZE	512
//typedef (portable_samplepair_t[PAINTBUFFER_SIZE]) paintbuffer_t;
//<<snd_mix.c:

//typedef enum {false, true} qboolean; // QW/common-

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

//#include "util_vector.h" // vec_3t

typedef void (*S_Update_t)(vec3_t origin, vec3_t forward, vec3_t right, vec3_t up);
typedef void (*S_Update__t)(void);
typedef void (*S_PaintChannels_t)(int endtime);
typedef void (*S_TransferPaintBuffer_t)(int endtime);

S_Update_t detoured_S_Update=NULL;
S_Update__t detoured_S_Update_=NULL;
S_PaintChannels_t detoured_S_PaintChannels=NULL;
S_TransferPaintBuffer_t detoured_S_TransferPaintBuffer=NULL;

bool g_bIn_S_Update=false;

void touring_S_Update(vec3_t origin, vec3_t forward, vec3_t right, vec3_t up)
{
	g_bIn_S_Update=true;
	detoured_S_Update(origin, forward, right, up);
	g_bIn_S_Update=false;
}

void touring_S_Update_(void)
{
	int total;
	channel_t* ch;
	int i;

	int total_channels = *(int *)ADDRESS_total_channels;
	channel_t *channels = (channel_t *)ADDRESS_channels;

	if (test_snd_show->value!=0)
	{
		total = 0;
		ch = channels;
		for (i=0 ; i<total_channels; i++, ch++)
			if (ch->sfx && (ch->leftvol || ch->rightvol) )
			{
				pEngfuncs->Con_Printf ("%3i %3i %s\n", ch->leftvol, ch->rightvol, ch->sfx->name);
				//pEngfuncs->Con_Printf ("%3i %3i 0x%08x\n", ch->leftvol, ch->rightvol, ch->sfx);
				total++;
			}
		
			pEngfuncs->Con_Printf ("----(%i/%i,%s)----\n", total,total_channels,g_bIn_S_Update ? "norm" : "extra");
	}

	detoured_S_Update_();
}

void touring_S_PaintChannels(int endtime)
{
	int paintedtime_o = *(int *)ADDRESS_paintedtime;
	detoured_S_PaintChannels(endtime);
	int paintedtime = *(int *)ADDRESS_paintedtime;

	if (test_snd_show->value>1.0) pEngfuncs->Con_Printf("%i -> %i / %i \n",paintedtime_o,paintedtime,endtime);
}

//test_snd_export

// designed after:
// General RIFF description provided by
// Robert Shuler <rlshuler@aol.com>
// (downloaded form www.wotsit.org)+hopefully without his mistakes : P

typedef struct
{			// CHUNK 8-byte header
	char  id[4];	// identifier, e.g. "fmt " or "data"
	DWORD len;		// remaining chunk length after header
} chunk_hdr_t;

static struct
{
	struct {     
		char	id[4];	// identifier string = "RIFF"
		DWORD	len;	// remaining length after this header
	} riff_hdr;

	char wave_id[4];	// WAVE file identifier = "WAVE"

	chunk_hdr_t fmt_chunk_hdr; // Fmt chunk header

	struct
	{
		WORD	wFormatTag;			// Format category
		WORD	wChannels;			// Number of channels
		DWORD	dwSamplesPerSec;	// Sampling rate
		DWORD	dwAvgBytesPerSec;	// For buffer estimation
		WORD	wBlockAlign;		// Data block size

		// PCM specific:
		WORD wBitsPerSample;

	} fmt_chunk_pcm;

	chunk_hdr_t data_chunk_hdr; // Fmt chunk header

} g_wave_header;


DWORD g_dwSamples;

FILE* fBeginWave(char *pszFileName,DWORD SamplesPerSec)
{
	memset(&g_wave_header,0,sizeof(g_wave_header)); // clear header

	g_dwSamples = 0; // clear written samples num

	FILE *pHandle=fopen(pszFileName, "wb");

	if (!pHandle) return NULL;

	// write temporary header:
	memcpy(g_wave_header.riff_hdr.id,"RIFF",4);
	g_wave_header.riff_hdr.len=0;

	memcpy(g_wave_header.wave_id,"WAVE",4);

	memcpy(g_wave_header.fmt_chunk_hdr.id,"fmt ",4);
	g_wave_header.fmt_chunk_hdr.len = sizeof(g_wave_header.fmt_chunk_pcm);

	g_wave_header.fmt_chunk_pcm.wFormatTag = 0x0001; // Microsoft PCM
	g_wave_header.fmt_chunk_pcm.wChannels = 2;
	g_wave_header.fmt_chunk_pcm.dwSamplesPerSec = SamplesPerSec;
	g_wave_header.fmt_chunk_pcm.dwAvgBytesPerSec = 2 * SamplesPerSec * (16 / 8);
	g_wave_header.fmt_chunk_pcm.wBlockAlign = 2 * (16 / 8);
	g_wave_header.fmt_chunk_pcm.wBitsPerSample = 16;

	memcpy(g_wave_header.data_chunk_hdr.id,"data",4);
	g_wave_header.data_chunk_hdr.len = 0;

	fwrite(&g_wave_header,sizeof(g_wave_header),1,pHandle);

	return pHandle;

}

void fWriteWave(FILE *pHandle,WORD leftchan,WORD rightchan)
{
	if (!pHandle) return;
	
	WORD chans[2];

	chans[0]=leftchan;
	chans[1]=rightchan;

	fwrite(chans,sizeof(WORD),2,pHandle);
	g_dwSamples++;
}

void fEndWave(FILE* pHandle)
{
	if (!pHandle) return;

	long lfpos = ftell(pHandle);
	
	fseek(pHandle,0,SEEK_SET);
	
	// we need fo finish the header:
	g_wave_header.riff_hdr.len=lfpos-4;
	g_wave_header.data_chunk_hdr.len= g_dwSamples * (g_wave_header.fmt_chunk_pcm.wBitsPerSample/8) * g_wave_header.fmt_chunk_pcm.wChannels;

	// and write it:
	fwrite(&g_wave_header,sizeof(g_wave_header),1,pHandle);

	fclose(pHandle);
}

bool g_bRec=false;

void touring_S_TransferPaintBuffer(int endtime)
{
	static FILE *pWaveFile;
	
	// retrive globals:
	int paintedtime = *(int *)ADDRESS_paintedtime;
	volatile dma_t *shm=*(dma_t **)ADDRESS_p_shm;
	portable_samplepair_t *paintbuffer = (portable_samplepair_t *)ADDRESS_paintbuffer;


	 // ´for infos why we do what here see QW/snd_mix.c/S_TransferStereo16 and others

	int		lpos;
	int		lpaintedtime;

	int *snd_p;
	int snd_linear_count;

	snd_p = (int *) paintbuffer;
	lpaintedtime = paintedtime;

	bool b_show = (test_snd_show->value!=0);

	if (b_show) pEngfuncs->Con_Printf("PaintBuffer: %i .. %i\n",lpaintedtime,endtime);

	if (test_snd_export->value==1.0)
	{
		if (!g_bRec)
		{
			pWaveFile=fBeginWave("mdt_Test.wav",shm->samples);
			g_bRec=true;
		}

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

			fWriteWave(pWaveFile,wlchan,wrchan);
			lpaintedtime+=2;

			//pEngfuncs->Con_Printf("%i %i ",snd_p[0],snd_p[1]);
			snd_p+=2;
		}
	} else {
		if (g_bRec)
		{
			g_bRec=false;
			fEndWave(pWaveFile);
		}
	}


	/*while (lpaintedtime < endtime)
	{
	// handle recirculating buffer issues
		lpos = lpaintedtime & ((shm->samples>>1)-1);

		//snd_out = (short *) pbuf + (lpos<<1);

		snd_linear_count = (shm->samples>>1) - lpos;
		if (lpaintedtime + snd_linear_count > endtime)
			snd_linear_count = endtime - lpaintedtime;

		snd_linear_count <<= 1;

	// write a linear blast of samples
		//Snd_WriteLinearBlastStereo16 ();
		// Instead we capture our sound here:

		if (b_show) pEngfuncs->Con_Printf("newpos=%i, count=%i\n",lpos,snd_linear_count);

		snd_p += snd_linear_count;
		lpaintedtime += (snd_linear_count>>1);
	}*/

	if (b_show) pEngfuncs->Con_Printf("SoundInfo: Channels=%i, Samples=%i\n",shm->channels,shm->samples);
	if (b_show) pEngfuncs->Con_Printf("------\n");

	detoured_S_TransferPaintBuffer(endtime);
}
REGISTER_DEBUGCMD_FUNC(test_snd_inst)
{
	//if (!detoured_S_Update_) detoured_S_Update_ = (S_Update__t) DetourApply((BYTE *)ADDRESS_S_Update_, (BYTE *)touring_S_Update_, (int)DETOURSIZE_S_Update_);
	//if (!detoured_S_Update) detoured_S_Update = (S_Update_t) DetourApply((BYTE *)ADDRESS_S_Update, (BYTE *)touring_S_Update, (int)DETOURSIZE_S_Update);
	//if (!detoured_S_PaintChannels) detoured_S_PaintChannels = (S_PaintChannels_t) DetourApply((BYTE *)ADDRESS_S_PaintChannels, (BYTE *)touring_S_PaintChannels, (int)DETOURSIZE_S_PaintChannels);
	if (!detoured_S_TransferPaintBuffer) detoured_S_TransferPaintBuffer = (S_TransferPaintBuffer_t) DetourApply((BYTE *)ADDRESS_S_TransferPaintBuffer, (BYTE *)touring_S_TransferPaintBuffer, (int)DETOURSIZE_S_TransferPaintBuffer);
	pEngfuncs->Con_Printf("__mirv_test_snd: tried to install hooks.\n");
}