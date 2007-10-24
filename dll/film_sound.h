/*
File        : film_sound.h
Started     : 2007-10-22T17:39Z
Project     : Mirv Demo Tool
Authors     : Dominik Tugend
Description : Object with functions for interfacing the H-L Sound
*/

#ifndef MDT_FILM_SOUND_H
#define MDT_FILM_SOUND_H

#include <windows.h>
#include <stdio.h> // FILE, ...

class CFilmSound
//
// attention, this should be used as singelton and it is not threadsafe!
//
// attention, Timing and flow asumptions:
// this class makes assumptions about the engine's main loop and function call order which you can understand when looking into the Quake 1 Source.
// i.e. it asumes that Start, AdvanceFrame and Stop happen before the hooks kick in.
{
public:
	enum FILM_SOUND_STATE {FSS_IDLE,FSS_STARTING,FSS_FILMING,FSS_STOPPING};
private:
	#define ADDRESS_GetSoundtime 0x01da4370
	#define DETOURSIZE_GetSoundtime 0x06

	#define ADDRESS_S_PaintChannels 0x01da6910
	#define DETOURSIZE_S_PaintChannels 0x06

	#define ADDRESS_S_TransferPaintBuffer 0x01da6120
	#define DETOURSIZE_S_TransferPaintBuffer 0x05

	#define ADDRESS_p_shm 0x024ab538
	#define ADDRESS_paintbuffer 0x0279b8e0
	#define ADDRESS_soundtime 0x027a7a3c
	#define ADDRESS_paintedtime 0x027a7a40

	typedef void (*GetSoundtime_t)(void);
	typedef void (*S_PaintChannels_t)(int endtime);
	typedef void (*S_TransferPaintBuffer_t)(int endtime);

	// wave header structures designed after:
	// General RIFF description provided by
	// Robert Shuler <rlshuler@aol.com>
	// (downloaded form www.wotsit.org)+hopefully without his mistakes : P

	typedef struct
	{			// CHUNK 8-byte header
		char  id[4];	// identifier, e.g. "fmt " or "data"
		DWORD len;		// remaining chunk length after header
	} chunk_hdr_t;

	struct
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

	} _wave_header;

	DWORD _dwWaveSmaplesWritten;

	static CFilmSound* _pFilmSound; // pointer on our self, used by the static funcs that hook to check state etc

	static GetSoundtime_t _detoured_GetSoundtime;
	static S_PaintChannels_t _detoured_S_PaintChannels;
	static S_TransferPaintBuffer_t _detoured_S_TransferPaintBuffer;

	//static int _initial_paintedtime;
	//static _loc_soundtime;

	FILE *_pWaveFile;
	FILM_SOUND_STATE _eFilmSoundState;
	
	float _fTargetTime;
	float _fCurrentTime;

	float _fOld_HL_snd_noextraupdate;

	// look into S_Update_ etc. in Quake 1 Source to understand those:
	// (this is also crucial in order to understand their flow order that is asumed by the class)
	static void _touring_GetSoundtime(void);
	static void _touring_S_PaintChannels(int endtime);
	static void _touring_S_TransferPaintBuffer(int endtime);

	void _InstallHooks();
	void _OnStoppingFinished();	

	// used by static hooks:
	float _get_fTargetTime();
	float _get_fCurrentTime();
	void _set_fCurrentTime(float fNewCurrTime);
	FILM_SOUND_STATE _get_eFilmSoundState();
	void _set_eFilmSoundState(FILM_SOUND_STATE eNewFilmSoundState);
	FILE* _get_pWaveFile();

	FILE* _fBeginWave(char *pszFileName,DWORD dwSamplesPerSec);
	void _fWriteWave(FILE *pHandle,WORD leftchan,WORD rightchan);
	void _fEndWave(FILE* pHandle);

public:

	CFilmSound();

	bool Start(char *pszFileName,float fTargetTime);
	// Only starts when eFilmSoundState()==FSS_IDLE
	// if starting failed it will return false
	// the targettime should be the delta frametime on the first call (so it is not null) and advance with every frame

	void AdvanceFrame(float fTargetTime);
	// this has to be called every engineframe (main loop)
	// to supply a new targettime
	// the targettime should advance with every frame (absoulte to starting)
	// it is suggested that you derive your targettime from the framecount to avoid problems with precision (when accumulating deltatime i.e.)

	void Stop();
	// We cannot stop instantly, we finally stopped when eFilmSoundState()==FSS_IDLE.

	FILM_SOUND_STATE eFilmSoundState();
};


#endif // #ifndef MDT_FILM_SOUND_H