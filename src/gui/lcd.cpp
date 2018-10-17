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
#include "synthesizer.h"
#include "control.h"
#include "lcd.h"

#define LCD_SEP_H   1*GUI_SCALE  // horizontal space between the LCD characters
#define LCD_SEP_V   1*GUI_SCALE  // vertical space between the LCD characters
#define LCD_CHAR_H  7*GUI_SCALE  // char height in pixels
#define LCD_CHAR_W  5*GUI_SCALE  // char width in pixels
#define LCD_X       0  // bg left
#define LCD_Y       0  // bg top

CLcd::CLcd(int bmp, int x, int y)
{
    int i;
    for (i=0;i<LCD_COLS;i++)
        text0[i] = ' ';
    for (i=0;i<LCD_COLS;i++)
        text1[i] = ' ';
    this->lcdx     = x;
    this->lcdy     = y;
    this->bmp      = bmp;
    this->left     = x + LCD_X;
    this->top      = y + LCD_Y;
    this->right    = this->left + (LCD_CHAR_W * LCD_COLS ) + (LCD_SEP_H * LCD_COLS );
    this->bottom   = this->top  + (LCD_CHAR_H * LCD_LINES) + (LCD_SEP_V * LCD_LINES);
    this->toolkit  = NULL;
}

int CLcd::GetCoordinates (oxeCoords *coords)
{
    int i;
    for (i=0;i<LCD_COLS;i++)
    {
        coords->destX   = lcdx + LCD_X + LCD_SEP_H + ((LCD_SEP_H + LCD_CHAR_W) * i);
        coords->destY   = lcdy + LCD_Y + LCD_SEP_V;
        coords->width   = LCD_CHAR_W;
        coords->height  = LCD_CHAR_H;
        coords->origBmp = this->bmp;
        coords->origX   = (0xF & (text0[i] - ' ')) * LCD_CHAR_W;
        coords->origY   = ((0xF0 & (text0[i] - ' ')) / 0x10) * LCD_CHAR_H;
        coords++;
    }
    for (i=0;i<LCD_COLS;i++)
    {
        coords->destX   = lcdx + LCD_X + LCD_SEP_H + ((LCD_SEP_H + LCD_CHAR_W) * i);
        coords->destY   = lcdy + LCD_Y + LCD_SEP_V + LCD_CHAR_H + LCD_SEP_V;
        coords->width   = LCD_CHAR_W;
        coords->height  = LCD_CHAR_H;
        coords->origBmp = this->bmp;
        coords->origX   = (0xF & (text1[i] - ' ')) * LCD_CHAR_W;
        coords->origY   = ((0xF0 & (text1[i] - ' ')) / 0x10) * LCD_CHAR_H;
        coords++;
	}
    return LCD_COORDS;
}

void CLcd::Repaint()
{
    if (!toolkit)
    {
        return;
    }
    oxeCoords coords[LCD_COLS * LCD_LINES];
    oxeCoords *c = coords;
    int count = GetCoordinates(c);
    while (count--)
    {
        toolkit->CopyRect(c->destX, c->destY, c->width, c->height, c->origBmp, c->origX, c->origY);
        c++;
    }
}

bool CLcd::SetText(char lineIndex, const char* text)
{
    int i = 0;
    int tam = strlen(text);
    if (lineIndex == 0)
    {
        if (tam >= LCD_COLS)
        {
            memcpy(text0,text,LCD_COLS);
        }
        else if (tam < LCD_COLS)
        {
            memcpy(text0,text,tam);
            for (i=tam;i<LCD_COLS;i++)
                text0[i] = ' ';
        }
        Repaint();
    }
    else if (lineIndex == 1)
    {
        if (tam >= LCD_COLS)
        {
            memcpy(text1,text,LCD_COLS);
        }
        else if (tam < LCD_COLS)
        {
            memcpy(text1,text,tam);
            for (i=tam;i<LCD_COLS;i++)
                text1[i] = ' ';
        }
        Repaint();
    }
    else
        return false;
    return true;
}
