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
#include <math.h>

#define INPUT_AMOUNT 537
#define INPUT_SHIFT  (WAVEFORM_BIT_DEPTH-7)
#define FM_INPUT_SCALE (32767.0f * (float)(INPUT_AMOUNT << INPUT_SHIFT))

void COscillator::Init()
{
    sr         = 0.f;
    fr         = 0.f;
    tu         = 1.f;
    pt         = 1.f;
    phase      = 0;
    prevout    = 0.0f;
    self_scale = 0.0f;
    bwave      = NULL;
    hq         = true;
    freq       = 0;
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
            self_scale = value * (32767.0f * (float)(256 << INPUT_SHIFT));
            break;
        case INTERPOLATION:
            hq = value?true:false;
            break;
    }
    // initializes coefficients
    freq = lrintf(65536.f * fr * tu * pt * ((float)WAVEFORM_BSIZE) / sr);
}

void COscillator::SetBuffer(char param, float *b)
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

void COscillator::Process(float *b, int size, int offset, bool has_input)
{
    int i;
    int prevphase;
    int l1;
    constexpr int PHASE_MASK = (WAVEFORM_BSIZE << 16) - 1;
    constexpr float INV_65536 = 1.0f / 65536.0f;
    const float * __restrict w = bwave;
    float * __restrict buf = b;
    int local_phase = phase;
    const int local_freq = freq;
    float local_prevout = prevout;
    const float local_self = self_scale;

    if (!hq)
    {
        if (!has_input && local_self == 0.0f)
        {
            for (i=offset;i<size;i++)
            {
                prevphase     = local_phase;
                local_phase   = (local_phase + local_freq) & PHASE_MASK;
                local_prevout = w[prevphase>>16];
                buf[i]        = local_prevout;
            }
        }
        else if (local_self == 0.0f)
        {
            for (i=offset;i<size;i++)
            {
                int mod       = (int)(buf[i] * FM_INPUT_SCALE);
                prevphase     = (local_phase + mod) & PHASE_MASK;
                local_phase   = (local_phase + local_freq) & PHASE_MASK;
                local_prevout = w[prevphase>>16];
                buf[i]        = local_prevout;
            }
        }
        else if (!has_input)
        {
            for (i=offset;i<size;i++)
            {
                int mod       = (int)(local_prevout * local_self);
                prevphase     = (local_phase + mod) & PHASE_MASK;
                local_phase   = (local_phase + local_freq) & PHASE_MASK;
                local_prevout = w[prevphase>>16];
                buf[i]        = local_prevout;
            }
        }
        else
        {
            for (i=offset;i<size;i++)
            {
                int mod       = (int)(buf[i] * FM_INPUT_SCALE + local_prevout * local_self);
                prevphase     = (local_phase + mod) & PHASE_MASK;
                local_phase   = (local_phase + local_freq) & PHASE_MASK;
                local_prevout = w[prevphase>>16];
                buf[i]        = local_prevout;
            }
        }
    }
    else
    {
        if (!has_input && local_self == 0.0f)
        {
            for (i=offset; i + 1 < size; i += 2)
            {
                int p0 = local_phase;
                int p1 = (p0 + local_freq) & PHASE_MASK;
                local_phase = (p1 + local_freq) & PHASE_MASK;

                int l1_0 = p0 >> 16;
                int l1_1 = p1 >> 16;

                float frac0 = (float)(p0 & 0xFFFF) * INV_65536;
                float frac1 = (float)(p1 & 0xFFFF) * INV_65536;

                buf[i]     = w[l1_0] + frac0 * (w[l1_0 + 1] - w[l1_0]);
                buf[i + 1] = w[l1_1] + frac1 * (w[l1_1 + 1] - w[l1_1]);
            }
            if (i < size)
            {
                prevphase     = local_phase;
                local_phase   = (local_phase + local_freq) & PHASE_MASK;
                l1            = prevphase>>16;
                float frac    = (float)(prevphase & 0xFFFF) * INV_65536;
                local_prevout = w[l1] + frac * (w[l1+1] - w[l1]);
                buf[i]        = local_prevout;
            }
            else if (size > offset)
            {
                local_prevout = buf[size - 1];
            }
        }
        else if (local_self == 0.0f)
        {
            for (i=offset; i + 1 < size; i += 2)
            {
                int mod0 = (int)(buf[i] * FM_INPUT_SCALE);
                int mod1 = (int)(buf[i + 1] * FM_INPUT_SCALE);

                int p0 = (local_phase + mod0) & PHASE_MASK;
                local_phase = (local_phase + local_freq) & PHASE_MASK;
                int p1 = (local_phase + mod1) & PHASE_MASK;
                local_phase = (local_phase + local_freq) & PHASE_MASK;

                int l1_0 = p0 >> 16;
                int l1_1 = p1 >> 16;

                float frac0 = (float)(p0 & 0xFFFF) * INV_65536;
                float frac1 = (float)(p1 & 0xFFFF) * INV_65536;

                buf[i]     = w[l1_0] + frac0 * (w[l1_0 + 1] - w[l1_0]);
                buf[i + 1] = w[l1_1] + frac1 * (w[l1_1 + 1] - w[l1_1]);
            }
            if (i < size)
            {
                int mod       = (int)(buf[i] * FM_INPUT_SCALE);
                prevphase     = (local_phase + mod) & PHASE_MASK;
                local_phase   = (local_phase + local_freq) & PHASE_MASK;
                l1            = prevphase>>16;
                float frac    = (float)(prevphase & 0xFFFF) * INV_65536;
                local_prevout = w[l1] + frac * (w[l1+1] - w[l1]);
                buf[i]        = local_prevout;
            }
            else if (size > offset)
            {
                local_prevout = buf[size - 1];
            }
        }
        else if (!has_input)
        {
            for (i=offset;i<size;i++)
            {
                int mod       = (int)(local_prevout * local_self);
                prevphase     = (local_phase + mod) & PHASE_MASK;
                local_phase   = (local_phase + local_freq) & PHASE_MASK;
                l1            = prevphase>>16;
                float frac    = (float)(prevphase & 0xFFFF) * INV_65536;
                local_prevout = w[l1] + frac * (w[l1+1] - w[l1]);
                buf[i]        = local_prevout;
            }
        }
        else
        {
            for (i=offset;i<size;i++)
            {
                int mod       = (int)(buf[i] * FM_INPUT_SCALE + local_prevout * local_self);
                prevphase     = (local_phase + mod) & PHASE_MASK;
                local_phase   = (local_phase + local_freq) & PHASE_MASK;
                l1            = prevphase>>16;
                float frac    = (float)(prevphase & 0xFFFF) * INV_65536;
                local_prevout = w[l1] + frac * (w[l1+1] - w[l1]);
                buf[i]        = local_prevout;
            }
        }
    }
    phase   = local_phase;
    prevout = local_prevout;
}

float COscillator::Process() // without input, with interpolation, for LFO
{
    constexpr int PHASE_MASK = (WAVEFORM_BSIZE << 16) - 1;
    int l1     = phase>>16;
    float frac = (float)(phase & 0xFFFF) * (1.0f / 65536.0f);
    phase     += freq;
    phase     &= PHASE_MASK;
    return bwave[l1] + frac * (bwave[l1+1] - bwave[l1]);
}
