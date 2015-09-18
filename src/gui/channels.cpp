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
#include "toolkit.h"
#include "control.h"
#include "synthesizer.h"
#include "channels.h"

#define KEY_WIDTH   10
#define KEY_HEIGHT  10

CChannels::CChannels(int bmp, CSynthesizer *synthesizer, char &channel, int x, int y)
{
    this->toolkit     = NULL;
    this->bmp         = bmp;
    this->channel     = &channel;
    this->synthesizer = synthesizer;
    this->left        = x;
    this->top         = y;
    this->right       = x + KEY_WIDTH  * 8;
    this->bottom      = y + KEY_HEIGHT * 2;
}

void CChannels::SetToolkit(CToolkit *toolkit)
{
    this->toolkit = toolkit;
    if (toolkit)
    {
        Repaint();
    }
}

void CChannels::OnClick(int x, int y)
{
    *channel = (char)((x - this->left) / KEY_WIDTH);
    if (y > this->top + KEY_HEIGHT)
    {
        *channel += 8;
    }
    Repaint();
}

void CChannels::Repaint()
{
    char i;
    if (!toolkit)
    {
        return;
    }
    for (i=0;i< 8;i++)
        toolkit->CopyRect(this->left +  i    * KEY_WIDTH, this->top             , KEY_WIDTH, KEY_HEIGHT, this->bmp, (*channel==i)?KEY_WIDTH:0, 0);
    for (i=8;i<16;i++)
        toolkit->CopyRect(this->left + (i-8) * KEY_WIDTH, this->top + KEY_HEIGHT, KEY_WIDTH, KEY_HEIGHT, this->bmp, (*channel==i)?KEY_WIDTH:0, 0);
}

int CChannels::GetType()
{
    return VL_CHANNELS;
}

bool CChannels::GetName(char* str)
{
    snprintf(str, TEXT_SIZE, "Channel %02i", *channel + 1);
    return true;
}
