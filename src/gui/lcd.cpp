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

#include <string.h>
#include <stdio.h>
#include "synthesizer.h"
#include "control.h"
#include "lcd.h"

CLcd::CLcd(int bmp, int x, int y)
{
    this->bmp     = bmp;
    this->left    = x;
    this->top     = y;
    this->right   = x + 165;
    this->bottom  = y + 48;
    this->toolkit = NULL;
    ShowWelcome();
}

void CLcd::ShowWelcome()
{
    char version_info[LCD_COLS + 1];
    char vstr[32];
    snprintf(vstr, sizeof(vstr), "v%s", VERSION_STR);
    int vlen = (int)strlen(vstr);
    int pad = (LCD_COLS - vlen) / 2;
    if (pad < 0) pad = 0;
    int pos = 0;
    for (int p = 0; p < pad && pos < LCD_COLS; p++) version_info[pos++] = ' ';
    for (int i = 0; i < vlen && pos < LCD_COLS; i++) version_info[pos++] = vstr[i];
    while (pos < LCD_COLS) version_info[pos++] = ' ';
    version_info[LCD_COLS] = 0;
    SetText(0, "  Oxe FM Synth  ");
    SetText(1, version_info);
}

void CLcd::GetRepaintBounds(int &rx, int &ry, int &rw, int &rh) const
{
    rx = this->left - 2;
    ry = this->top - 2;
    rw = (this->right - this->left) + 4;
    rh = (this->bottom - this->top) + 4;
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

    char str0[TEXT_SIZE + 1];
    char str1[TEXT_SIZE + 1];
    memcpy(str0, text[0], TEXT_SIZE); str0[TEXT_SIZE] = 0;
    memcpy(str1, text[1], TEXT_SIZE); str1[TEXT_SIZE] = 0;

    // Line 0 (top line: bank or parameter label)
    if (fontSmall.is_valid())
    {
        ctx.fill_utf8_text(BLPoint(left + 8, top + 18), fontSmall, str0, SIZE_MAX, BLRgba32(0x6e, 0x9e, 0xb8));
    }

    // Line 1 (bottom line: preset or parameter value)
    if (fontNormal.is_valid())
    {
        ctx.fill_utf8_text(BLPoint(left + 8, top + 36), fontNormal, str1, SIZE_MAX, BLRgba32(0x00, 0xf0, 0xff));
    }
}

bool CLcd::SetText(char lineIndex, const char* str)
{
    if (lineIndex < 0 || lineIndex >= LCD_LINES)
        return false;
    char *dst = text[(int)lineIndex];
    int tam = (int)strlen(str);
    if (tam >= TEXT_SIZE)
    {
        memcpy(dst, str, TEXT_SIZE);
    }
    else
    {
        memcpy(dst, str, tam);
        memset(dst + tam, ' ', TEXT_SIZE - tam);
    }
    Repaint();
    return true;
}
