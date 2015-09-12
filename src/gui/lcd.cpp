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
#include "control.h"
#include "lcd.h"

#define LCD_LINES   2  // lines
#define LCD_SEP_H   1  // horizontal space between the LCD characters
#define LCD_SEP_V   1  // vertical space between the LCD characters
#define LCD_CHAR_H  7  // char height in pixels
#define LCD_CHAR_W  5  // char width in pixels
#define LCD_X       0  // bg left
#define LCD_Y       0  // bg top

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
    this->rect.right  = rect.left + (LCD_CHAR_W * LCD_COLS ) + (LCD_SEP_H * LCD_COLS );
    this->rect.bottom = rect.top  + (LCD_CHAR_H * LCD_LINES) + (LCD_SEP_V * LCD_LINES);
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
        BitBlt(dc, lcdx + LCD_X + LCD_SEP_H + ((LCD_SEP_H + LCD_CHAR_W) * i), lcdy + LCD_Y + LCD_SEP_V,                         LCD_CHAR_W, LCD_CHAR_H, memdc, (0xF & (text0[i] - ' ')) * LCD_CHAR_W, ((0xF0 & (text0[i] - ' ')) / 0x10) * LCD_CHAR_H, SRCCOPY);
    for (i=0;i<LCD_COLS;i++)
        BitBlt(dc, lcdx + LCD_X + LCD_SEP_H + ((LCD_SEP_H + LCD_CHAR_W) * i), lcdy + LCD_Y + LCD_SEP_V + LCD_CHAR_H + LCD_SEP_V, LCD_CHAR_W, LCD_CHAR_H, memdc, (0xF & (text1[i] - ' ')) * LCD_CHAR_W, ((0xF0 & (text1[i] - ' ')) / 0x10) * LCD_CHAR_H, SRCCOPY);
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
