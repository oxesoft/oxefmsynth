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
#include <string.h>
#include <math.h>

CBuffers::CBuffers()
{
	memset(bOPA,         0,sizeof(bOPA));
	memset(bOPB,         0,sizeof(bOPB));
	memset(bOPC,         0,sizeof(bOPC));
	memset(bOPD,         0,sizeof(bOPD));
	memset(bOPE,         0,sizeof(bOPE));
	memset(bOPF,         0,sizeof(bOPF));
	memset(bOPX,         0,sizeof(bOPX));
	memset(bOPZ,         0,sizeof(bOPZ));
	memset(bREV,         0,sizeof(bREV));
	memset(bDLY,         0,sizeof(bDLY));
	memset(bNoteOut,     0,sizeof(bNoteOut));
	for (int i=0;i<WAVEFORMS;i++)
		memset(bWaves[i], 0, sizeof(bWaves[i]));
	FillWaveforms();
}

void CBuffers::FillWaveforms(void)
{
	int     i   = 0;
	double un   = 0.0f;
	double v0   = 0.0f;
	double v1   = 0.0f;
	double tam  = (double)WAVEFORM_BSIZE;
	int    tam2 =         WAVEFORM_BSIZE / 2;
	int    tam4 =         WAVEFORM_BSIZE / 4;
	int    ta24 = tam2 + tam4;
	// sine
	for (i=0;i<WAVEFORM_BSIZE;i++)
	{
		v0 = D_PI * 2.0 * (double)i / tam;
		v0 = sin(v0);
		bWaves[0][i] = (short)lrintf(float(v0 * 32767.0));
	}
	// saw
	v0 = 1.0;
	un = 2.0 / tam;
	for (i=0;i<WAVEFORM_BSIZE;i++)
	{
		v0 -= un;
		bWaves[1][i] = (short)lrintf(float(v0 * 32767.f));
	}
	// triangle
	un = 4.0 / tam;
	v0 = 0.0;
	for (i=0;i<tam4;i++)
	{
		bWaves[2][i]      = (short)lrintf(float((      v0) * 32767.f));
		bWaves[2][i+tam4] = (short)lrintf(float((1.f - v0) * 32767.f));
		bWaves[2][i+tam2] = (short)lrintf(float((    - v0) * 32767.f));
		bWaves[2][i+ta24] = (short)lrintf(float((v0 - 1.f) * 32767.f));
		v0 += un;
	}
	// pulse
	for (i=0;i<tam2;i++)
	{
		bWaves[3][i     ] =  32767;
		bWaves[3][i+tam2] = -32767;
	}
	// band limited pulse
	Filtrar(3,4);
	// band limited saw
	Filtrar(1,5);
	Normalizar(5);
}

void CBuffers::Filtrar(int indorigem, int inddestino)
{
	#define N 256    // number of filters
	//-----------------------------------------
	double cutoff     = 128.f;
	double samplerate = (double)WAVEFORM_BSIZE;
	double x          = 2.0 * D_PI * cutoff / samplerate;
	double p          = (2.0 - cos(x)) - sqrt(pow((2.0 - cos(x)), 2.0) - 1.0);
	double ummenosp   = 1.0 - p;
	double tmp[N];
	memset(tmp,0,sizeof(tmp));
	//-----------------------------------------
	double entsai = 0.0;
	int   ciclos = 2;
	int   i;
	int   n;
	while (ciclos--)
	{
		for (i=0;i<WAVEFORM_BSIZE;i++)
		{	
			entsai = (double)bWaves[indorigem][i];
			for (n=0;n<N;n++)
			{
				//-----------------------------
				tmp[n] = ummenosp * entsai + p * tmp[n];
				entsai = tmp[n];
				//-----------------------------
			}
			bWaves[inddestino][i] = (short)lrintf((float)entsai);
		}
	}
}

void CBuffers::Normalizar(int indice)
{
	double max = 0.0;
	double aux = 0.0;
	int   i;
	// finds the peak value
	for (i=0;i<WAVEFORM_BSIZE;i++)
	{
		aux = fabs((double)bWaves[indice][i]);
		if (aux > max)
			max = aux;
	}
	// calculates the multiplication factor
	aux = 32767.0/max;
	// normalizes the signal
	for (i=0;i<WAVEFORM_BSIZE;i++)
		bWaves[indice][i] = (short)(lrintf(float((double)bWaves[indice][i]*aux)));
}
