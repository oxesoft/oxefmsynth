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
#include "oscillator.h"
#include "buffers.h"
#include "delay.h"
#include <string.h>
#include <math.h>

///////////////////////////////////
//#define WITH_LINEAR_INTERPOLATION
///////////////////////////////////

void CDelay::Init(short *b)
{
	ti = 0.f;
	fe = 0.f;
	lf = 0.f;
	la = 0.f;
	idelay = 0;
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

void CDelay::Process(int *b, int size)
{
	float          lfo;
	int            tempo;
	int            passo;
	unsigned short tmp;
	// sets the LFO that acts about the delay time
	lfo = 1.f;
	if (lf)
	{
		osc.SetPar(FREQUENCY, lf * (float)size);
		lfo -= ((osc.Process() + 1.f)/2.f) * la * 0.25f;
	}
	// sets the time delay
	tempo = lrintf(ti * 65535.f * 32768.f * lfo);
	// actual process
	if (fe && ((!lf) || (!la)))
	{
		int feedb = lrintf(fe * 32768.f);
		    tempo = tempo>>15;
		for (int i=0;i<size;i++)
		{
			tmp            = idelay-(unsigned short)tempo;
			bdelay[idelay] = b[i] + ((bdelay[tmp] * feedb)/32768);
			b[i]           = bdelay[tmp];
			idelay++;
		}
	}
	else if (!fe && ((!lf) || (!la)))
	{
		tempo = tempo>>15;
		for (int i=0;i<size;i++)
		{
			tmp            = idelay-(unsigned short)tempo;
			bdelay[idelay] = b[i];
			b[i]           = bdelay[tmp];
			idelay++;
		}
	}
	else if (fe && lf && la)
	{
		int feedb = lrintf(fe * 32768.f);
		    passo = (tempo - tempoant) / size;
		for (int i=0;i<size;i++)
		{
			tempoant      += passo;
			tmp            = idelay-(unsigned short)(tempoant>>15);
			bdelay[idelay] = b[i] + ((bdelay[tmp] * feedb)/32768);
			#ifdef WITH_LINEAR_INTERPOLATION
				b[i]       = bdelay[tmp] + (((bdelay[(tmp+1) & 0xFFFF] - bdelay[tmp]) * (65536 - ((int)tempoant & 0xFFFF)))>>16);
			#else
				b[i]       = bdelay[tmp];
			#endif
			idelay++;
		}
	}
	else if (!fe && lf && la)
	{
		passo = (tempo - tempoant) / size;
		for (int i=0;i<size;i++)
		{
			tempoant      += passo;
			tmp            = idelay-(unsigned short)(tempoant>>15);
			bdelay[idelay] = b[i];
			#ifdef WITH_LINEAR_INTERPOLATION
				b[i]       = bdelay[tmp] + (((bdelay[(tmp+1) & 0xFFFF] - bdelay[tmp]) * (65536 - ((int)tempoant & 0xFFFF)))>>16);
			#else
				b[i]       = bdelay[tmp];
			#endif
			idelay++;
		}
	}
	tempoant = tempo;
}
