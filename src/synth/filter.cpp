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
#include "filter.h"
#include <math.h>

void CFilter::Init()
{
    sr      = 0.f;
    tf      = 0;
    fr      = 0.f;
    fr1     =-1.f;
    re      = 0.f;
    re1     =-1.f;
    en      = 1.f;
    b0a0    = 0;
    b1a0    = 0;
    b2a0    = 0;
    a1a0    = 0;
    a2a0    = 0;
    ou1     = 0;
    ou2     = 0;
    in1     = 0;
    in2     = 0;
}

void CFilter::SetPar(char param, float value)
{
    switch (param)
    {
        case SAMPLERATE:
            sr = value;
            break;
        case ENVELOP:
            en = powf(2.f,value)-1.f;
            break;
        case TYPE:
            tf = (char)lrintf(value);
            break;
        case FREQUENCY:
            fr = value;
            break;
        case RESONANCE:
            re = value; if (re < 0.001f) re = 0.001f;
            break;
    }
    // Initializes coefficients
    CalcCoef(tf,fr*en,re);
}    

void CFilter::Process(int *b, int size, int offset)
{
    float in0;
    float ou0;
    for (int i=offset;i<size;i++)
    {
        in0  = (float)b[i];
        ou0  = b0a0*in0 + b1a0*in1 + b2a0*in2 - a1a0*ou1 - a2a0*ou2;
        in2  = in1;
        in1  = in0;
        ou2  = ou1;
        ou1  = ou0;
        b[i] = lrintf(ou0);
    }
}

void CFilter::CalcCoef(int const type,double const frequencia,double const q)
{
    double a0;
    double a1;
    double a2;
    double b0;
    double b1;
    double b2;
    double       freq   = frequencia; if (freq <= C0) freq = C0;
    double const omega  = 2.0*D_PI*freq/sr;
    double const tsin   = sin(omega);
    double const tcos   = cos(omega);
    double const alpha  = tsin/(2.0*q);
    
    // lowpass
    if(type==0 || type>2)
    {
        b0=(1.0-tcos)/2.0;
        b1=1.0-tcos;
        b2=b0;
        a0=1.0+alpha;
        a1=-2.0*tcos;
        a2=1.0-alpha;
    }

    // hipass
    if(type==1)
    {
        b0=(1.0+tcos)/2.0;
        b1=-(1.0+tcos);
        b2=b0;
        a0=1.0+alpha;
        a1=-2.0*tcos;
        a2=1.0-alpha;
    }

    // bandpass csg
    if(type==2)
    {
        b0=tsin/2.0;
        b1=0.0;
        b2=-b0;
        a0=1.0+alpha;
        a1=-2.0*tcos;
        a2=1.0-alpha;
    }

    // set filter coeffs
    b0a0=float(b0/a0);
    b1a0=float(b1/a0);
    b2a0=float(b2/a0);
    a1a0=float(a1/a0);
    a2a0=float(a2/a0);
}
