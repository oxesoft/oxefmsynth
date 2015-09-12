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
#include <strsafe.h>
#include "control.h"
#include "synthesizer.h"
#include "key.h"

CKey::CKey(HBITMAP bmp, int index, int chave_h, int chave_v, const char *name, CSynthesizer *synthesizer, char &channel, int par, int x, int y)
{
    StringCchCopyA(this->name, TEXT_SIZE, name);
    this->hwnd        = NULL;
    this->bmp         = bmp;
    this->index       = index;
    this->chave_h     = chave_h;
    this->chave_v     = chave_v;
    this->channel     = &channel;
    this->synthesizer = synthesizer;
    this->par         = par;
    this->rect.left   = x;
    this->rect.top    = y;
    this->rect.right  = x + chave_h;
    this->rect.bottom = y + chave_v;
    this->value       = 0;
}

void CKey::SetHandlers(HWND hwnd, HDC dc, HDC memdc)
{
    this->hwnd  = hwnd;
    this->dc    = dc;
    this->memdc = memdc;
    if (this->hwnd && this->dc && this->memdc)
    {
        Repaint();
    }
}

void CKey::OnClick(POINT point)
{
    this->value = !this->value;
    synthesizer->SetPar(*channel, par, this->value);
    if (hwnd)
    {
        Repaint();
        InvalidateRect(hwnd, &rect, FALSE);
    }
}

void CKey::Repaint()
{
    int dcant = SaveDC(memdc);
    SelectObject(memdc,bmp);
    BitBlt(dc, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, memdc, value?chave_h:0, chave_v * index, SRCCOPY);
    RestoreDC(memdc,dcant);
}

bool CKey::Update(void)
{
    if (this->value != (char)synthesizer->GetPar(*channel,par))
    {
        this->value = !this->value;
        if (hwnd)
        {
            Repaint();
            InvalidateRect(hwnd, &rect, FALSE);
        }
    }
    return TRUE;
}

bool CKey::GetName(char* str)
{
    StringCchCopyA(str, TEXT_SIZE, name);
    return TRUE;
}

int CKey::GetIndex()
{
    return this->par;
}

int CKey::GetType()
{
    return VL_ON_OFF;
}
