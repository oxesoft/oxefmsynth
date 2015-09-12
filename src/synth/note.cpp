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
#include "persist.h"
#include "programs.h"
#include "oscillator.h"
#include "envelop.h"
#include "noise.h"
#include "filter.h"
#include "buffers.h"
#include "note.h"
#include <string.h>
#include <math.h>

/*
TODO:
1) When create a buffer for each channel (to apply effects in) move the VOL and PAN calculations to it (lvolL, lvolR)
2) To make only one pitch var, commom to all oscs
*/

void CNote::Init(SProgram *prg, CBuffers *buf, unsigned char key, unsigned char previousKey, float velocity, float samplerate)
{
    this->samplerate    = samplerate;
    float previousFreq  = Key2Frequency(previousKey);
          freqnote      = Key2Frequency(key);
    buffers    = buf;
    program    = prg;
    ptc        = 1.0f;
    aft        = 0.0f;
    lfodph     = 0.0f;
    state      = INACTIVE;
    freqAtual  = freqnote;
    portaFator = 1.0f;
    portaCont  = 0;
    curvAtual  = 1.0f;
    curvFator  = 1.0f;
    curvCont   = 0;
    lastpos    = 0;
    memset(opstate, 0, sizeof(opstate));
    lpan       = 0;
    lvol       = 127;
    opAvol     = Scaling(key,program->OPAKS) * VelSen(program->OPAVS,velocity);
    opBvol     = Scaling(key,program->OPBKS) * VelSen(program->OPBVS,velocity);
    opCvol     = Scaling(key,program->OPCKS) * VelSen(program->OPCVS,velocity);
    opDvol     = Scaling(key,program->OPDKS) * VelSen(program->OPDVS,velocity);
    opEvol     = Scaling(key,program->OPEKS) * VelSen(program->OPEVS,velocity);
    opFvol     = Scaling(key,program->OPFKS) * VelSen(program->OPFVS,velocity);
    opXvol     = Scaling(key,program->OPXKS) * VelSen(program->OPXVS,velocity);
    opZvol     = Scaling(key,program->OPZKS) * VelSen(program->OPZVS,velocity);

    // Initializes the portamento constants
    if (program->PORTA)
    {
        if (key != previousKey && previousKey != 255)
        {
            portaCont = (int)lrintf(program->PORTA * samplerate / SAMPLES_PER_PROCESS);
            freqAtual = previousFreq;
            if (portaCont)
                portaFator = powf(freqnote / previousFreq, 1.0f / portaCont);
            else
                freqAtual = freqnote;
        }
        else
            freqAtual = freqnote;
    }
    // Initializes the pitch curve constants
    if (program->PTCTI && program->PTCCU != 1.f)
    {
        curvCont  = (int)lrintf(program->PTCTI * samplerate / SAMPLES_PER_PROCESS);
        curvAtual = program->PTCCU;
        if (curvCont)
            curvFator = powf(1.f / curvAtual, 1.0f / curvCont);
    }
    UpdateProgram();
}

