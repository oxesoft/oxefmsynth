/*
Oxe FM Synth: a software synthesizer
Copyright (C) 2004-2015  Daniel Moura <oxesoft@gmail.com>

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
#include "reverb.h"
#include <cmath>
#include <cstring>
#include <stdint.h>

void CReverb::Init()
{
    sr     = 0;
    ti     = 0;
    da     = 0;
    icomb1 = 0;
    icomb2 = 0;
    icomb3 = 0;
    icomb4 = 0;
    iallp1 = 0;
    iallp2 = 0;
    memset(bcomb1,0,sizeof(bcomb1));
    memset(bcomb2,0,sizeof(bcomb2));
    memset(bcomb3,0,sizeof(bcomb3));
    memset(bcomb4,0,sizeof(bcomb4));
    memset(ballp1,0,sizeof(ballp1));
    memset(ballp2,0,sizeof(ballp2));
    state = INACTIVE;
    ou0  = 0.0f;
    in1  = 0.0f;
    in1l = 0.0f;
    ou0l = 0.0f;
    a0   = 0.0f;
    a1   = 0.0f;
    b1   = 0.0f;
}

void CReverb::SetPar(char param, float value)
{
    switch (param)
    {
        case SAMPLERATE:
            sr = value;
            break;
        case TIME:
            ti = value;
            break;
        case DAMP:
            da = value;
            break;
    }
}

void CReverb::CalcCoefLowPass(float frequencia)
{
    float w     = 2.0f * sr; 
    float fCut  = 2.0f * PI * Key2Frequency(frequencia * MAXFREQFLT);
    float Norm  = 1.0f / (fCut + w); 
    b1          = (w - fCut) * Norm;
    a0 = a1     = fCut * Norm;
}

void CReverb::Process(float *b, int size)
{
    int i        = 0;
    float ent    = 0.0f;
    float aux    = 0.0f;
    float smp    = 0.0f;
    const float feedback = ti * (127.0f / 128.0f);
    if (REVDAant != da) 
    {
        CalcCoefLowPass(da);
        REVDAant =         da;
    }
    int l_icomb1 = icomb1;
    int l_icomb2 = icomb2;
    int l_icomb3 = icomb3;
    int l_icomb4 = icomb4;
    int l_iallp1 = iallp1;
    int l_iallp2 = iallp2;
    float l_ou0 = ou0;
    float l_in1 = in1;

    for (i=0;i<size;i++)
    {
        ent  = b[i];
        // comb 1
        float c1 = bcomb1[l_icomb1];
        bcomb1[l_icomb1] = ent + c1 * feedback;
        if (++l_icomb1>=TAMCOMB1) l_icomb1 = 0;
        // comb 2
        float c2 = bcomb2[l_icomb2];
        bcomb2[l_icomb2] = ent + c2 * feedback;
        if (++l_icomb2>=TAMCOMB2) l_icomb2 = 0;
        // comb 3
        float c3 = bcomb3[l_icomb3];
        bcomb3[l_icomb3] = ent + c3 * feedback;
        if (++l_icomb3>=TAMCOMB3) l_icomb3 = 0;
        // comb 4
        float c4 = bcomb4[l_icomb4];
        bcomb4[l_icomb4] = ent + c4 * feedback;
        if (++l_icomb4>=TAMCOMB4) l_icomb4 = 0;

        smp = c1 + c2 + c3 + c4;

        // allpass 1
        aux = ballp1[l_iallp1];
        float new_allp1 = aux * feedback + smp;
        ballp1[l_iallp1] = new_allp1;
        smp = aux - new_allp1 * feedback;
        if (++l_iallp1>=TAMALLP1) l_iallp1 = 0;

        // allpass 2
        aux = ballp2[l_iallp2];
        float new_allp2 = aux * feedback + smp;
        ballp2[l_iallp2] = new_allp2;
        smp = aux - new_allp2 * feedback;
        if (++l_iallp2>=TAMALLP2) l_iallp2 = 0;

        // DC filter
        l_ou0  = smp - l_in1 + l_ou0 * (32674.0f / 32768.0f);
        l_in1  = smp;
        b[i] = l_ou0 * 0.25f;
    }
    icomb1 = l_icomb1;
    icomb2 = l_icomb2;
    icomb3 = l_icomb3;
    icomb4 = l_icomb4;
    iallp1 = l_iallp1;
    iallp2 = l_iallp2;
    ou0 = l_ou0;
    in1 = l_in1;

    if (REVDAant < 1.f)
    {
        float l_ou0l = ou0l;
        float l_in1l = in1l;
        const float l_a0 = a0;
        const float l_a1 = a1;
        const float l_b1 = b1;
        for (i=0;i<size;i++)
        {
            // low pass filter
            float in = b[i];
            l_ou0l = in * l_a0 + l_in1l * l_a1 + l_ou0l * l_b1;
            l_in1l = in;
            b[i] = l_ou0l;
        }
        ou0l = l_ou0l;
        in1l = l_in1l;
    }
    state = ACTIVE;
    if (std::abs(b[0]) < 1e-6f && std::abs(b[size>>1]) < 1e-6f && std::abs(b[size>>2]) < 1e-6f && std::abs(b[size-1]) < 1e-6f && std::abs(ou0) < 1e-6f && std::abs(ou0l) < 1e-6f)
    {
        state = INACTIVE;
        ou0 = 0.0f;
        in1 = 0.0f;
        ou0l = 0.0f;
        in1l = 0.0f;
    }
}

char CReverb::GetState()
{
    return state;
}

inline float CReverb::Key2Frequency(float valor)
{
    return C0 * powf(2.0f, valor / 12.0f);
}
