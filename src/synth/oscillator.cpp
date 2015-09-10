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
#include <math.h>

#define INPUT_AMOUNT 537
#define INPUT_SHIFT  (WAVEFORM_BIT_DEPTH-7)

void COscillator::Init()
{
	sr      = 0.f;
	fr      = 0.f;
	tu      = 1.f;
	pt      = 1.f;
	phase   = 0;
	prevout = 0;
	self    = 0;
	bwave   = NULL;
	hq      = true;
	freq    = 0;
}

void COscillator::SetPar(char param, float value)
{
	switch (param)
	{
		case SAMPLERATE:
			sr   = value;
			break;
		case FREQUENCY:
			fr   = value;
			break;
		case TUNING:
			tu   = value;
			break;
		case PITCH:
			pt = value;
			break;
		case SELFOSC:
			self = lrintf(value * 256.f);
			break;
		case INTERPOLATION:
			hq = value?true:false;
			break;
	}
	// initializes coefficients
	freq = lrintf(65536.f * fr * tu * pt * ((float)WAVEFORM_BSIZE) / sr);
}

void COscillator::SetBuffer(char param, short *b)
{
	switch (param)
	{
		case BWAVE:
			bwave = b;
			break;
		default:
			break;
	}
}

void COscillator::Process(int *b, int size, int offset)
{
	int i;
	int prevphase;
	int l1;
	int l2;
	if (!hq)
	{
		for (i=offset;i<size;i++)
		{
			prevphase  = phase + ((b[i] * INPUT_AMOUNT) << INPUT_SHIFT) + ((prevout * self) << INPUT_SHIFT);
			prevphase &= ((WAVEFORM_BSIZE << 16) - 1);
			phase     += freq;
			phase     &= ((WAVEFORM_BSIZE << 16) - 1);
			prevout    = bwave[prevphase>>16];
			b[i]       = prevout;
		}
	}
	else
	{
		for (i=offset;i<size;i++)
		{
			prevphase  = phase + ((b[i] * INPUT_AMOUNT) << INPUT_SHIFT) + ((prevout * self) << INPUT_SHIFT);
			prevphase &= ((WAVEFORM_BSIZE << 16) - 1);
			phase     += freq;
			phase     &= ((WAVEFORM_BSIZE << 16) - 1);
			l1         = prevphase>>16;
			l2         = (l1+1) & (WAVEFORM_BSIZE - 1);
			prevout    = (short)(bwave[l1] + (((bwave[l2] - bwave[l1]) * (prevphase & 0xFFFF))>>16));
			b[i]       = prevout;
		}
	}
}

float COscillator::Process() // without input, with interpolation, for LFO
{
	int l1  = phase>>16;
	int l2  = (l1+1) & (WAVEFORM_BSIZE - 1);
	phase  += freq;
	phase  &= ((WAVEFORM_BSIZE << 16) - 1);
	int sa  = bwave[l1] + (((bwave[l2] - bwave[l1]) * (phase & 0xFFFF))>>16);
	return float(sa) / 32768.f;
}