void CNote::UpdateProgram(void)
{
    float OPCU = 0.0f; // OPX OPZ
    char  i;
    char  OPON = 0;
    char  OPWF = 0;
    char  OPCT = 0;
    float OPFT = 0.0f;
    float OPKT = 0;
    float OPDL = 0.0f;
    float OPAT = 0.0f;
    float OPDE = 0.0f;
    float OPRE = 0.0f;
    float OPSU = 0.0f;
    float OPST = 0.0f;
    float SELF = 0.0f;
    float OPRS = 0.0f; // OPX OPZ
    char  OPBP = 0;    // OPX
    float OPAM = 0.0f; // OPX
    float OPTY = 0.0f; // OPZ
    float OPRA = 0.0f; // LFO
    float OPSS = 0.0f; // LFO OPZ
    for(i = 0; i <= 8; i++)
    {
        switch (i)
        {
            case 0: // OPERATOR A
                OPON =    (char)lrintf(program->OPAON);
                OPWF =    (char)lrintf(program->OPAWF);
                OPCT =    (char)lrintf(program->OPACT);
                OPFT =                 program->OPAFT;
                OPKT =                 program->OPAKT;
                OPDL =                 program->OPADL;
                OPAT =                 program->OPAAT;
                OPDE =                 program->OPADE;
                OPST =                 program->OPAST;
                OPSU =                 program->OPASU;
                OPRE =                 program->OPARE;
                SELF =                 program->MAA;
                break;
            case 1: // OPERATOR B
                OPON =    (char)lrintf(program->OPBON);
                OPWF =    (char)lrintf(program->OPBWF);
                OPCT =    (char)lrintf(program->OPBCT);
                OPFT =                 program->OPBFT;
                OPKT =                 program->OPBKT;
                OPDL =                 program->OPBDL;
                OPAT =                 program->OPBAT;
                OPDE =                 program->OPBDE;
                OPSU =                 program->OPBSU;
                OPST =                 program->OPBST;
                OPRE =                 program->OPBRE;
                SELF =                 program->MBB;
                break;
            case 2: // OPERATOR C
                OPON =    (char)lrintf(program->OPCON);
                OPWF =    (char)lrintf(program->OPCWF);
                OPCT =    (char)lrintf(program->OPCCT);
                OPFT =                 program->OPCFT;
                OPKT =                 program->OPCKT;
                OPDL =                 program->OPCDL;
                OPAT =                 program->OPCAT;
                OPDE =                 program->OPCDE;
                OPSU =                 program->OPCSU;
                OPST =                 program->OPCST;
                OPRE =                 program->OPCRE;
                SELF =                 program->MCC;
                break;
            case 3: // OPERATOR D
                OPON =    (char)lrintf(program->OPDON);
                OPWF =    (char)lrintf(program->OPDWF);
                OPCT =    (char)lrintf(program->OPDCT);
                OPFT =                 program->OPDFT;
                OPKT =                 program->OPDKT;
                OPDL =                 program->OPDDL;
                OPAT =                 program->OPDAT;
                OPDE =                 program->OPDDE;
                OPSU =                 program->OPDSU;
                OPST =                 program->OPDST;
                OPRE =                 program->OPDRE;
                SELF =                 program->MDD;
                break;
            case 4: // OPERATOR E
                OPON =    (char)lrintf(program->OPEON);
                OPWF =    (char)lrintf(program->OPEWF);
                OPCT =    (char)lrintf(program->OPECT);
                OPFT =                 program->OPEFT;
                OPKT =                 program->OPEKT;
                OPDL =                 program->OPEDL;
                OPAT =                 program->OPEAT;
                OPDE =                 program->OPEDE;
                OPSU =                 program->OPESU;
                OPST =                 program->OPEST;
                OPRE =                 program->OPERE;
                SELF =                 program->MEE;
                break;
            case 5: // OPERATOR F
                OPON =    (char)lrintf(program->OPFON);
                OPWF =    (char)lrintf(program->OPFWF);
                OPCT =    (char)lrintf(program->OPFCT);
                OPFT =                 program->OPFFT;
                OPKT =                 program->OPFKT;
                OPDL =                 program->OPFDL;
                OPAT =                 program->OPFAT;
                OPDE =                 program->OPFDE;
                OPSU =                 program->OPFSU;
                OPST =                 program->OPFST;
                OPRE =                 program->OPFRE;
                SELF =                 program->MFF;
                break;
            case 6: // OPERATOR X
                OPON =    (char)lrintf(program->OPXON);
                OPCU =                 program->OPXCU;
                OPRS =                 program->OPXRS;
                OPAM =                 program->OPXAM;
                OPBP =    (char)lrintf(program->OPXBP);
                OPDL =                 program->OPXDL;
                OPAT =                 program->OPXAT;
                OPDE =                 program->OPXDE;
                OPSU =                 program->OPXSU;
                OPST =                 program->OPXST;
                OPRE =                 program->OPXRE;
                break;
            case 7: // OPERATOR Z
                OPON =    (char)lrintf(program->OPZON);
                OPCU =                 program->OPZCU;
                OPRS =                 program->OPZRS;
                OPTY =                 program->OPZTY;
                OPKT =                 program->OPZKT;
                OPDL =                 program->OPZDL/SAMPLES_PER_PROCESS;
                OPAT =                 program->OPZAT/SAMPLES_PER_PROCESS;
                OPDE =                 program->OPZDE/SAMPLES_PER_PROCESS;
                OPSU =                 program->OPZSU;
                OPST =                 program->OPZST/SAMPLES_PER_PROCESS;
                OPRE =                 program->OPZRE/SAMPLES_PER_PROCESS;
                OPSS =                 1.f;
                break;
            case 8: // LFO
                OPON =          (char)(program->LFODE && program->LFORA);
                OPWF =    (char)lrintf(program->LFOWF);
                OPRA =                 program->LFORA;
                OPDL =                 0.f;
                OPAT =                 program->LFODL/SAMPLES_PER_PROCESS;
                OPDE =                 0.f;
                OPSU =                 1.f;
                OPST =                 0.f;
                OPRE =                 0.f;
                OPSS =                 1.f;
                break;
        }
        if (OPON)
        {
            // OPERATORS A to F
            if (i < 6)
            {
                if (!opstate[i])
                {
                    osc[i].Init();
                }
                osc[i].SetBuffer(BWAVE,buffers->bWaves[OPWF]);
                osc[i].SetPar(SAMPLERATE   , samplerate);
                osc[i].SetPar(SELFOSC      , SELF);
                osc[i].SetPar(INTERPOLATION, program->HQ);
                if (OPKT)
                {
                    float TU = 0.0f;
                    TU = (float)(OPCT - 50);
                    TU = Val2Mul(TU);
                    osc[i].SetPar(TUNING, TU*OPFT);
                }
                else
                {
                    osc[i].SetPar(FREQUENCY, OPFT * Key2Frequency((char)((float)OPCT / MAXPARVALUE * MAXFREQOSC)));
                }
            }
            // OPERATOR X
            if (i == 6)
            {
                if (!opstate[i])
                {
                    noise.Init();
                }
                OPCU = Key2Frequency((char)lrintf((float)OPCU / MAXPARVALUE * MAXFREQFLT));
                noise.SetPar(SAMPLERATE, samplerate);
                noise.SetPar(FREQUENCY , OPCU);
                noise.SetPar(RESONANCE , OPRS);
                noise.SetPar(VOLUME    , OPAM);
                noise.SetPar(BYPASS    , OPBP);
            }
            // OPERATOR Z
            if (i == 7)
            {
                if (!opstate[i])
                {
                    filter.Init();
                }
                OPCU = Key2Frequency((char)lrintf((float)OPCU / MAXPARVALUE * MAXFREQFLT));
                filter.SetPar(SAMPLERATE, samplerate);
                filter.SetPar(TYPE      , OPTY);
                filter.SetPar(FREQUENCY , OPCU);
                filter.SetPar(RESONANCE , OPRS);
            }
            // LFO
            if (i == 8)
            {
                if (!opstate[i])
                {
                    lfoosc.Init();
                }
                lfoosc.SetBuffer(BWAVE,buffers->bWaves[OPWF]);
                lfoosc.SetPar(SAMPLERATE, samplerate);
                lfoosc.SetPar(FREQUENCY , OPRA*(float)SAMPLES_PER_PROCESS);
                lfodph = program->LFODE;
            }
            if (!opstate[i])
            {
                env[i].Init();
            }
            env[i].SetPar(SAMPLERATE      , samplerate);
            env[i].SetPar(DELAY           , OPDL);
            env[i].SetPar(ATTACK          , OPAT);
            env[i].SetPar(DECAY           , OPDE);
            env[i].SetPar(SUSTAIN         , OPSU);
            env[i].SetPar(SUSTAINTIME     , OPST);
            env[i].SetPar(RELEASE         , OPRE);
            env[i].SetPar(SINGLESAMPLEMODE, OPSS);
            if (!opstate[i])
            {
                env[i].SendEvent(NOTEON, 0);
            }
            opstate[i] = 1;
        }
        else
        {
            opstate[i] = 0;
        }
    }
}

