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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <strsafe.h>
#include <math.h>
#include "control.h"
#include "synthesizer.h"
#include "mapper.h"

char CMapper::FloatValueToIntValue(CSynthesizer *synthesizer, char channel, int par, int type, float fvalue)
{
    char value = 0;
    switch (type)
    {
        case VL_ZERO_TO_ONE:
            value = (char)lrintf(fvalue*MAXPARVALUE);
            break;
        case VL_MINUS1_2_PLUS1:
            value = (char)lrintf(((fvalue/2.f)*MAXPARVALUE)+MAXPARVALUE/2.f);
            break;
        case VL_FILTER_CUTOFF:
        case VL_COARSE_TUNE:
            value = (char)lrintf(fvalue);
            break;
        case VL_FINE_TUNE:
        {
            if (!synthesizer->GetPar(channel,par-1) && !synthesizer->GetPar(channel,par+1))
                value = (char)(sqrt(fvalue) * MAXPARVALUE);
            else
            {
                float ftemp = 0.f;
                if      (fvalue > 1.f)
                    ftemp = (logf(fvalue)/logf(2.0f)) *  12.0f;
                else if (fvalue < 1.f)
                    ftemp = (logf(fvalue)/logf(0.5f)) * -12.0f;
                else
                    ftemp = 0.f;
                value = (char)lrintf(((ftemp / 2.0f) + 0.5f) * MAXPARVALUE);
            }
            break;
        }
        case VL_TEMPO:
            value = (char)lrintf((MAXPARVALUE + 2.f) * powf(fvalue / MAXTIMEENV, 1.f/3.f) - 2.f);
            break;
        case VL_PORTAMENTO:
            value = (char)lrintf(MAXPARVALUE * powf(fvalue / MAXTIMEENV, 1.f/3.f));
            break;
        case VL_WAVEFORM:
            value = (char)lrintf(fvalue*(MAXPARVALUE/float(WAVEFORMS-1)));
            break;
        case VL_FILTER:
            value = (char)lrintf(fvalue*(MAXPARVALUE/2.f));
            break;
        case VL_MOD:
            value = (char)lrintf(MAXPARVALUE * powf(fvalue, 0.5f));
            break;
        case VL_PAN:
            value = (char)lrintf(((fvalue/2.f)*MAXPARVALUE)+MAXPARVALUE/2);
            break;
        case VL_PITCH_CURVE:
        {
            float ftemp = 0.f;
            if      (fvalue > 1.f)
                ftemp =  (logf(fvalue)/logf(2.0f)) / OCTAVES_PITCH_CURVE;
            else if (fvalue < 1.f)
                ftemp = -(logf(fvalue)/logf(0.5f)) / OCTAVES_PITCH_CURVE;
            else
                ftemp = 0.f;
            value = (char)lrintf(((ftemp / 2.0f) + 0.5f) * MAXPARVALUE);
            break;
        }
        case VL_LFO_RATE:
            value = (char)lrintf((MAXPARVALUE / 16.f) * fvalue);
            break;
        case VL_LFO_DEST:
            value = (char)lrintf(fvalue*(MAXPARVALUE/2.f));
            break;
        case VL_MOD_DEST:
            value = (char)lrintf(fvalue*(MAXPARVALUE/6.f));
            break;
        case VL_ON_OFF:
            value = fvalue ? MAXPARVALUE : 0;
    }
    return value;
}

float CMapper::IntValueToFloatValue(CSynthesizer *synthesizer, char channel, int par, int type, char value)
{
    float fvalue = 0.f;
    switch (type)
    {
        case VL_ZERO_TO_ONE:
            fvalue = (float)value/MAXPARVALUE;
            break;
        case VL_MINUS1_2_PLUS1:
            fvalue = (((float)value/MAXPARVALUE)-0.5f)*2.0f;
            break;
        case VL_FILTER_CUTOFF:
        case VL_COARSE_TUNE:
            fvalue = (float)value;
            break;
        case VL_FINE_TUNE:
            if (!synthesizer->GetPar(channel,par-1) && !synthesizer->GetPar(channel,par+1))
            {
                fvalue = ((float)value)/MAXPARVALUE;
                fvalue = fvalue*fvalue;
            }
            else
            {
                float ftemp = (((float)value - (MAXPARVALUE/2.0f)) * 2.0f) / MAXPARVALUE;
                if      (ftemp > 0)
                    fvalue = powf(2.0f,  ftemp / 12.0f);
                else if (ftemp < 0)
                    fvalue = powf(0.5f, -ftemp / 12.0f);
                else
                    fvalue = 1.0f;
            }
            break;
        case VL_TEMPO:
        {
            float ftemp = ((float)value + 2.0f)/(MAXPARVALUE + 2.0f);
            fvalue = MAXTIMEENV * ftemp*ftemp*ftemp;
            break;
        }
        case VL_PORTAMENTO:
        {
            float ftemp = (float)value/MAXPARVALUE;
            fvalue = MAXTIMEENV * ftemp*ftemp*ftemp;
            break;
        }
        case VL_WAVEFORM:
            fvalue = (float)(value/((lrintf(MAXPARVALUE)+4)/WAVEFORMS));
            break;
        case VL_FILTER:
            fvalue = (float)(value/((lrintf(MAXPARVALUE)+4)/(2+1)));
            break;
        case VL_MOD:
            fvalue = (float)value/MAXPARVALUE;
            fvalue = fvalue*fvalue;
            break;
        case VL_PAN:
            fvalue = float(lrintf((((float)value/MAXPARVALUE)-0.5f)*200.0f))/100.f;
            break;
        case VL_PITCH_CURVE:
        {
            char maxvalue;
            maxvalue = (char)lrintf(MAXPARVALUE);
            if      (value > (maxvalue/2))
                fvalue = powf(2.0f,OCTAVES_PITCH_CURVE * float((value   - (maxvalue>>1))<<1)/MAXPARVALUE);
            else if (value < (maxvalue/2))
                fvalue = powf(0.5f,OCTAVES_PITCH_CURVE * float(maxvalue - (value<<1))       /MAXPARVALUE);
            else
                fvalue = 1.f;
            break;
        }
        case VL_LFO_RATE:
            fvalue = (16.f / MAXPARVALUE) * (float)value;
            break;
        case VL_LFO_DEST:
            fvalue = (float)(value/((lrintf(MAXPARVALUE)+4)/(2+1)));
            break;
        case VL_MOD_DEST:
            fvalue = (float)(value/((lrintf(MAXPARVALUE)+5)/(6+1)));
            break;
        case VL_ON_OFF:
            fvalue = value > (MAXPARVALUE / 2) ? 1 : 0;
            break;
    }
    return fvalue;
}

