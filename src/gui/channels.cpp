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
#include "channels.h"

#define KEY_WIDTH   15
#define KEY_HEIGHT  15

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

void CChannels::OnClick(int x, int y)
{
    *channel = (char)((x - this->left) / KEY_WIDTH);
    if (y > this->top + KEY_HEIGHT)
    {
        *channel += 8;
    }
    Repaint();
}

int CChannels::GetCoordinates (oxeCoords *coords)
{
    char i;
    for (i=0;i< 8;i++)
    {
        coords->destX   = this->left +  i    * KEY_WIDTH;
        coords->destY   = this->top;
        coords->width   = KEY_WIDTH;
        coords->height  = KEY_HEIGHT;
        coords->origBmp = this->bmp;
        coords->origX   = (*channel==i)?KEY_WIDTH:0;
        coords->origY   = 0;
        coords++;
    }
    for (i=8;i<16;i++)
    {
        coords->destX   = this->left + (i-8) * KEY_WIDTH;
        coords->destY   = this->top + KEY_HEIGHT;
        coords->width   = KEY_WIDTH;
        coords->height  = KEY_HEIGHT;
        coords->origBmp = this->bmp;
        coords->origX   = (*channel==i)?KEY_WIDTH:0;
        coords->origY   = 0;
        coords++;
    }
    return MIDICHANNELS;
}

void CChannels::Repaint()
{
    if (toolkit)
    {
        toolkit->InvalidateRect(this->left, this->top, this->right - this->left, this->bottom - this->top);
    }
}

void CChannels::Paint(BLContext &ctx, const BLFont &fontSmall, const BLFont &fontNormal)
{
    float cellW = (float)(right - left) / 8.0f;
    float cellH = (float)(bottom - top) / 2.0f;
    float r = (cellW < cellH ? cellW : cellH) * 0.36f;

    for (int i = 0; i < 16; i++)
    {
        int col = (i < 8) ? i : (i - 8);
        int row = (i < 8) ? 0 : 1;
        float cx = left + (col + 0.5f) * cellW;
        float cy = top  + (row + 0.5f) * cellH;

        if (*channel == i)
        {
            // Active selected channel
            ctx.fill_circle(cx, cy, r + 2.0f, BLRgba32(0x00, 0xf0, 0xff, 0x40));
            ctx.fill_circle(cx, cy, r, BLRgba32(0x00, 0xf0, 0xff));
            ctx.fill_circle(cx - r * 0.25f, cy - r * 0.25f, r * 0.3f, BLRgba32(0xff, 0xff, 0xff, 0xd0));
        }
        else
        {
            // Inactive channel LED
            ctx.fill_circle(cx, cy, r, BLRgba32(0x19, 0x20, 0x2b));
            ctx.set_stroke_width(1.0);
            ctx.stroke_circle(cx, cy, r, BLRgba32(0x2f, 0x3b, 0x4c));
            ctx.fill_circle(cx, cy, r * 0.35f, BLRgba32(0x24, 0x2c, 0x38));
        }
    }
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
