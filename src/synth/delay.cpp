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
#include "oscillator.h"
#include "buffers.h"
#include "delay.h"
#include <string.h>
#include <math.h>

///////////////////////////////////
//#define WITH_LINEAR_INTERPOLATION
///////////////////////////////////

void CDelay::Init(float *b)
{
    ti = 0.f;
    fe = 0.f;
    lf = 0.f;
    la = 0.f;
    idelay = 0;
    prev_time = 0;
    memset(bdelay,0,sizeof(bdelay));
    osc.SetBuffer(BWAVE,b);
}

void CDelay::SetPar(char param, float value)
{
    switch (param)
    {
        case SAMPLERATE:
            osc.SetPar(SAMPLERATE, value);
            break;
        case TIME:
            ti = value;
            break;
        case FEEDBACK:
            fe = value;
            break;
        case RATE:
            lf = value;
            break;
        case AMOUNT:
            la = value;
            break;
    }
}

void CDelay::Process(float *b, int size)
{
    float          lfo;
    int            time;
    int            step;
    unsigned short tmp;
    // sets the LFO that acts about the delay time
    lfo = 1.f;
    if (lf)
    {
        osc.SetPar(FREQUENCY, lf * (float)size);
        lfo -= ((osc.Process() + 1.f)/2.f) * la * 0.25f;
    }
    // sets the time delay
    time = lrintf(ti * 65535.f * 32768.f * lfo);
    // actual process
    if (fe && ((!lf) || (!la)))
    {
        time = time>>15;
        for (int i=0;i<size;i++)
        {
            tmp            = idelay-(unsigned short)time;
            bdelay[idelay] = b[i] + (bdelay[tmp] * fe);
            b[i]           = bdelay[tmp];
            idelay++;
        }
    }
    else if (!fe && ((!lf) || (!la)))
    {
        time = time>>15;
        for (int i=0;i<size;i++)
        {
            tmp            = idelay-(unsigned short)time;
            bdelay[idelay] = b[i];
            b[i]           = bdelay[tmp];
            idelay++;
        }
    }
    else if (fe && lf && la)
    {
        step = (time - prev_time) / size;
        for (int i=0;i<size;i++)
        {
            prev_time     += step;
            tmp            = idelay-(unsigned short)(prev_time>>15);
            bdelay[idelay] = b[i] + (bdelay[tmp] * fe);
            #ifdef WITH_LINEAR_INTERPOLATION
                float frac = (float)(65536 - ((int)prev_time & 0xFFFF)) * (1.0f / 65536.0f);
                b[i]       = bdelay[tmp] + (bdelay[(tmp+1) & 0xFFFF] - bdelay[tmp]) * frac;
            #else
                b[i]       = bdelay[tmp];
            #endif
            idelay++;
        }
    }
    else if (!fe && lf && la)
    {
        step = (time - prev_time) / size;
        for (int i=0;i<size;i++)
        {
            prev_time     += step;
            tmp            = idelay-(unsigned short)(prev_time>>15);
            bdelay[idelay] = b[i];
            #ifdef WITH_LINEAR_INTERPOLATION
                float frac = (float)(65536 - ((int)prev_time & 0xFFFF)) * (1.0f / 65536.0f);
                b[i]       = bdelay[tmp] + (bdelay[(tmp+1) & 0xFFFF] - bdelay[tmp]) * frac;
            #else
                b[i]       = bdelay[tmp];
            #endif
            idelay++;
        }
    }
    prev_time = time;
}