void CNote::SendEvent(char param, float value, int position)
{
    switch (param)
    {
        case NOTEON:
            startPosition = position;
            break;
        case NOTEOFF:
        {
            int remainingSamples = position - lastpos;
            if (remainingSamples < 0)
            {
                remainingSamples = 0;
            }
            for (int i = 0; i < MAXOPER; i++)
            {
                if (opstate[i])
                {
                    env[i].SendEvent(NOTEOFF, remainingSamples);
                }
            }
            break;
        }
        case PITCH:
            ptc = value;
            break;
        case AFTERTOUCH:
            aft = value;
            break;
        case MODULATION:
            switch (lrintf(program->MDLDS))
            {
                case 0: // LFO Depth
                    lfodph = program->LFODE * value;
                    break;
                case 1: // LFO Rate
                    if (opstate[8])
                    {
                        lfoosc.SetPar(FREQUENCY, value * program->LFORA * (float)SAMPLES_PER_PROCESS);
                    }
                    break;
                case 2: // Filter Cuttof
                    if (opstate[7])
                    {
                        filter.SetPar(FREQUENCY, Key2Frequency((char)lrintf(value * program->OPZCU / MAXPARVALUE * MAXFREQFLT)));
                    }
                    break;
                case 3: // LFO Depth Invert
                    lfodph = program->LFODE * (1.f - value);
                    break;
                case 4: // LFO Rate Invert
                    if (opstate[8])
                    {
                        lfoosc.SetPar(FREQUENCY, (1.f - value) * program->LFORA * (float)SAMPLES_PER_PROCESS);
                    }
                    break;
                case 5: // Filter Cuttof Invert
                    if (opstate[7])
                    {
                        filter.SetPar(FREQUENCY, Key2Frequency((char)lrintf((1.f - value) * program->OPZCU / MAXPARVALUE * MAXFREQFLT)));
                    }
                    break;
            }
            break;
        case PAN:
            lpan = lrintf(value * 127.f);
            break;
        case VOLUME:
            lvol = lrintf(value * 127.f);
            break;
    }
}

