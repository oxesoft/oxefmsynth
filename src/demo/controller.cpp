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

#ifdef __WITH_DSOUND__
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include "controller.h"
#include <stdio.h>
#include <string.h>

CController::CController()
{
    tunePointer        = 0;
    bufferPos          = 0;
    nextPosition       = 0;
    posExt             = 0;
    posInt             = 0;
#ifdef __WITH_DSOUND__
    lpDS               = NULL;
    lplpDsb            = NULL;
    BufPtr             = NULL;
    stopProcess        = 0;
    playCursor         = 0;
#endif
    pcmwf.wFormatTag      = WAVE_FORMAT_PCM;
    pcmwf.nChannels       = 2;
    pcmwf.nSamplesPerSec  = (unsigned int)SAMPLE_RATE;
    pcmwf.wBitsPerSample  = 16;
    pcmwf.nBlockAlign     = (pcmwf.nChannels * pcmwf.wBitsPerSample) / 8;
    pcmwf.nAvgBytesPerSec = pcmwf.nSamplesPerSec * pcmwf.nBlockAlign;
    pcmwf.cbSize          = 0;
}

void CController::FillBuffer(short *bShort, int size)
{
    int tmp1;
    int tmp2;
    int iaux;
    int tambufferInt = SAMPLES_PER_PROCESS<<1;
    int tambufferExt = size<<1;

    while (1)
    {
        if (!posInt)
        {
            while (tuneReader.events[tunePointer].timestamp < (bufferPos + SAMPLES_PER_PROCESS) && tunePointer < NEVENTS)
            {
                synthesizer.SendEvent(tuneReader.events[tunePointer].status,tuneReader.events[tunePointer].data1,tuneReader.events[tunePointer].data2,tuneReader.events[tunePointer].timestamp);
                tunePointer++;
            }
            synthesizer.Process(synthesizer.buffers.bSynthOut, SAMPLES_PER_PROCESS, bufferPos);
            bufferPos += SAMPLES_PER_PROCESS;
        }
        tmp1 = tambufferInt-posInt;
        tmp2 = tambufferExt-posExt;
        iaux = tmp1<tmp2?tmp1:tmp2;
        while (iaux--)
        {
            bShort[posExt] = (short)synthesizer.buffers.bSynthOut[posInt];
            posExt++;
            posInt++;
        }
        if (posInt >= tambufferInt)
        {
            posInt = 0;
        }
        if (posExt >= tambufferExt)
        {
            posExt = 0;
            break;
        }
    }
}

#ifndef __RENDER_TO_MEMORY__ONLY__
void CController::RenderToFile(void)
{
    FILE  *file;
    short  Buffer[BUFFER_SIZE_FILE<<1];
    int    Temp   = 0;
    int    filesize;

    memset(Buffer, 0, (BUFFER_SIZE_FILE<<1) * sizeof(short));
    
    file = fopen(EXECUTABLE_NAME".wav", "wb");
    fseek(file, 0, SEEK_SET);
    
    // file header
    fwrite("RIFF", 4, 1, file);
    fwrite("    ", 4, 1, file); // file size
    fwrite("WAVE", 4, 1, file);
    fwrite("fmt ", 4, 1, file);
    Temp = sizeof(WAVEFORMATEX);
    fwrite(&Temp , 4, 1, file);
    
    // wave format
    fwrite(&pcmwf, Temp, 1, file);
    
    // chunk header
    fwrite("data", 4, 1, file);
    fwrite("    ", 4, 1, file); // data size

    while (tunePointer < NEVENTS || synthesizer.GetState() == ACTIVE)
    {
        FillBuffer(Buffer, BUFFER_SIZE_FILE);
        fwrite(Buffer, (BUFFER_SIZE_FILE * pcmwf.nBlockAlign), 1, file);
    }

    fseek(file, 0, SEEK_END);
    filesize = ftell(file);

    fseek(file, 4, SEEK_SET);
    fwrite(&filesize, 4, 1, file);

    filesize -= 48; // minus header size
    fseek(file, 44, SEEK_SET);
    fwrite(&filesize, 4, 1, file);

    fclose(file);
}
#endif

