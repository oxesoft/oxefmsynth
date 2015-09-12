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

#include "constants.h"
#include "buffers.h"
#include "persist.h"
#include "programs.h"
#include "reverb.h"
#include "oscillator.h"
#include "envelop.h"
#include "noise.h"
#include "filter.h"
#include "delay.h"
#include "note.h"

class CSynthesizer
{
public:
    CBuffers                  buffers; // auxiliar buffers
private:
    float                  samplerate;
    CPrograms               programs;
    CReverb                    reverb;
    CDelay                      delay;
    CNote            notes[POLIPHONY];
    float            veloc[POLIPHONY]; // notes velocity
    unsigned char    state[POLIPHONY]; // notes state
    unsigned char channels[POLIPHONY]; // notes channel
    unsigned char    tecla[POLIPHONY]; // notes key
    unsigned char heldkeys[POLIPHONY]; // notes held by hold pedal
    char            hld[MIDICHANNELS]; // hold pedal state
    unsigned char   tca[MIDICHANNELS]; // last key pressed
    float           rev[MIDICHANNELS]; // reverb value
    float           dly[MIDICHANNELS]; // delay value
    float           mod[MIDICHANNELS]; // modulation wheel value
    float           vol[MIDICHANNELS]; // volume
    float           pan[MIDICHANNELS]; // pan
    float           aft[MIDICHANNELS]; // aftertouch
    float           ptc[MIDICHANNELS]; // pitch wheel
    int              activeNotesCount; // active notes count
    char                      revrbON; // reverb on
    char                      delayON; // delay on
    // sum signals
    void SumMonoStereo    (int *bInput, int *bNoteOut,               int size);
    void SumStereoMono    (int *bInput, int *bNoteOut, float volume, int size);
    void SumStereoStereo  (int *bInput, int *bNoteOut, float volume, int size);
    inline float Val2Mul  (float  valor);
    void UpdateGlobalEffects();
public:
    CSynthesizer();
    void          SetSampleRate (float samplerate);
    void          Process       (int *b, int size, int position /* the start sample number */);
    void          SendEvent     (unsigned char bS, unsigned char bD1,unsigned char bD2,int position /* the start position */);
    void          KillNotes     (void);
    char          GetState      ();
#ifndef __OXEDMO__
    float         SetDefault    (char channel, int par           );
    void          SetPar        (char channel, int par, float val);
    float         GetPar        (char channel, int par           );
    bool          GetStandBy    (char channel);
    void          SetStandBy    (char channel, bool isWaiting);
    bool          IsEditingName ();
    void          SetEditingName(bool value);
    bool          GetBankMode   ();
    void          SetBankMode   (bool bankMode);
    unsigned char GetNumProgr   (char channel);
    void          GetProgName   (char *str, char channel);
    void          SetProgName   (char *str, char channel);
    void          GetProgName   (char *str, int numpg);
    void          SetProgName   (char *str, int numpg);
    void          GetBankName   (char *str);
    int           GetBankCount  ();
    int           GetBankIndex  ();
    void          SetBankIndex  (int nbank);
    void          SetNumProgr   (char channel, unsigned char numprog);
    void          StoreProgram  (char channel);
    SBank*        GetBank       (void);
    void          CopyProgram   (int destination, int source);
    void          SetBank       (SBank *bank);
    void          SetProgram    (char numprg, SProgram *program);
    bool          HasChanges    ();
    void          SetEditorHn   (void *hWndEditor);
#endif
};