void CNote::Process(int *bsaida, int size, int position)
{
    float freqLocal;
    float curvLocal;
    float opXvll = opXvol;
    float opZvll = opZvol;
    int i;
    int offset = 0;
    
    // returns if there is no signal generators active
    if (!opstate[0] && !opstate[1] && !opstate[2] && !opstate[3] && !opstate[4] && !opstate[5] && !opstate[6])
    {
        state = INACTIVE;
        return;
    }
    
    // if the note start is not in this chunk, exits and set the note state as active to the synth do not detroy it
    if (startPosition > (position + size))
    {
        state = ACTIVE;
        return;
    }
    
    // if the note starts in the middle of this chunk, calculates the offset
    if (startPosition > 0 && startPosition < (position + size))
    {
        offset = startPosition - position;
    }

    // portamento
    if (portaCont)
    {
        portaCont--;
        freqAtual *= portaFator;
    }
    else
    {
        freqAtual = freqnote;
    }
    freqLocal = freqAtual;

    // pitch bend
    freqLocal *= ptc;

    // pitch curve
    if (curvCont)
    {
        curvCont--;
        curvAtual *= curvFator;
    }
    else
    {
        curvAtual = 1.f;
    }
    curvLocal = curvAtual;

    // LFO
    if (opstate[8])
    {
        float lfo = lfoosc.Process() * env[8].Process() * lfodph;
        switch (lrintf(program->LFODS))
        {
            case 0:
                curvLocal *= (lfo + 1.f);
                break;
            case 1:
                opXvll    *= (lfo + 1.f) / 2.f;
                break;
            case 2:
                opZvll    *= (lfo + 1.f) / 2.f;
                break;
        }
    }

    // sets the pitch's multiplier of the oscillators
    for (i = 0; i < 6; i++)
    {
        if (opstate[i])
        {
            osc[i].SetPar(PITCH, curvLocal);
        }
    }

    // sets the frequency
    if (opstate[0] && program->OPAKT)
    {
        osc[0].SetPar(FREQUENCY, freqLocal);
    }
    if (opstate[1] && program->OPBKT)
    {
        osc[1].SetPar(FREQUENCY, freqLocal);
    }
    if (opstate[2] && program->OPCKT)
    {
        osc[2].SetPar(FREQUENCY, freqLocal);
    }
    if (opstate[3] && program->OPDKT)
    {
        osc[3].SetPar(FREQUENCY, freqLocal);
    }
    if (opstate[4] && program->OPEKT)
    {
        osc[4].SetPar(FREQUENCY, freqLocal);
    }
    if (opstate[5] && program->OPFKT)
    {
        osc[5].SetPar(FREQUENCY, freqLocal);
    }

    // operator A
    if (opstate[0])
    {
        memset(buffers->bOPA,0,sizeof(buffers->bOPA));
        osc[0].Process(buffers->bOPA,size,offset);
        env[0].Process(buffers->bOPA,size,offset,opAvol);
    }

    // operator B
    if (opstate[1])
    {
        memset(buffers->bOPB,0,sizeof(buffers->bOPB));
        if (opstate[0] && program->MAB)
            SumMonoMono(buffers->bOPA,buffers->bOPB,program->MAB,size,offset);
        osc[1].Process(buffers->bOPB,size,offset);
        env[1].Process(buffers->bOPB,size,offset,opBvol);
    }

    // operator C
    if (opstate[2])
    {
        memset(buffers->bOPC,0,sizeof(buffers->bOPC));
        if (opstate[0] && program->MAC)
            SumMonoMono(buffers->bOPA,buffers->bOPC,program->MAC,size,offset);
        if (opstate[1] && program->MBC)
            SumMonoMono(buffers->bOPB,buffers->bOPC,program->MBC,size,offset);
        osc[2].Process(buffers->bOPC,size,offset);
        env[2].Process(buffers->bOPC,size,offset,opCvol);
    }

    // operator D
    if (opstate[3])
    {
        memset(buffers->bOPD,0,sizeof(buffers->bOPD));
        if (opstate[0] && program->MAD)
            SumMonoMono(buffers->bOPA,buffers->bOPD,program->MAD,size,offset);
        if (opstate[1] && program->MBD)
            SumMonoMono(buffers->bOPB,buffers->bOPD,program->MBD,size,offset);
        if (opstate[2] && program->MCD)
            SumMonoMono(buffers->bOPC,buffers->bOPD,program->MCD,size,offset);
        osc[3].Process(buffers->bOPD,size,offset);
        env[3].Process(buffers->bOPD,size,offset,opDvol);
    }
    
    // operator E
    if (opstate[4])
    {
        memset(buffers->bOPE,0,sizeof(buffers->bOPE));
        if (opstate[0] && program->MAE)
            SumMonoMono(buffers->bOPA,buffers->bOPE,program->MAE,size,offset);
        if (opstate[1] && program->MBE)
            SumMonoMono(buffers->bOPB,buffers->bOPE,program->MBE,size,offset);
        if (opstate[2] && program->MCE)
            SumMonoMono(buffers->bOPC,buffers->bOPE,program->MCE,size,offset);
        if (opstate[3] && program->MDE)
            SumMonoMono(buffers->bOPD,buffers->bOPE,program->MDE,size,offset);
        osc[4].Process(buffers->bOPE,size,offset);
        env[4].Process(buffers->bOPE,size,offset,opEvol);
    }

    // operator F
    if (opstate[5])
    {
        memset(buffers->bOPF,0,sizeof(buffers->bOPF));
        if (opstate[0] && program->MAF)
            SumMonoMono(buffers->bOPA,buffers->bOPF,program->MAF,size,offset);
        if (opstate[1] && program->MBF)
            SumMonoMono(buffers->bOPB,buffers->bOPF,program->MBF,size,offset);
        if (opstate[2] && program->MCF)
            SumMonoMono(buffers->bOPC,buffers->bOPF,program->MCF,size,offset);
        if (opstate[3] && program->MDF)
            SumMonoMono(buffers->bOPD,buffers->bOPF,program->MDF,size,offset);
        if (opstate[4] && program->MEF)
            SumMonoMono(buffers->bOPE,buffers->bOPF,program->MEF,size,offset);
        osc[5].Process(buffers->bOPF,size,offset);
        env[5].Process(buffers->bOPF,size,offset,opFvol);
    }

    // operator X
    if (opstate[6])
    {
        memset(buffers->bOPX,0,sizeof(buffers->bOPX));
        if (opstate[0]  && program->MAX)
            SumMonoMono(buffers->bOPA,buffers->bOPX,program->MAX,size,offset);
        if (opstate[1]  && program->MBX)
            SumMonoMono(buffers->bOPB,buffers->bOPX,program->MBX,size,offset);
        if (opstate[2]  && program->MCX)
            SumMonoMono(buffers->bOPC,buffers->bOPX,program->MCX,size,offset);
        if (opstate[3]  && program->MDX)
            SumMonoMono(buffers->bOPD,buffers->bOPX,program->MDX,size,offset);
        if (opstate[4]  && program->MEX)
            SumMonoMono(buffers->bOPE,buffers->bOPX,program->MEX,size,offset);
        if (opstate[5] && program->MFX)
            SumMonoMono(buffers->bOPF,buffers->bOPX,program->MFX,size,offset);
        noise.Process (buffers->bOPX,size,offset);
        env[6].Process(buffers->bOPX,size,offset,opXvll);
    }
    
    // operator Z
    if (opstate[7])
    {
        memset(buffers->bOPZ,0,sizeof(buffers->bOPZ));
        filter.SetPar(ENVELOP, env[7].Process() * opZvll);
        if (opstate[0]  && program->MAZ)
            SumMonoMono(buffers->bOPA,buffers->bOPZ,program->MAZ,size,offset);
        if (opstate[1]  && program->MBZ)
            SumMonoMono(buffers->bOPB,buffers->bOPZ,program->MBZ,size,offset);
        if (opstate[2]  && program->MCZ)
            SumMonoMono(buffers->bOPC,buffers->bOPZ,program->MCZ,size,offset);
        if (opstate[3]  && program->MDZ)
            SumMonoMono(buffers->bOPD,buffers->bOPZ,program->MDZ,size,offset);
        if (opstate[4]  && program->MEZ)
            SumMonoMono(buffers->bOPE,buffers->bOPZ,program->MEZ,size,offset);
        if (opstate[5] && program->MFZ)
            SumMonoMono(buffers->bOPF,buffers->bOPZ,program->MFZ,size,offset);
        if (opstate[6] && program->MXZ)
            SumMonoMono(buffers->bOPX,buffers->bOPZ,program->MXZ,size,offset);
        filter.Process(buffers->bOPZ,size,offset);
    }

    // sums the mono operators signal to the stereo output
    if (opstate[0] && program->MAO)
        SumMonoStereo(buffers->bOPA,bsaida,program->MAO,program->MAP,size,offset);
    if (opstate[1] && program->MBO)
        SumMonoStereo(buffers->bOPB,bsaida,program->MBO,program->MBP,size,offset);
    if (opstate[2] && program->MCO)
        SumMonoStereo(buffers->bOPC,bsaida,program->MCO,program->MCP,size,offset);
    if (opstate[3] && program->MDO)
        SumMonoStereo(buffers->bOPD,bsaida,program->MDO,program->MDP,size,offset);
    if (opstate[4] && program->MEO)
        SumMonoStereo(buffers->bOPE,bsaida,program->MEO,program->MEP,size,offset);
    if (opstate[5] && program->MFO)
        SumMonoStereo(buffers->bOPF,bsaida,program->MFO,program->MFP,size,offset);
    if (opstate[6] && program->MXO)
        SumMonoStereo(buffers->bOPX,bsaida,program->MXO,program->MXP,size,offset);
    if (opstate[7] && program->MZO)
        SumMonoStereo(buffers->bOPZ,bsaida,program->MZO,program->MZP,size,offset);

    // applies pan and volume
    PanVolStereo(bsaida,lvol,lpan,size,offset);

    // the note state is defined by the operators's envelops except the Z one
    state = INACTIVE;
    if      (opstate[0] && program->MAO && INACTIVE != env[0].GetState())
        state = ACTIVE;
    else if (opstate[1] && program->MBO && INACTIVE != env[1].GetState())
        state = ACTIVE;
    else if (opstate[2] && program->MCO && INACTIVE != env[2].GetState())
        state = ACTIVE;
    else if (opstate[3] && program->MDO && INACTIVE != env[3].GetState())
        state = ACTIVE;
    else if (opstate[4] && program->MEO && INACTIVE != env[4].GetState())
        state = ACTIVE;
    else if (opstate[5] && program->MFO && INACTIVE != env[5].GetState())
        state = ACTIVE;
    else if (opstate[6] && program->MXO && INACTIVE != env[6].GetState())
        state = ACTIVE;
    else if (opstate[7] && program->MZO && 
        (
            (opstate[0] && program->MAZ && INACTIVE != env[0].GetState()) ||
            (opstate[1] && program->MBZ && INACTIVE != env[1].GetState()) || 
            (opstate[2] && program->MCZ && INACTIVE != env[2].GetState()) || 
            (opstate[3] && program->MDZ && INACTIVE != env[3].GetState()) || 
            (opstate[4] && program->MEZ && INACTIVE != env[4].GetState()) || 
            (opstate[5] && program->MFZ && INACTIVE != env[5].GetState()) ||
            (opstate[6] && program->MXZ && INACTIVE != env[6].GetState())
        )
    )
    {
        state = ACTIVE;
    }

    lastpos = position + size;

    // from now on, the note will fill the entire buffer until it ends
    startPosition = 0;
}

