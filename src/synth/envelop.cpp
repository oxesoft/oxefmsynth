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
#include "envelop.h"
#include <math.h>

/*
It need to be done to keep previous compatibility.
The original version of this code (before to be public) made an envelop
interpolation that limited each state to 32 samples, causing a side
effect where the value choosen is smaller than the actual time calculated.
If we remove this, the times will respect the program values from
0.000075 seconds (3 samples), changing the sounds of the original programs.
*/
#define KEEP_OLD_BEHAVIOUR if (!ss && counter < 32) counter = 32;

void CEnvelop::Init()
{
    sr         = 0.f;
    es         = INACTIVE;
    counter    = 0;
    coef       = 0;
    sa         = 0;
    dl         = 0.f;
    at         = 0.f;
    de         = 0.f;
    st         = 0.f;
    su         = 0.f;
    re         = 0.f;
    ss         = 0.f;
}

void CEnvelop::SetPar(char param, float value)
{
    switch (param)
    {
        case SAMPLERATE:
            sr = value;
            break;
        case DELAY:
            dl = value;
            break;
        case ATTACK:
            at = value;
            break;
        case DECAY:
            de = value;
            break;
        case SUSTAIN:
            su = value;
            break;
        case SUSTAINTIME:
            st = value;
            break;
        case RELEASE:
            re = value;
            break;
        case SINGLESAMPLEMODE:
            ss = value;
            break;
    }
}

void CEnvelop::SendEvent(char event, int remainingSamples)
{
    switch (event)
    {
        case NOTEON:
            if      (dl)
                es = DELAY;
            else if (at)
                es = ATTACK;
            else if (de)
                es = DECAY;
            else if (su)
                es = SUSTAIN;
            else
                es = ENDED;
            break;
        case NOTEOFF:
            counter = remainingSamples;
            if (re)
                es = RELEASE;
            else
                es = ENDED;
            break;
    }
}

int CEnvelop::CalcCoef()
{
    if (counter)
        return counter;
    switch (es)
    {
        case DELAY:
            counter = lrintf(dl * sr);
            KEEP_OLD_BEHAVIOUR
            coef     = 0;
            if (at)
                es = ATTACK;
            else if (de)
                es = DECAY;
            else if (su)
                es = SUSTAIN;
            else
                es = ENDED;
            break;
        case ATTACK:
            counter = lrintf(at * sr);
            KEEP_OLD_BEHAVIOUR
            if (counter < 1)
            {
                counter = 1;
            }
            coef = MAXINT / counter;
            if (de)
                es = DECAY;
            else if (su)
                es = SUSTAIN;
            else
                es = ENDED;
            break;
        case DECAY:
            counter = lrintf(de * sr);
            KEEP_OLD_BEHAVIOUR
            if (counter < 1)
            {
                counter = 1;
            }
            coef     = lrintf((-1.f + su) / counter * FMAXINT);
            sa       = MAXINT;
            if (su)
                es = SUSTAIN;
            else
                es = RELEASE;
            break;
        case SUSTAIN:
            if (st)
            {
                counter = lrintf(st * sr);
                KEEP_OLD_BEHAVIOUR
                if (counter < 1)
                {
                    counter = 1;
                }
                coef     = lrintf(-su / counter * FMAXINT);
                es       = RELEASE;
            }
            else
            {
                counter = MAXINT;
                coef    = 0;
            }
            sa = lrintf(su*FMAXINT);
            break;
        case RELEASE:
            counter = lrintf(re * sr);
            KEEP_OLD_BEHAVIOUR
            if (counter < 1)
            {
                counter = 1;
            }
            coef     = -sa / counter;
            es = ENDED;
            break;
        case ENDED:
            es       = INACTIVE;
            counter  = MAXINT;
            coef     = 0;
            sa       = 0;
            break;
    }
    return counter;
}

float CEnvelop::Process()
{
    if (!counter)
        CalcCoef();
    counter--;
    sa  += coef;
    float  temp  = float(sa>>16);
           temp /= 32768.f      ;
           temp *= temp         ;
    return temp                 ;
}

char CEnvelop::GetState()
{
    return es;
}

void CEnvelop::Process(int *b, int size, int offset, float volume)
{
    int temp;
    int vol = lrintf(volume * 127.f);
    while (counter <= size - offset)
    {
        for (int i = offset; i < offset + counter; i++)
        {
            sa  += coef           ;
            temp = sa>>16         ;
            temp = (temp*temp)>>15;
            b[i] = (b[i] * vol)>>7;
            b[i] = (temp*b[i])>>15;
        }
        offset += counter;
        counter = 0;
        CalcCoef();
    }
    for (int i = offset; i < size; i++)
    {
        sa  += coef           ;
        temp = sa>>16         ;
        temp = (temp*temp)>>15;
        b[i] = (b[i] * vol)>>7;
        b[i] = (temp*b[i])>>15;
    }
    counter -= size - offset;
}
