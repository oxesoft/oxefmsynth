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
#include "lcd.h"

CLcd::CLcd(HBITMAP bmpchars, int x, int y)
{
    this->hwnd        = NULL;
    this->lcdx        = x;
    this->lcdy        = y;
    this->bmpchars    = bmpchars;
    int i;
    for (i=0;i<LCD_COLS;i++)
        text0[i] = ' ';
    for (i=0;i<LCD_COLS;i++)
        text1[i] = ' ';
    this->rect.left   = x + LCD_X;
    this->rect.top    = y + LCD_Y;
    this->rect.right  = rect.left + (LCD_LARDIG * LCD_COLS) + (LCD_EECH * LCD_COLS);
    this->rect.bottom = rect.top  + (LCD_ALTDIG * LCD_LINS) + (LCD_EECV * LCD_LINS);
}

void CLcd::SetHandlers(HWND hwnd, HDC dc, HDC memdc)
{
    this->hwnd  = hwnd;
    this->dc    = dc;
    this->memdc = memdc;
    if (this->hwnd && this->dc && this->memdc)
    {
        Repaint();
    }
}

void CLcd::Repaint()
{
    int i;
    int dcant = SaveDC(memdc);
    SelectObject(memdc,bmpchars);
    for (i=0;i<LCD_COLS;i++)
        BitBlt(dc, lcdx + LCD_X + LCD_EECH + ((LCD_EECH + LCD_LARDIG) * i), lcdy + LCD_Y + LCD_EECV,                         LCD_LARDIG, LCD_ALTDIG, memdc, (0xF & (text0[i] - ' ')) * LCD_LARDIG, ((0xF0 & (text0[i] - ' ')) / 0x10) * LCD_ALTDIG, SRCCOPY);
    for (i=0;i<LCD_COLS;i++)
        BitBlt(dc, lcdx + LCD_X + LCD_EECH + ((LCD_EECH + LCD_LARDIG) * i), lcdy + LCD_Y + LCD_EECV + LCD_ALTDIG + LCD_EECV, LCD_LARDIG, LCD_ALTDIG, memdc, (0xF & (text1[i] - ' ')) * LCD_LARDIG, ((0xF0 & (text1[i] - ' ')) / 0x10) * LCD_ALTDIG, SRCCOPY);
    RestoreDC(memdc,dcant);
}

bool CLcd::SetText(char linha, const char* text)
{
    int i = 0;
    int tam = strlen(text);
    if (linha == 0)
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
        if (hwnd)
        {
            Repaint();
            InvalidateRect(hwnd, &rect, FALSE);
        }
    }
    else if (linha == 1)
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
        if (hwnd)
        {
            Repaint();
            InvalidateRect(hwnd, &rect, FALSE);
        }
    }
    else
        return FALSE;
    return TRUE;
}