char CNote::GetState()
{
    return state;
}

void CNote::SumMonoMono(int *bInput, int *bNoteOut, float volume, int size, int offset)
{
    int i;
    int vol = lrintf(volume * 127.f);
    if (vol != 127)
        for (i=offset;i<size;i++)
            bNoteOut[i] += (bInput[i] * vol)>>7;
    else
        for (i=offset;i<size;i++)
            bNoteOut[i] += bInput[i];
}

void CNote::SumMonoStereo(int *bInput, int *bNoteOut, float volume, float pan, int size, int offset)
{
    int i;
    int stemp;
    int ini   = offset<<1;
    int tam   = size<<1;
    int ptemp = lrintf(pan    * 127.f);
    int vtemp = lrintf(volume * 127.f);
    if (ptemp > 0)
    {
        ptemp = 127 - ptemp;
        for (i=ini;i<tam;i+=2)
        {
            stemp = (bInput[i>>1] * vtemp)>>7;
            bNoteOut[i]   += (stemp * ptemp)>>7;
            bNoteOut[i+1] += stemp;
        }
    }
    else if (ptemp < 0)
    {
        ptemp = 127 + ptemp;
        for (i=ini;i<tam;i+=2)
        {
            stemp = (bInput[i>>1] * vtemp)>>7;
            bNoteOut[i]   += stemp;
            bNoteOut[i+1] += (stemp * ptemp)>>7;
        }
    }
    else
    {
        for (i=ini;i<tam;i+=2)
        {
            stemp = (bInput[i>>1] * vtemp)>>7;
            bNoteOut[i]   += stemp;
            bNoteOut[i+1] += stemp;
        }
    }
}

