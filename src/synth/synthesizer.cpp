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
#include <string.h>
#include <math.h>

CSynthesizer::CSynthesizer()
{
    int i;
    revrbON = INACTIVE;
    delayON = INACTIVE;
    activeNotesCount = 0;
    for (i=0; i<POLIPHONY; i++)
    {
        state[i]    = INACTIVE;
        channels[i] = 0;
        tecla[i]    = 0;
        veloc[i]    = 0;
        heldkeys[i] = 0;
    }
    for (i=0; i<MIDICHANNELS; i++)
    {
        tca[i] = 255;
        rev[i] = 0.0f;
        dly[i] = 0.0f;
        mod[i] = 0.0f;
        vol[i] = 0.25f;
        pan[i] = 0.0f;
        aft[i] = 0.0f;
        ptc[i] = 1.0f;
        hld[i] = 0;
    }
    programs.Init();
    reverb.Init();
    delay.Init(buffers.bWaves[0]);
    SetSampleRate(44100.f);
}

void CSynthesizer::SendEvent(unsigned char bS,unsigned char bD1,unsigned char bD2,int position)
{
    int   i         = 0;
    float f         = 0.0f;
    float menorvol  = 1.1f; // any value bigger then the maximum (1.0f)
    int   imenorvl  = POLIPHONY;
    int   channel   = bS & 0x0F;
    switch (bS & 0xF0)
    {
        case 0x80: // Note Off
            bD2 = 0;
        case 0x90: // Note On
            if (!bD2)
            {
                for (i=0; i<POLIPHONY; i++)
                {
                    if (state[i] == ACTIVE)
                    {
                        if (channels[i] == channel)
                        {
                            if (tecla[i] == bD1)
                            {
                                if (hld[channel])
                                {
                                    heldkeys[i] = 1;
                                }
                                else
                                {
                                    notes[i].SendEvent(NOTEOFF,0,position);
                                    state[i] = ENDED;
                                }
                                break;
                            }
                        }
                    }
                }
            }
            else
            {
                // verifies if the node was released (improbable)
                for (i=0; i<POLIPHONY; i++)
                {
                    if (state[i] == ACTIVE)
                    {
                        if (channels[i] == channel)
                        {
                            if (tecla[i] == bD1)
                            {
                                notes[i].SendEvent(NOTEOFF,0,position);
                                state[i] = ENDED;
                                break;
                            }
                        }
                    }
                }
                // ------------------------------------------------------------------
                // a) if all notes are being used, identifies the same note in the same channel that was released and reallocate it;
                // b) otherwise, identifies all notes that were released and reallocate the one of lower volume;
                // c) otherwise, identifies the same note in the same channel and reallocate it;
                // d) otherwise, reallocate the lower volume note.
                // ------------------------------------------------------------------
                // case a
                if (activeNotesCount==POLIPHONY)
                {
                    // kill the same released note on the same channel;
                    for (i=0; i<POLIPHONY; i++)
                    {
                        if (tecla[i] == bD1)
                        {
                            if (channels[i] == channel)
                            {
                                if (state[i] == ENDED)
                                {
                                    //---------------------
                                    state[i] = INACTIVE;
                                    heldkeys[i] = 0;
                                    activeNotesCount--;
                                    //---------------------
                                    break;
                                }
                            }
                        }
                    }
                    // case b
                    if (i==POLIPHONY)
                    {
                        for (i=0; i<POLIPHONY; i++)
                        {
                            if (state[i] == ENDED)
                            {
                                if (veloc[i]<menorvol)
                                {
                                    imenorvl = i;
                                    menorvol = veloc[i];
                                }
                            }
                        }
                        if (imenorvl<POLIPHONY)
                        {
                            i = imenorvl;
                            //---------------------
                            state[i] = INACTIVE;
                            heldkeys[i] = 0;
                            activeNotesCount--;
                            //---------------------
                        }
                        else
                        // case c
                        {
                            for (i=0; i<POLIPHONY; i++)
                            {
                                if (tecla[i] == bD1)
                                {
                                    if (channels[i] == channel)
                                    {
                                        //---------------------
                                        state[i] = INACTIVE;
                                        heldkeys[i] = 0;
                                        activeNotesCount--;
                                        //---------------------
                                        break;
                                    }
                                }
                            }
                            // case d
                            if (i==POLIPHONY)
                            {
                                for (i=0; i<POLIPHONY; i++)
                                {
                                    if (veloc[i]<menorvol)
                                    {
                                        imenorvl = i;
                                        menorvol = veloc[i];
                                    }
                                }
                                i = imenorvl;
                                //---------------------
                                state[i] = INACTIVE;
                                heldkeys[i] = 0;
                                activeNotesCount--;
                                //---------------------
                            }
                        }
                    }
                }
                else
                {
                    // find a free note index
                    for (i=0; i<POLIPHONY; i++)
                    {
                        if (state[i] == INACTIVE)
                        {
                            break;
                        }
                    }
                }
                // allocates a new note ------------------------------------------------------------------
                channels[i]   = channel;
                veloc[i]      = float(bD2) / 127.0f;
                notes[i].Init(programs.GetProgram(channels[i]), &buffers, bD1, tca[channels[i]], veloc[i], samplerate);
                notes[i].SendEvent(NOTEON    , 0               , position);
                notes[i].SendEvent(MODULATION, mod[channels[i]], position);
                notes[i].SendEvent(VOLUME    , vol[channels[i]], position);
                notes[i].SendEvent(PAN       , pan[channels[i]], position);
                notes[i].SendEvent(AFTERTOUCH, aft[channels[i]], position);
                notes[i].SendEvent(PITCH     , ptc[channels[i]], position);
                tecla[i]      = bD1;
                state[i]     = ACTIVE;
                tca[channels[i]] = bD1;
                activeNotesCount++;
                // ---------------------------------------------------------------------------------------
                UpdateGlobalEffects();
            }
            break;
        case 0xA0: // Polyphonic aftertouch
            f = (float)bD2 / 127.0f;
            for (i=0; i<POLIPHONY; i++)
            {
                if (state[i] == ACTIVE)
                {
                    if ((channels[i] == channel) && (tecla[i] == bD1))
                    {
                        notes[i].SendEvent(AFTERTOUCH,f,position);
                        break;
                    }
                }
            }
            break;
        case 0xB0: // Control / Mode change
            switch (bD1)
            {
                case 1: // Modulation Wheel
                {
                    f = (float)bD2 / 127.0f;
                    mod[channel] = f;
                    for (i=0; i<POLIPHONY; i++)
                    {
                        if (state[i] == ACTIVE)
                        {
                            if (channels[i] == channel)
                            {
                                notes[i].SendEvent(MODULATION,f,position);
                            }
                        }
                    }
                    break;
                }
                case 7: // Volume
                    f = (float)bD2 / 127.0f;
                    vol[channel] = f;
                    for (i=0; i<POLIPHONY; i++)
                    {
                        if (state[i] == ACTIVE)
                        {
                            if (channels[i] == channel)
                            {
                                notes[i].SendEvent(VOLUME,f,position);
                            }
                        }
                    }
                    break;
                case 10: // Pan
                    f = (float)(bD2 - 64) / 64.f;
                    pan[channel] = f;
                    for (i=0; i<POLIPHONY; i++)
                    {
                        if (state[i] == ACTIVE)
                        {
                            if (channels[i] == channel)
                            {
                                notes[i].SendEvent(PAN,f,position);
                            }
                        }
                    }
                    break;
                case 91: // Reverb
                    f = (float)bD2 / 127.0f;
                    rev[channel] = f;
                    revrbON = 0;
                    for (i=0; i<16; i++)
                    {
                        if (rev[i])
                        {
                            revrbON = 1;
                            break;
                        }
                    }
                    break;
                case 93: // Delay
                    f = (float)bD2 / 127.0f;
                    dly[channel] = f;
                    delayON = 0;
                    for (i=0; i<16; i++)
                    {
                        if (dly[i])
                        {
                            delayON = 1;
                            break;
                        }
                    }
                    break;
                case 64:  // Hold Pedal (on/off)
                    if (!bD2)
                    {
                        for (i=0; i<POLIPHONY; i++)
                        {
                            if (channels[i] == channel && heldkeys[i] && state[i] == ACTIVE)
                            {
                                notes[i].SendEvent(NOTEOFF,0,position);
                                state[i] = ENDED;
                                heldkeys[i] = 0;
                            }
                        }
                    }
                    hld[channel] = bD2;
                    break;
                case 0: // Bank Select (msb)
                    //OXEDEBUG("Bank Select (msb) = %d", bD2);
                    break;
                case 32: // Bank Select (lsb)
                    programs.SetBankIndex(bD2);
                    UpdateGlobalEffects();
                    //OXEDEBUG("Bank Select (lsb) = %d", bD2);
                    break;
                default:
                    break;
            }
            break;
        case 0xC0: // Program change
            programs.SetNumProgr(channel, bD1);
            for (i=0; i<POLIPHONY; i++)
            {
                if (state[i] == ACTIVE)
                {
                    if (channels[i] == channel)
                    {
                        notes[i].UpdateProgram();
                    }
                }
            }
            break;
        case 0xD0: // Channel aftertouch
            f = (float)bD1 / 127.0f;
            aft[channel] = f;
            for (i=0; i<POLIPHONY; i++)
            {
                if (state[i] == ACTIVE)
                {
                    if (channels[i] == channel)
                    {
                        notes[i].SendEvent(AFTERTOUCH,f,position);
                    }
                }
            }
            break;
        case 0xE0: // Pitch wheel range
            unsigned short _14bit;
            _14bit   = (unsigned short)bD2;
            _14bit <<= 7;
            _14bit  |= (unsigned short)bD1;
            f  = (float(_14bit) / 8192.0f) - 1.0f;
            f *= VPITCH;
            f  = Val2Mul(f);
            ptc[channel] = f;
            for (i=0; i<POLIPHONY; i++)
            {
                if (state[i] == ACTIVE)
                {
                    if (channels[i] == channel)
                    {
                        notes[i].SendEvent(PITCH,f,position);
                    }
                }
            }
        default:   // Ignora qualquer outro event
            break;
    }
}

