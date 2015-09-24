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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "synthesizer.h"
#include "control.h"
#include "knob.h"
#include "mapper.h"

CKnob::CKnob(int bmp, int knobSize, const char *name, CSynthesizer *synthesizer, char &channel, int type, int par, int x, int y)
{
    strncpy(this->name, name, TEXT_SIZE);
    this->toolkit     = NULL;
    this->bmp         = bmp;
    this->knobSize    = knobSize;
    this->channel     = &channel;
    this->synthesizer = synthesizer;
    this->par         = par;
    this->type        = type;
    this->left        = x;
    this->top         = y;
    this->right       = x + knobSize;
    this->bottom      = y + knobSize;
    this->value       = 0;
    this->fvalue      = 999.f;
}

void CKnob::Repaint()
{
    char valtemp = value;
    switch (type)
    {
        case VL_WAVEFORM:
            valtemp = (char)lrintf(fvalue*(MAXPARVALUE/(WAVEFORMS-1)));
            break;
        case VL_FILTER:
            valtemp = (char)lrintf(fvalue*(MAXPARVALUE/2));
            break;
        case VL_LFO_DEST:
            valtemp = (char)lrintf(fvalue*(MAXPARVALUE/2));
            break;
        case VL_MOD_DEST:
            valtemp = (char)lrintf(fvalue*(MAXPARVALUE/6));
            break;
    }
    if (valtemp > 99)
        valtemp = 99;
    if (toolkit)
    {
        toolkit->CopyRect(this->left, this->top, this->right - this->left, this->bottom - this->top, this->bmp, (valtemp - (abs(valtemp/10) * 10)) * this->knobSize, abs(valtemp/10) * this->knobSize);
    }
}

bool CKnob::Update(void)
{
    float newValue = synthesizer->GetPar(*channel, par);
    if (newValue != this->fvalue)
    {
        this->fvalue = newValue;
        this->value = CMapper::FloatValueToIntValue(this->synthesizer, *channel, this->par, this->type, this->fvalue);
        Repaint();
    }
    return true;
}

bool CKnob::IncreaseValue(int delta)
{
    this->value += delta;
    if (this->value > (char)MAXPARVALUE)
    {
        this->value = (char)MAXPARVALUE;
    }
    if (this->value < 0)
    {
        this->value = 0;
    }
    this->fvalue = CMapper::IntValueToFloatValue(this->synthesizer, *channel, this->par, this->type, this->value);
    synthesizer->SetPar(*channel, this->par, this->fvalue);
    Repaint();
    return true;
}

bool CKnob::GetName(char* str)
{
    strncpy(str, name, TEXT_SIZE);
    return true;
}

int CKnob::GetIndex()
{
    return this->par;
}

int CKnob::GetType()
{
    return this->type;
}
