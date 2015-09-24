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
#include "synthesizer.h"
#include "control.h"
#include "key.h"

CKey::CKey(int bmp, int index, int w, int h, const char *name, CSynthesizer *synthesizer, char &channel, int par, int x, int y)
{
    strncpy(this->name, name, TEXT_SIZE);
    this->toolkit     = NULL;
    this->bmp         = bmp;
    this->index       = index;
    this->w           = w;
    this->h           = h;
    this->channel     = &channel;
    this->synthesizer = synthesizer;
    this->par         = par;
    this->left        = x;
    this->top         = y;
    this->right       = x + w;
    this->bottom      = y + h;
    this->value       = 0;
}

void CKey::OnClick(int x, int y)
{
    this->value = !this->value;
    synthesizer->SetPar(*channel, par, this->value);
    Repaint();
}

void CKey::Repaint()
{
    if (toolkit)
    {
        toolkit->CopyRect(this->left, this->top, this->right - this->left, this->bottom - this->top, this->bmp, value?this->w:0, this->h * this->index);
    }
}

bool CKey::Update(void)
{
    if (this->value != (char)synthesizer->GetPar(*channel,par))
    {
        this->value = !this->value;
        Repaint();
    }
    return true;
}

bool CKey::GetName(char* str)
{
    strncpy(str, name, TEXT_SIZE);
    return true;
}

int CKey::GetIndex()
{
    return this->par;
}

int CKey::GetType()
{
    return VL_ON_OFF;
}
