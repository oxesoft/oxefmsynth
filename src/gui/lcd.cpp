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
#include "lcd.h"

#define LCD_SEP_H   1  // horizontal space between the LCD characters
#define LCD_SEP_V   1  // vertical space between the LCD characters
#define LCD_CHAR_H  7  // char height in pixels
#define LCD_CHAR_W  5  // char width in pixels
#define LCD_X       0  // bg left
#define LCD_Y       0  // bg top

CLcd::CLcd(int bmp, int x, int y)
{
    memset(text0, ' ', LCD_COLS);
    memset(text1, ' ', LCD_COLS);
    const char* def0 = "  Oxe FM Synth  ";
    char def1[32];
    char vstr[32];
    snprintf(vstr, sizeof(vstr), "v%s", VERSION_STR);
    int vlen = (int)strlen(vstr);
    int pad = (LCD_COLS - vlen) / 2;
    if (pad < 0) pad = 0;
    int pos = 0;
    for (int p = 0; p < pad && pos < LCD_COLS; p++) def1[pos++] = ' ';
    for (int i = 0; i < vlen && pos < LCD_COLS; i++) def1[pos++] = vstr[i];
    while (pos < LCD_COLS) def1[pos++] = ' ';
    def1[LCD_COLS] = 0;
    memcpy(text0, def0, strlen(def0) > LCD_COLS ? LCD_COLS : strlen(def0));
    memcpy(text1, def1, LCD_COLS);
    this->lcdx     = x;
    this->lcdy     = y;
    this->bmp      = bmp;
    this->left     = x;
    this->top      = y;
    this->right    = x + 165;
    this->bottom   = y + 42;
    this->toolkit  = NULL;
}

int CLcd::GetCoordinates (oxeCoords *coords)
{
    return LCD_COORDS;
}

void CLcd::GetRepaintBounds(int &rx, int &ry, int &rw, int &rh) const
{
    rx = this->left - 2;
    ry = this->top - 2;
    rw = (this->right - this->left) + 4;
    rh = (this->bottom - this->top) + 4;
}

void CLcd::Repaint()
{
    if (toolkit)
    {
        int rx, ry, rw, rh;
        GetRepaintBounds(rx, ry, rw, rh);
        toolkit->InvalidateRect(rx, ry, rw, rh);
    }
}

void CLcd::Paint(BLContext &ctx, const BLFont &fontSmall, const BLFont &fontNormal)
{
    float w = right - left;
    float h = bottom - top;
    BLRoundRect rr(left, top, w, h, 4.0, 4.0);

    // Recessed dark display screen
    ctx.fill_round_rect(rr, BLRgba32(0x0c, 0x10, 0x16));
    ctx.set_stroke_width(1.0);
    ctx.stroke_round_rect(rr, BLRgba32(0x25, 0x30, 0x40));

    char str0[LCD_COLS + 1];
    char str1[LCD_COLS + 1];
    memcpy(str0, text0, LCD_COLS); str0[LCD_COLS] = 0;
    memcpy(str1, text1, LCD_COLS); str1[LCD_COLS] = 0;

    // Line 0 (top line: bank or parameter label)
    if (fontSmall.is_valid())
    {
        ctx.fill_utf8_text(BLPoint(left + 8, top + 16), fontSmall, str0, SIZE_MAX, BLRgba32(0x6e, 0x9e, 0xb8));
    }

    // Line 1 (bottom line: preset or parameter value)
    if (fontNormal.is_valid())
    {
        ctx.fill_utf8_text(BLPoint(left + 8, top + 33), fontNormal, str1, SIZE_MAX, BLRgba32(0x00, 0xf0, 0xff));
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
