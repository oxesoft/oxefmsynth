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

#include <mmreg.h>
#include <mmsystem.h>
#include <dsound.h>

#define NUM_BUFFERS           2
#define BUFFER_SIZE_DS     2048

class CController
{
private:
    IDirectSound        *lpDS;
    IDirectSoundBuffer  *lplpDsb;
    DSBUFFERDESC        dsbdesc;
    WAVEFORMATEX        pcmwf;
    CSynthesizer        synthesizer;
    char                nextPosition;
    char                previousPosition;
    volatile char       stopProcess;
    void                *BufPtr;
    int                 bufferPos;
    DWORD               BufBytes;
    DWORD               playCursor;
    int                 posExt;
    int                 posInt;
    HWND                hWnd;
    void FillBuffer(short *bShort, int size);
    void RenderToSoundcard(void);
    HANDLE hthread;
    static DWORD WINAPI Process(void*);
    // MIDI ----------------------------------------------------------------------------------------
    HMIDIIN m_hmi;
    static void CALLBACK callback(HMIDIIN hmi, UINT wMsg, DWORD dwInstance, DWORD dw1, DWORD dw2);
    struct MIDIEvent
    {
        unsigned char bstat;
        unsigned char bdad1;
        unsigned char bdad2;
                 int  pos;
    };
    struct MIDIEvents
    {
        int       eventsCount;
        int       nextEvent;
        MIDIEvent event[256];
    };
    MIDIEvents   events;
    int          bufferPosAnt;
    unsigned int timestampant;
    // ---------------------------------------------------------------------------------------------
public:
    CController();
    // MIDI ----------------------------------------------------------------------------------------
    void SendMIDIEvent(unsigned char bstat,unsigned char bdad1,unsigned char bdad2,unsigned int timestamp);
    // ---------------------------------------------------------------------------------------------
    // GUI --------------------------------------
    CSynthesizer* GetSynth();
    // ------------------------------------------
    bool Start(HWND hWnd,unsigned int portamidi, LPGUID portaaudio);
    void Stop();
};
extern CController *controller; // used by MIDI capture