void CNote::PanVolStereo(int *b, int volume, int pan, int size, int offset)
{
    int i;
    int i2;
    int ini   = offset<<1;
    int tam   = size<<1;
    if (volume == 127 && !pan)
        return;
    if (pan > 0)
    {
        pan = 127 - pan;
        for (i=ini;i<tam;i+=2)
        {
            i2    = i+1;
            b[i]  = (b[i]  * volume * pan)>>14;
            b[i2] = (b[i2] * volume)>>7;
        }
    }
    else if (pan < 0)
    {
        pan = 127 + pan;
        for (i=ini;i<tam;i+=2)
        {
            i2    = i+1;
            b[i]  = (b[i]  * volume)>>7;
            b[i2] = (b[i2] * volume * pan)>>14;
        }
    }
    else
    {
        for (i=ini;i<tam;i+=2)
        {
            i2    = i+1;
            b[i]  = (b[i]  * volume)>>7;
            b[i2] = (b[i2] * volume)>>7;
        }
    }
}

float CNote::Scaling(unsigned char tecla, float valor)
{
    if (!valor)
        return 1.f;
    unsigned char tclMax = 96; // last key of a 5 octave keyboard
    unsigned char tclMin = 36; // first key of a 5 octave keyboard
            float valMin = 1.f;
            float valMax = 1.f;
    if (valor > 0.f)
        valMin = 1.f - valor;
    else
    if (valor < 0.f)
        valMax = 1.f + valor;

    float volume   = valMin + (((valMax - valMin) / (float)(tclMax - tclMin)) * (float)(tecla - tclMin));
    
    if (valor > 0.f)
    {
        if (volume>valMax)
            volume = valMax;
        if (volume<valMin)
            volume = valMin;
    }
    else
    if (valor < 0.f)
    {
        if (volume<valMax)
            volume = valMax;
        if (volume>valMin)
            volume = valMin;
    }
    return volume;
}

inline float CNote::VelSen(float valor, float vel)
{
    float val = vel + (1.0f - valor);
    if (val > 1.0f)
        val = 1.0f;
    val = val*val;
    return val;
}

inline float CNote::Key2Frequency(char valor)
{
    return C0 * powf(2.0f, valor / 12.0f);
}

inline float CNote::Val2Mul(float valor)
{
    if      (valor > 0.0f)
             return powf(2.0f,  valor / 12.0f);
    else if (valor < 0.0f)
             return powf(0.5f, -valor / 12.0f);
    else
             return 1.0f;
}