void CSynthesizer::SetSampleRate(float samplerate)
{
    this->samplerate = samplerate;
    reverb.SetPar(SAMPLERATE, samplerate);
    delay.SetPar(SAMPLERATE, samplerate);
}

void CSynthesizer::Process(int *b, int size, int position)
{
    int i;
    int tam = size<<1;
    memset(b,0,tam*sizeof(int));
    if (revrbON)
        memset(buffers.bREV,0,sizeof(buffers.bREV));
    if (delayON)
        memset(buffers.bDLY,0,sizeof(buffers.bDLY));
    for (i=0;i<POLIPHONY;i++)
    {
        if (state[i] != INACTIVE)
        {
            memset(buffers.bNoteOut,0,sizeof(buffers.bNoteOut));
            notes[i].Process(buffers.bNoteOut,size,position);
            if (INACTIVE == notes[i].GetState())
            {
                //---------------------
                state[i] = INACTIVE;
                heldkeys[i] = 0;
                activeNotesCount--;
                //---------------------
            }
            if (revrbON)
                SumStereoMono(buffers.bNoteOut, buffers.bREV, rev[channels[i]], size);
            if (delayON)
                SumStereoMono(buffers.bNoteOut, buffers.bDLY, dly[channels[i]], size);
            SumStereoStereo(buffers.bNoteOut, b, 1.f-rev[channels[i]], size);
        }
    }
    
    if (revrbON)
    {
        reverb.Process(buffers.bREV,size);
        if (reverb.GetState() == ACTIVE)
            SumMonoStereo(buffers.bREV,b,size);
    }
    if (delayON)
    {
        delay.Process(buffers.bDLY,size);
        SumMonoStereo(buffers.bDLY,b,size);
    }
    // peak limiting
    for (i=0;i<tam;i++)
    {
        if (b[i] >  32767)
            b[i] =  32767;
        if (b[i] < -32768)
            b[i] = -32768;
    }
}