void CController::RenderToProfiling(void)
{
    short Buffer[BUFFER_SIZE_FILE<<1];
    
    memset(Buffer, 0, sizeof(Buffer));
    
    while (tunePointer < NEVENTS || synthesizer.GetState() == ACTIVE)
    {
        FillBuffer(Buffer, BUFFER_SIZE_FILE);
    }
}

#ifdef __WITH_DSOUND__
void CController::RenderToSoundcard(void)
{
    lplpDsb->Lock(previousPosition * BUFFER_SIZE_DS * pcmwf.nBlockAlign, BUFFER_SIZE_DS * pcmwf.nBlockAlign, &BufPtr, &BufBytes, NULL, NULL, 0);
    
    short *Buffer = (short*)BufPtr;
    
    FillBuffer(Buffer, BUFFER_SIZE_DS);
    
    lplpDsb->Unlock(BufPtr, BufBytes, NULL, 0);

    if (tunePointer >= NEVENTS && synthesizer.GetState() == INACTIVE)
        PostMessage(hWnd,WM_DESTROY,0,0);
}

DWORD WINAPI CController::Process(void* classe)
{
    CController* pThis = (CController*)classe;

    ULONG size = BUFFER_SIZE_DS * pThis->pcmwf.nBlockAlign;

    while (1)
    {
        if (pThis->stopProcess)
        {
            pThis->stopProcess = 2;
            ExitThread(0);
        }

        pThis->lplpDsb->GetCurrentPosition(&pThis->playCursor, NULL);
        
        pThis->previousPosition = pThis->nextPosition;
        pThis->nextPosition = (char)(pThis->playCursor / size);
        if (pThis->nextPosition != pThis->previousPosition)
            pThis->RenderToSoundcard();

        Sleep(5);
    }
    return 0;
}

void CController::Start(HWND hWnd)
{
    this->hWnd = hWnd;
    HRESULT hr = 0;

    hr = DirectSoundCreate(NULL, &lpDS, NULL);
    if (hr != DS_OK)
    {
        printf("DirectSoundCreate error");
        PostMessage(hWnd,WM_DESTROY,0,0);
        return;
    }
    
    hr = lpDS->SetCooperativeLevel(hWnd, DSSCL_PRIORITY);
    if (hr != DS_OK)
    {
        printf("SetCooperativeLevel error");
        PostMessage(hWnd,WM_DESTROY,0,0);
        return;
    }
    
    memset(&dsbdesc, 0, sizeof(DSBUFFERDESC));
    dsbdesc.dwSize        = sizeof(DSBUFFERDESC);
    dsbdesc.dwFlags       = DSBCAPS_LOCSOFTWARE | DSBCAPS_GLOBALFOCUS | DSBCAPS_GETCURRENTPOSITION2;
    dsbdesc.dwBufferBytes = NUM_BUFFERS * BUFFER_SIZE_DS * pcmwf.nBlockAlign;
    dsbdesc.lpwfxFormat   = &pcmwf;
    
    hr = lpDS->CreateSoundBuffer(&dsbdesc, &lplpDsb, NULL);
    if (hr != DS_OK)
    {
        printf("CreateSoundBuffer error");
        PostMessage(hWnd,WM_DESTROY,0,0);
        return;
    }

    // fill the entire buffer before to start playing
    previousPosition = 0;
    RenderToSoundcard();
    previousPosition = 1;
    RenderToSoundcard();
    previousPosition = 2;
    RenderToSoundcard();
    previousPosition = 3;
    RenderToSoundcard();
    previousPosition = 0;

    lplpDsb->SetCurrentPosition(0);
    lplpDsb->Play(0, 0, DSBPLAY_LOOPING);

    DWORD threadID;
    hthread = CreateThread(NULL, 0, Process, this, 0, &threadID);
}

void CController::Stop()
{
    stopProcess = 1;
    
    while (stopProcess != 2);

    lplpDsb->Stop();
    lplpDsb->Release();
    lpDS->Release();
}
#endif
