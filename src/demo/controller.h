/*
Oxe FM Synth: a software synthesizer
Copyright (C) 2004-2015  Daniel Moura <oxe@oxesoft.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "synthesizer.h"
#include "tune.h"
#include "tunereader.h"

#ifdef __WITH_DSOUND__
#include <mmreg.h>
#include <mmsystem.h>
#include <dsound.h>
#else
typedef struct { 
    unsigned short wFormatTag; 
    unsigned short nChannels; 
    unsigned int   nSamplesPerSec; 
    unsigned int   nAvgBytesPerSec; 
    unsigned short nBlockAlign; 
    unsigned short wBitsPerSample; 
    unsigned short cbSize; 
} WAVEFORMATEX; 

#define WAVE_FORMAT_PCM 1
#endif

#define NUM_BUFFERS        4
#define BUFFER_SIZE_DS     32768
#define BUFFER_SIZE_FILE   1024

class CController
{
private:
	WAVEFORMATEX        pcmwf;
	CSynthesizer        synthesizer;
	CTuneReader         tuneReader;
	char                nextPosition;
	char                previousPosition;
	unsigned int        bufferPos;
#ifdef __DSOUND_INCLUDED__
	IDirectSound        *lpDS;
	IDirectSoundBuffer  *lplpDsb;
	DSBUFFERDESC        dsbdesc;
	void                *BufPtr;
	DWORD               BufBytes;
	DWORD               playCursor;
	volatile char       stopProcess;
	HANDLE hthread;
	HWND   hWnd;
	static DWORD WINAPI Process(void*);
	void RenderToSoundcard(void);
#endif
	int           tunePointer; 
	int           posExt;
	int           posInt;
	void FillBuffer(short *bShort, int size);
public:
	CController();
#ifndef __RENDER_TO_MEMORY__ONLY__
	void RenderToFile(void);
#endif
	void RenderToProfiling(void);
#ifdef __DSOUND_INCLUDED__
	void Start(HWND hWnd);
	void Stop();
#endif
};