char CSynthesizer::GetState()
{
    for (int i=0; i<POLIPHONY; i++)
    {
        if (state[i] != INACTIVE)
        {
            return ACTIVE;
        }
    }
    if (revrbON)
    {
        if (reverb.GetState() == ACTIVE)
        {
            return ACTIVE;
        }
    }
    return INACTIVE;
}

void CSynthesizer::KillNotes()
{
    for (char i=0; i<POLIPHONY; i++)
    {
        if (state[i] != INACTIVE)
        {
            //---------------------
            state[i] = INACTIVE;
            heldkeys[i] = 0;
            activeNotesCount--;
            //---------------------
        }
    }
}

void CSynthesizer::SumMonoStereo(int *bInput, int *bNoteOut, int size)
{
    int i2;
    int tam = size<<1;
    for (int i=0;i<tam;i+=2)
    {
        i2 = i>>1;
        bNoteOut[i]   += bInput[i2];
        bNoteOut[i+1] += bInput[i2];
    }
}

void CSynthesizer::SumStereoMono(int *bInput, int *bNoteOut, float volume, int size)
{
    int i;
    int tam   = size<<1;
    int vtemp = lrintf(volume * 127.f);
    if (volume != 1.0f)
        for (i=0;i<tam;i+=2)
            bNoteOut[i>>1] += ((bInput[i] + bInput[i+1]) * vtemp)>>7;
    else
        for (i=0;i<tam;i+=2)
            bNoteOut[i>>1] += (bInput[i] + bInput[i+1]);
}

void CSynthesizer::SumStereoStereo(int *bInput, int *bNoteOut, float volume, int size)
{
    int i;
    int tam   = size<<1;
    int vtemp = lrintf(volume * 127.f);
    if (vtemp != 127)
        for (i=0;i<tam;i++)
            bNoteOut[i]  += (bInput[i]  * vtemp)>>7;
    else
        for (i=0;i<tam;i++)
            bNoteOut[i]  += bInput[i];
}

inline float CSynthesizer::Val2Mul(float valor)
{
    if      (valor > 0.0f)
             return powf( 2.0f,  valor / 12.0f);
    else if (valor < 0.0f)
             return powf( 0.5f, -valor / 12.0f);
    else
             return 1.0f;
}

