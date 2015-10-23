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

int CKey::GetCoordinates (oxeCoords *coords)
{
    coords->destX   = this->left;
    coords->destY   = this->top;
    coords->width   = this->right - this->left;
    coords->height  = this->bottom - this->top;
    coords->origBmp = this->bmp;
    coords->origX   = value ? this->w : 0;
    coords->origY   = this->h * this->index;
    return 1;
}

void CKey::Repaint()
{
    if (!toolkit)
    {
        return;
    }
    oxeCoords coords;
    GetCoordinates(&coords);
    toolkit->CopyRect(coords.destX, coords.destY, coords.width, coords.height, coords.origBmp, coords.origX, coords.origY);
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
