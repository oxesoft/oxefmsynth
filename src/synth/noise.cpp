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
#include "noise.h"
#include <math.h>

void CNoise::Init()
{
    sr   = 0.f;
    fr   = 0.f;
    fr1  =-1.f;
    re   = 0.f;
    re1  =-1.f;
    am   = 128;
    bp   = 0;
    b0a0 = 0;
    b2a0 = 0;
    a1a0 = 0;
    a2a0 = 0;
    ou1  = 0;
    ou2  = 0;
    in1  = 0;
    in2  = 0;
}

void CNoise::SetPar(char param, float value)
{
    switch (param)
    {
        case SAMPLERATE:
            sr = value;
            break;
        case FREQUENCY:
            fr = value;
            break;
        case RESONANCE:
            re = value;
            break;
        case VOLUME:
            am = lrintf(value * 128.f);
            break;
        case BYPASS:
            bp = lrintf(value);
            break;
    }
    // initializes coefficients
    CalcCoef(fr,re);
}

void CNoise::Process(int *b, int size, int offset)
{
    short in0;
    int   ou0;
    int   i;
    // bypasses the signal if the key is on
    if (!bp)
    {
        for (i=offset;i<size;i++)
        {
            // Limitador
            if (b[i]> 4096)
                b[i]= 4096;
            if (b[i]<-4096)
                b[i]=-4096;
            b[i]<<=3;
        }
    }
    // generates noise if there is volume
    if (am)
    {
        // recalc coeffs if they have changed
        if (fr1 != fr || re1 != re)
        {
            CalcCoef(fr,re);
            fr1 = fr;
            re1 = re;
        }
        for (i=offset;i<size;i++)
        {
            // pseudo random generator
            static unsigned int randSeed = 22222;
            randSeed = (randSeed * 196314165) + 907633515;
            // band-pass filter
            in0  = (short)(randSeed>>16);
            ou0  = ((b0a0*in0)>>15) + ((b2a0*in2)>>15) - ((a1a0*ou1)>>15) - ((a2a0*ou2)>>15);
            in2  = in1;
            in1  = in0;
            ou2  = ou1;
            ou1  = ou0;
            b[i]+= (ou0*am)>>7;
        }
    }
}

void CNoise::CalcCoef(double const frequencia,double const q)
{
    double a0;
    double a1;
    double a2;
    double b0;
    double b2;
    double       freq   = frequencia; if (freq <= C0) freq = C0;
    double const omega  = 2.0*D_PI*freq/sr;
    double const tsin   = sin(omega);
    double const tcos   = cos(omega);
    double const alpha  = tsin/(2.0*q);

    // bandpass czpg
    b0=tsin/2.0;
    b2=-b0;
    a0=1.0+alpha;
    a1=-2.0*tcos;
    a2=1.0-alpha;

    // set filter coeffs
    b0a0=lrintf(float((b0/a0)*32768.0));
    b2a0=lrintf(float((b2/a0)*32768.0));
    a1a0=lrintf(float((a1/a0)*32768.0));
    a2a0=lrintf(float((a2/a0)*32768.0));
}