void CSynthesizer::UpdateGlobalEffects()
{
    SBank *bank = programs.GetBank();
    reverb.SetPar(TIME    , bank->global.REVTI);
    reverb.SetPar(DAMP    , bank->global.REVDA);
    delay.SetPar (TIME    , bank->global.DLYTI);
    delay.SetPar (FEEDBACK, bank->global.DLYFE);
    delay.SetPar (RATE    , bank->global.DLYLF);
    delay.SetPar (AMOUNT  , bank->global.DLYLA);
}

/* GUI clue */
#ifndef __OXEDMO__

float CSynthesizer::SetDefault(char channel, int par)
{
    return programs.SetDefault(channel, par);
}

void CSynthesizer::SetPar(char channel, int par, float val)
{
    switch (par)
    {
        case RVBLV:
            SendEvent(0xB0|channel,91,(unsigned char)lrintf(val*127.f),0);
            break;
        case DLYLV:
            SendEvent(0xB0|channel,93,(unsigned char)lrintf(val*127.f),0);
            break;
        default:
            switch (par)
            {
                case REVTI: reverb.SetPar(TIME    , val); break;
                case REVDA: reverb.SetPar(DAMP    , val); break;
                case DLYTI: delay.SetPar (TIME    , val); break;
                case DLYFE: delay.SetPar (FEEDBACK, val); break;
                case DLYLF: delay.SetPar (RATE    , val); break;
                case DLYLA: delay.SetPar (AMOUNT  , val); break;
            }
            programs.SetPar(channel, par, val);
            for (int i=0; i<POLIPHONY; i++)
            {
                if (state[i] == ACTIVE)
                {
                    if (this->channels[i] == (unsigned char)channel)
                    {
                        notes[i].UpdateProgram();
                    }
                }
            }
    }
}

float CSynthesizer::GetPar(char channel, int par)
{
    switch (par)
    {
        case RVBLV:
            return rev[channel];
        case DLYLV:
            return dly[channel];
        default:
            return programs.GetPar(channel, par);
    }
}

bool CSynthesizer::GetStandBy(char channel)
{
    return programs.GetStandBy(channel);
}

void CSynthesizer::SetStandBy(char channel, bool isWaiting)
{
    programs.SetStandBy(channel,isWaiting);
}

bool CSynthesizer::IsEditingName()
{
    return programs.IsEditingName();
}

void CSynthesizer::SetEditingName(bool value)
{
    programs.SetEditingName(value);
}

bool CSynthesizer::GetBankMode()
{
    return programs.GetBankMode();
}

void CSynthesizer::SetBankMode(bool bankMode)
{
    programs.SetBankMode(bankMode);
}

unsigned char CSynthesizer::GetNumProgr(char channel)
{
    return programs.GetNumProgr(channel);
}

void CSynthesizer::GetProgName(char *str, char channel)
{
    programs.GetProgName(str, channel);
}

void CSynthesizer::SetProgName(char *str, char channel)
{
    programs.SetProgName(str, channel);
}

void CSynthesizer::GetProgName(char *str, int numpg)
{
    programs.GetProgName(str, numpg);
}

void CSynthesizer::SetProgName(char *str, int numpg)
{
    programs.SetProgName(str, numpg);
}

void CSynthesizer::GetBankName(char *str)
{
    programs.GetBankName(str);
}

int CSynthesizer::GetBankCount()
{
    return programs.GetBankCount();
}

int CSynthesizer::GetBankIndex()
{
    return programs.GetBankIndex();
}

void CSynthesizer::SetBankIndex(int nbank)
{
    programs.SetBankIndex(nbank);
    UpdateGlobalEffects();
}

void CSynthesizer::SetNumProgr(char channel, unsigned char numprog)
{
    programs.SetNumProgr(channel,numprog);
    for (char i=0; i<POLIPHONY; i++)
    {
        if (state[i] == ACTIVE)
        {
            if (this->channels[i] == channel)
            {
                notes[i].UpdateProgram();
            }
        }
    }
}

void CSynthesizer::StoreProgram(char channel)
{
    programs.StoreProgram(channel);
}

SBank* CSynthesizer::GetBank()
{
    return programs.GetBank();
}

void CSynthesizer::CopyProgram(int destination, int source)
{
    programs.CopyProgram(destination, source);
}

void CSynthesizer::SetBank(SBank *bank)
{
    programs.SetBank(bank);
    UpdateGlobalEffects();
}

void CSynthesizer::SetProgram(char numprg, SProgram *program)
{
    programs.SetProgram(numprg, program);
}

bool CSynthesizer::HasChanges()
{
    return programs.HasChanges();
}

void CSynthesizer::SetEditorHn(void *hWndEditor)
{
    programs.SetEditorHn(hWndEditor);
}

#endif