void CMapper::GetDisplayValue(CSynthesizer *synthesizer, char channel, int par, int type, char* str)
{
    float fvalue = synthesizer->GetPar(channel, par);
    char  value  = CMapper::FloatValueToIntValue(synthesizer, channel, par, type, fvalue);
    switch (type)
    {
        case VL_ZERO_TO_ONE:
            StringCchPrintfA(str, TEXT_SIZE, "%.2f", fvalue);
            break;
        case VL_MINUS1_2_PLUS1:
            StringCchPrintfA(str, TEXT_SIZE, "%.2f", fvalue);
            break;
        case VL_FILTER_CUTOFF:
            StringCchPrintfA(str, TEXT_SIZE, "%.2fHz", C0 * powf(2.0f, ((float)value/MAXPARVALUE * MAXFREQOSC) / 12.0f));
            break;
        case VL_COARSE_TUNE:
        {
            float fineTun = synthesizer->GetPar(channel,par+1);
            float kbTrack = synthesizer->GetPar(channel,par+2);
            if (!kbTrack)
            {
                if (value)
                    StringCchPrintfA(str, TEXT_SIZE, "%.2fHz", C0 * fineTun * powf(2.0f, ((float)value/MAXPARVALUE * MAXFREQOSC) / 12.0f));
                else
                    StringCchPrintfA(str, TEXT_SIZE, "%.2fHz", C0 * fineTun);
            }
            else
            {
                if (value>50)
                {
                    char lval = value - 50;
                    char octa = lval / 12;
                    char semi = lval - (octa * 12);
                    StringCchPrintfA(str, TEXT_SIZE, "+%i oct & %02i semi", octa, semi);
                }
                else if (value<50)
                {
                    char lval = 50 - value;
                    char octa = lval / 12;
                    char semi = lval - (octa * 12);
                    StringCchPrintfA(str, TEXT_SIZE, "-%i oct & %02i semi", octa, semi);
                }
                else
                {
                    StringCchCopyA(str, TEXT_SIZE, "Center");
                }
            }
            break;
        }
        case VL_FINE_TUNE:
        {
            float kbTrack = synthesizer->GetPar(channel,par+1);
            float coarseT = synthesizer->GetPar(channel,par-1);
            if (!coarseT && !kbTrack)
            {
                StringCchPrintfA(str, TEXT_SIZE, "%.2fHz", C0 * fvalue);
            }
            else
            {
                if (!kbTrack)
                {
                    StringCchPrintfA(str, TEXT_SIZE, "%.2fHz", C0 * fvalue * powf(2.0f, (coarseT/MAXPARVALUE * MAXFREQOSC) / 12.0f));
                }
                else
                {
                    StringCchPrintfA(str, TEXT_SIZE, "%.2f semitone", (float)((value-(MAXPARVALUE/2))*2)/MAXPARVALUE);
                }
            }
            break;
        }
        case VL_TEMPO:
            StringCchPrintfA(str, TEXT_SIZE, "%fs", fvalue);
            break;
        case VL_PORTAMENTO:
            StringCchPrintfA(str, TEXT_SIZE, "%fs", fvalue);
            break;
        case VL_WAVEFORM:
            switch (lrintf(fvalue))
            {
                case 0:
                    StringCchCopyA(str, TEXT_SIZE, "Sine");
                    break;
                case 1:
                    StringCchCopyA(str, TEXT_SIZE, "Saw");
                    break;
                case 2:
                    StringCchCopyA(str, TEXT_SIZE, "Triangle");
                    break;
                case 3:
                    StringCchCopyA(str, TEXT_SIZE, "Square");
                    break;
                case 4:
                    StringCchCopyA(str, TEXT_SIZE, "Square bandlimit");
                    break;
                case 5:
                    StringCchCopyA(str, TEXT_SIZE, "Saw bandlimited");
                    break;
                default:
                    StringCchCopyA(str, TEXT_SIZE, "?");
                    break;
            }
            break;
        case VL_FILTER:
            if      (fvalue == 0.f)
                StringCchCopyA(str, TEXT_SIZE, "Lowpass");
            else if (fvalue == 1.f)
                StringCchCopyA(str, TEXT_SIZE, "Hipass");
            else if (fvalue == 2.f)
                StringCchCopyA(str, TEXT_SIZE, "Bandpass");
            else
                StringCchCopyA(str, TEXT_SIZE, "?");
            break;
        case VL_MOD:
            StringCchPrintfA(str, TEXT_SIZE, "%i", value);
            break;
        case VL_PAN:
            if      (fvalue < 0.f)
                StringCchPrintfA(str, TEXT_SIZE, "%i%% Left", lrintf(-fvalue*100.f));
            else if (fvalue > 0.f)
                StringCchPrintfA(str, TEXT_SIZE, "%i%% Right", (int)lrintf(fvalue*100.f));
            else
                StringCchCopyA(str, TEXT_SIZE, "Center");
            break;
        case VL_PITCH_CURVE:
        {
            char maxvalue = (char)lrintf(MAXPARVALUE);
            if      (value > (maxvalue/2))
                StringCchPrintfA(str, TEXT_SIZE, "down %i%%", (value - (maxvalue>>1))<<1);
            else if (value < (maxvalue/2))
                StringCchPrintfA(str, TEXT_SIZE, "up %i%%", maxvalue - (value<<1));
            else
                StringCchCopyA(str, TEXT_SIZE, "Curve Off");
            break;
        }
        case VL_LFO_RATE:
            StringCchPrintfA(str, TEXT_SIZE, "%.2fHz", fvalue);
            break;
        case VL_LFO_DEST:
            if      (fvalue == 0.f)
                StringCchCopyA(str, TEXT_SIZE, "Pitch");
            else if (fvalue == 1.f)
                StringCchCopyA(str, TEXT_SIZE, "OPX");
            else if (fvalue == 2.f)
                StringCchCopyA(str, TEXT_SIZE, "OPZ");
            else
                StringCchCopyA(str, TEXT_SIZE, "?");
            break;
        case VL_MOD_DEST:
            if      (fvalue == 0.f)
                StringCchCopyA(str, TEXT_SIZE, "LFO Depth");
            else if (fvalue == 1.f)
                StringCchCopyA(str, TEXT_SIZE, "LFO Rate");
            else if (fvalue == 2.f)
                StringCchCopyA(str, TEXT_SIZE, "Filter Cuttof");
            else if (fvalue == 3.f)
                StringCchCopyA(str, TEXT_SIZE, "LFO Depth Invert");
            else if (fvalue == 4.f)
                StringCchCopyA(str, TEXT_SIZE, "LFO Rate Invert");
            else if (fvalue == 5.f)
                StringCchCopyA(str, TEXT_SIZE, "Filter Cutt Invr");
            else if (fvalue == 6.f)
                StringCchCopyA(str, TEXT_SIZE, "None");
            else
                StringCchCopyA(str, TEXT_SIZE, "?");
            break;
        case VL_ON_OFF:
            StringCchCopyA(str, TEXT_SIZE, fvalue?"On":"Off");
            break;
        case VL_CHANNELS:
            StringCchPrintfA(str, TEXT_SIZE, "Program %03i", synthesizer->GetNumProgr(channel));
            break;
        case BT_MINUS_10:
        case BT_MINUS_1:
        case BT_PLUS_1:
        case BT_PLUS_10:
            if (synthesizer->GetBankMode())
            {
                synthesizer->GetBankName(str);
            }
            else
            {
                if (synthesizer->GetStandBy(channel))
                {
                    StringCchPrintfA(str, TEXT_SIZE, "conf in Prg%03i?", synthesizer->GetNumProgr(channel));
                }
                else
                {
                    synthesizer->GetProgName(str, channel);
                }
            }
            break;
        case BT_NAME:
            synthesizer->GetProgName(str, channel);
            break;
        case BT_STORE:
            if (synthesizer->GetStandBy(channel))
            {
                StringCchPrintfA(str, TEXT_SIZE, "conf in Prg%03i?", synthesizer->GetNumProgr(channel));
            }
            else
            {
                StringCchPrintfA(str, TEXT_SIZE, "was stored!");
            }
            break;
        case BT_BANK:
            synthesizer->GetBankName(str);
            break;
        case BT_PROGRAM:
            synthesizer->GetProgName(str, channel);
            break;
        default:
            StringCchPrintfA(str, TEXT_SIZE, "%i", value);
            break;
    }
}
