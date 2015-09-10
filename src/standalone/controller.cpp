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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "synthesizer.h"
#include "controller.h"

#ifndef min
#define min(X, Y) (((X) < (Y)) ? (X) : (Y))
#endif

CController::CController()
{
	// MIDI ------------------
	memset(&events,0,sizeof(events));
	bufferPosAnt       = 0;
	timestampant       = 0;
	m_hmi              = NULL;
	// -----------------------
	lpDS               = NULL;
	lplpDsb            = NULL;
	BufPtr             = NULL;
	bufferPos          = 0;
	stopProcess = 0;
	playCursor         = 0;
	nextPosition       = 0;
	posExt             = 0;
	posInt             = 0;
	
	memset(&pcmwf, 0, sizeof(WAVEFORMATEX));
	pcmwf.wFormatTag      = WAVE_FORMAT_PCM;
	pcmwf.nChannels       = 2;
	pcmwf.nSamplesPerSec  = 44100;
	pcmwf.wBitsPerSample  = 16;
	pcmwf.nBlockAlign     = (pcmwf.nChannels * pcmwf.wBitsPerSample) / 8;
	pcmwf.nAvgBytesPerSec = pcmwf.nSamplesPerSec * pcmwf.nBlockAlign;
}

void CController::FillBuffer(short *bShort, int size)
{
	int tambufferInt = SAMPLES_PER_PROCESS<<1;
	int tambufferExt = size<<1;

	while (true)
	{
		if (!posInt)
		{
			// MIDI -------------------------------------------------------------------------------------------------------
			while(events.eventsCount)
			{
				if (events.event[events.nextEvent].pos > bufferPos+SAMPLES_PER_PROCESS)
					break;
				if (events.event[events.nextEvent].pos < bufferPos)
					events.event[events.nextEvent].pos = bufferPos;
				synthesizer.SendEvent(events.event[events.nextEvent].bstat,events.event[events.nextEvent].bdad1,events.event[events.nextEvent].bdad2,events.event[events.nextEvent].pos);
				events.eventsCount--;
				events.nextEvent++;
				events.nextEvent &=0xFF; // 256
			}
			// ------------------------------------------------------------------------------------------------------------
			synthesizer.Process(synthesizer.buffers.bSynthOut, SAMPLES_PER_PROCESS, bufferPos);
			bufferPos += SAMPLES_PER_PROCESS;
		}
		int iaux = min(tambufferInt-posInt,tambufferExt-posExt);
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

void CController::RenderToSoundcard(void)
{
	lplpDsb->Lock(previousPosition * BUFFER_SIZE_DS * pcmwf.nBlockAlign, BUFFER_SIZE_DS * pcmwf.nBlockAlign, &BufPtr, &BufBytes, NULL, NULL, 0);
	
	short *Buffer = (short*)BufPtr;
	
	FillBuffer(Buffer, BUFFER_SIZE_DS);
	
	lplpDsb->Unlock(BufPtr, BufBytes, NULL, 0);
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
		
		// fill the previous buffer
		pThis->previousPosition = pThis->nextPosition;
		pThis->nextPosition = (char)(pThis->playCursor / size);
		if (pThis->nextPosition != pThis->previousPosition)
			pThis->RenderToSoundcard();
		
		Sleep(5);
	}
	return 0;
}

bool CController::Start(HWND hWnd,unsigned int portamidi, LPGUID portaaudio)
{
	this->hWnd = hWnd;
	HRESULT hr = 0;
	
	hr = DirectSoundCreate(portaaudio, &lpDS, NULL);
	if (hr != DS_OK)
	{
		MessageBox(hWnd,"This audio device don't support the audio capabilities needed",TITLE_SMALL,MB_OK);
		return false;
	}
	
	hr = lpDS->SetCooperativeLevel(hWnd, DSSCL_PRIORITY);
	if (hr != DS_OK)
	{
		MessageBox(hWnd,"Error in SetCooperativeLevel()",TITLE_SMALL,MB_OK);
		return false;
	}
	
	memset(&dsbdesc, 0, sizeof(DSBUFFERDESC));
	dsbdesc.dwSize        = sizeof(DSBUFFERDESC);
	dsbdesc.dwFlags       = DSBCAPS_LOCSOFTWARE | DSBCAPS_GLOBALFOCUS | DSBCAPS_GETCURRENTPOSITION2;
	dsbdesc.dwBufferBytes = NUM_BUFFERS * BUFFER_SIZE_DS * pcmwf.nBlockAlign;
	dsbdesc.lpwfxFormat   = &pcmwf;
	
	hr = lpDS->CreateSoundBuffer(&dsbdesc, &lplpDsb, NULL);
	if (hr != DS_OK)
	{
		MessageBox(hWnd,"Error in CreateSoundBuffer()",TITLE_SMALL,MB_OK);
		return false;
	}

	lplpDsb->Lock(0, dsbdesc.dwBufferBytes, &BufPtr, &BufBytes, NULL, NULL, 0);
	memset(BufPtr, 0, BufBytes);
	lplpDsb->Unlock(BufPtr, BufBytes, NULL, 0);

	lplpDsb->SetCurrentPosition(0);
	lplpDsb->Play(0, 0, DSBPLAY_LOOPING);

	// MIDI -------------------------------------------------------------------
	if (FAILED(midiInOpen( &m_hmi, portamidi, (DWORD)callback, 0, CALLBACK_FUNCTION )))
		MessageBox(hWnd,"Error on midiInOpen()",TITLE_SMALL,MB_OK);
	if (FAILED(midiInStart( m_hmi )))
		MessageBox(hWnd,"Error on midiInStart()",TITLE_SMALL,MB_OK);
	// ------------------------------------------------------------------------

	stopProcess = 0;
	DWORD threadID;
	hthread = CreateThread(NULL, 0, Process, this, 0, &threadID);
	return true;
}

// MIDI -----------------------------------------------------------------------
void CALLBACK CController::callback( HMIDIIN hmi, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2 )
{
	// dwParam1 is the midi event with status in the low byte
	// dwParam2 is the event time in ms since the start of midi in
	if (MIM_DATA == wMsg)
	{
		BYTE data0 = BYTE( dwParam1      & 0x000000FF); 
		BYTE data1 = BYTE((dwParam1>>8)  & 0x000000FF);
		BYTE data2 = BYTE((dwParam1>>16) & 0x000000FF);
		controller->SendMIDIEvent(data0,data1,data2,dwParam2);
	}
}

void CController::SendMIDIEvent(unsigned char bstat,unsigned char bdad1,unsigned char bdad2,unsigned int timestamp)
{
	int i;
	i = events.nextEvent + events.eventsCount;
	i&= 0xFF; // 255
	events.event[i].bstat = bstat;
	events.event[i].bdad1 = bdad1;
	events.event[i].bdad2 = bdad2;
	events.eventsCount++;
	if (bufferPosAnt == bufferPos)
		events.event[i].pos = bufferPos + (((timestamp-timestampant) * pcmwf.nSamplesPerSec) / 1000);
	else
	{
		events.event[i].pos = bufferPos;
		timestampant          = timestamp;
	}
	bufferPosAnt = bufferPos;
}
// ----------------------------------------------------------------------------

void CController::Stop()
{
	if (stopProcess)
		return;

	stopProcess = 1;
	
	while (stopProcess != 2);

	lplpDsb->Stop();
	lplpDsb->Release();
	lpDS->Release();
	// MIDI -----------------
	if (m_hmi)
	{
        midiInStop(m_hmi);
        midiInReset(m_hmi);
        midiInClose(m_hmi);
		m_hmi = NULL;
	}
	// ----------------------
}

// GUI ----------------------
CSynthesizer* CController::GetSynth()
{
	return &synthesizer;
}
// --------------------------
