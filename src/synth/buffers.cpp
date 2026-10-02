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
    double size                 = (double)WAVEFORM_BSIZE;
    int    half_size            =         WAVEFORM_BSIZE / 2;
    int    quarter_size         =         WAVEFORM_BSIZE / 4;
    int    three_quarters_size  = half_size + quarter_size;
    // sine
    for (i=0;i<WAVEFORM_BSIZE;i++)
    {
        v0 = D_PI * 2.0 * (double)i / size;
        v0 = sin(v0);
        bWaves[0][i] = (float)v0;
    }
    // saw
    v0 = 1.0;
    un = 2.0 / size;
    for (i=0;i<WAVEFORM_BSIZE;i++)
    {
        v0 -= un;
        bWaves[1][i] = (float)v0;
    }
    // triangle
    un = 4.0 / size;
    v0 = 0.0;
    for (i=0;i<quarter_size;i++)
    {
        bWaves[2][i]                   = (float)(      v0);
        bWaves[2][i+quarter_size]      = (float)(1.0 - v0);
        bWaves[2][i+half_size]         = (float)(    - v0);
        bWaves[2][i+three_quarters_size] = (float)(v0 - 1.0);
        v0 += un;
    }
    // pulse
    for (i=0;i<half_size;i++)
    {
        bWaves[3][i          ] =  1.0f;
        bWaves[3][i+half_size] = -1.0f;
    }
    // band limited pulse
    Filter(3,4);
    // band limited saw
    Filter(1,5);
    Normalize(5);
    // guard sample for branchless interpolation
    for (i=0;i<WAVEFORMS;i++)
        bWaves[i][WAVEFORM_BSIZE] = bWaves[i][0];
}

void CBuffers::Filter(int source, int destination)
{
    #define N 256    // number of filters
    //-----------------------------------------
    double cutoff      = 128.f;
    double samplerate  = (double)WAVEFORM_BSIZE;
    double x           = 2.0 * D_PI * cutoff / samplerate;
    double p           = (2.0 - cos(x)) - sqrt(pow((2.0 - cos(x)), 2.0) - 1.0);
    double one_minus_p = 1.0 - p;
    double tmp[N];
    memset(tmp,0,sizeof(tmp));
    //-----------------------------------------
    double inout = 0.0;
    int   cycles = 2;
    int   i;
    int   n;
    while (cycles--)
    {
        for (i=0;i<WAVEFORM_BSIZE;i++)
        {    
            inout = (double)bWaves[source][i];
            for (n=0;n<N;n++)
            {
                //-----------------------------
                tmp[n] = one_minus_p * inout + p * tmp[n];
                inout = tmp[n];
                //-----------------------------
            }
            bWaves[destination][i] = (float)inout;
        }
    }
}

void CBuffers::Normalize(int wave_index)
{
    double max = 0.0;
    double aux = 0.0;
    int   i;
    // finds the peak value
    for (i=0;i<WAVEFORM_BSIZE;i++)
    {
        aux = fabs((double)bWaves[wave_index][i]);
        if (aux > max)
            max = aux;
    }
    // calculates the multiplication factor
    aux = 1.0/max;
    // normalizes the signal
    for (i=0;i<WAVEFORM_BSIZE;i++)
        bWaves[wave_index][i] = (float)((double)bWaves[wave_index][i]*aux);
}
