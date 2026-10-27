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
#include "key.h"

static const char* GetKeyShortLabel(int bmp, const char* name)
{
    if (bmp == BMP_OPS) return "";
    if (strstr(name, "Bypass")) return "BP";
    if (strstr(name, "Keyb Track")) return "KB";
    if (strstr(name, "On/Off")) return "On";
    return "";
}

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
    this->shortLabel  = GetKeyShortLabel(this->bmp, this->name);
}

void CKey::GetRepaintBounds(int &rx, int &ry, int &rw, int &rh) const
{
    if (shortLabel[0])
    {
        rx = this->left - 6;
        ry = this->top - 2;
        rw = this->w + 12;
        rh = 42;
    }
    else
    {
        rx = this->left - 2;
        ry = this->top - 2;
        rw = this->w + 4;
        rh = this->h + 4;
    }
}

void CKey::OnClick(int x, int y)
{
    this->value = !this->value;
    synthesizer->SetPar(*channel, par, this->value);
    Repaint();
}

void CKey::Paint(BLContext &ctx, const BLFont &fontSmall, const BLFont &fontNormal)
{
    float w = right - left;
    float h = bottom - top;
    float cx = left + w * 0.5f;
    float cy = top + h * 0.5f;
    float r = (w < h ? w : h) * 0.44f;

    // Check if it's one of the matrix diagonal operator toggles
    bool isMatrixOp = (bmp == BMP_OPS);

    if (isMatrixOp)
    {
        // Matrix diagonal operator node (labeled A, B, C, D, E, F, X, Z)
        char letter[2] = { GetOpLetter(index), 0 };
        BLRgba32 accentColor = GetOpAccentColor(index);

        // Circular background
        ctx.fill_circle(cx, cy, r, BLRgba32(0x18, 0x20, 0x2b));
        ctx.set_stroke_width(1.8);

        BLGlyphBuffer gb;
        gb.set_utf8_text(letter, 1);
        BLTextMetrics tm;
        if (fontNormal.is_valid())
        {
            fontNormal.get_text_metrics(gb, tm);
        }
        float lx = cx - (float)(tm.bounding_box.x0 + tm.bounding_box.x1) * 0.5f;

        BLRgba32 color = value ? accentColor : BLRgba32(0x55, 0x64, 0x76);
        BLRgba32 ringColor = value ? accentColor : BLRgba32(0x30, 0x3c, 0x4c);
        ctx.stroke_circle(cx, cy, r, ringColor);
        if (fontNormal.is_valid())
        {
            ctx.fill_utf8_text(BLPoint(lx, cy + 4.5f), fontNormal, letter, 1, color);
        }
    }
    else
    {
        // Standard LED or toggle button
        if (value)
        {
            // Illuminated active LED
            ctx.fill_circle(cx, cy, r + 2.0f, BLRgba32(0x00, 0xf0, 0xff, 0x38)); // soft outer glow
            ctx.fill_circle(cx, cy, r, BLRgba32(0x00, 0xf0, 0xff)); // bright cyan center
            ctx.fill_circle(cx - r * 0.25f, cy - r * 0.25f, r * 0.35f, BLRgba32(0xff, 0xff, 0xff, 0xd0)); // highlight
        }
        else
        {
            // Dark inactive LED
            ctx.fill_circle(cx, cy, r, BLRgba32(0x18, 0x1e, 0x27));
            ctx.set_stroke_width(1.2);
            ctx.stroke_circle(cx, cy, r, BLRgba32(0x32, 0x3e, 0x4e));
            ctx.fill_circle(cx, cy, r * 0.35f, BLRgba32(0x24, 0x2c, 0x38));
        }

        // Short label below LED
        if (shortLabel[0] && fontSmall.is_valid())
        {
            float tw = strlen(shortLabel) * 5.5f;
            float tx = cx - tw * 0.5f;
            float ty = top + 35.0f;
            ctx.fill_utf8_text(BLPoint(tx, ty), fontSmall, shortLabel, SIZE_MAX, BLRgba32(0x7a, 0x8a, 0x9e));
        }
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
