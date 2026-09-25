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
#include "button.h"

#define BUTTON_WIDTH   63
#define BUTTON_HEIGHT  24

CButton::CButton(int bmp, int index, CSynthesizer *synthesizer, char &channel, int type, int x, int y)
{
    this->toolkit     = NULL;
    this->index       = index;
    this->bmp         = bmp;
    this->synthesizer = synthesizer;
    this->channel     = &channel;
    this->type        = type;
    this->left        = x;
    this->top         = y;
    this->right       = x + BUTTON_WIDTH;
    this->bottom      = y + BUTTON_HEIGHT;
}

void CButton::OnClick(int x, int y)
{
    // fires the assigned action
    switch (type)
    {
        case BT_MINUS_10:
            if (synthesizer->GetBankMode())
            {
                int ultbank = synthesizer->GetBankCount() - 1;
                int numbank = synthesizer->GetBankIndex();
                numbank -= 10;
                if (numbank < 0)
                    numbank = 0;
                synthesizer->SetBankIndex(numbank);
            }
            else
            {
                unsigned char numprog;
                numprog = synthesizer->GetNumProgr(*channel);
                numprog -= 10;
                if (numprog > 127)
                    numprog = 127;
                synthesizer->SendEvent(0xC0 + *channel, numprog, 0, 0);
            }
            break;
        case BT_MINUS_1:
            if (synthesizer->GetBankMode())
            {
                int ultbank = synthesizer->GetBankCount() - 1;
                int numbank = synthesizer->GetBankIndex();
                numbank = synthesizer->GetBankIndex();
                numbank -= 1;
                if (numbank > ultbank)
                    numbank = ultbank;
                if (numbank < 0)
                    numbank = ultbank;
                synthesizer->SetBankIndex(numbank);
            }
            else
            {
                unsigned char numprog;
                numprog = synthesizer->GetNumProgr(*channel);
                numprog -= 1;
                if (numprog > 127)
                    numprog = 127;
                synthesizer->SendEvent(0xC0 + *channel, numprog, 0, 0);
            }
            break;
        case BT_PLUS_1:
            if (synthesizer->GetBankMode())
            {
                int ultbank = synthesizer->GetBankCount() - 1;
                int numbank = synthesizer->GetBankIndex();
                numbank = synthesizer->GetBankIndex();
                numbank += 1;
                if (numbank > ultbank)
                    numbank = 0;
                synthesizer->SetBankIndex(numbank);
            }
            else
            {
                unsigned char numprog;
                numprog = synthesizer->GetNumProgr(*channel);
                numprog += 1;
                if (numprog > 127)
                    numprog = 0;
                synthesizer->SendEvent(0xC0 + *channel, numprog, 0, 0);
            }
            break;
        case BT_PLUS_10:
            if (synthesizer->GetBankMode())
            {
                int ultbank = synthesizer->GetBankCount() - 1;
                int numbank = synthesizer->GetBankIndex();
                numbank = synthesizer->GetBankIndex();
                numbank += 10;
                if (numbank > ultbank)
                    numbank = ultbank;
                synthesizer->SetBankIndex(numbank);
            }
            else
            {
                unsigned char numprog;
                numprog = synthesizer->GetNumProgr(*channel);
                numprog += 10;
                if (numprog > 127)
                    numprog = 0;
                synthesizer->SendEvent(0xC0 + *channel, numprog, 0, 0);
            }
            break;
        case BT_STORE:
            synthesizer->SetBankMode(false);
            synthesizer->StoreProgram(*channel);
            break;
        case BT_BANK:
            synthesizer->SetBankMode(true);
            break;
        case BT_PROGRAM:
            synthesizer->SetBankMode(false);
            synthesizer->SetStandBy(*channel, false);
            break;
    }
}

int CButton::GetCoordinates (oxeCoords *coords)
{
    coords->destX   = this->left;
    coords->destY   = this->top;
    coords->width   = this->right - this->left;
    coords->height  = this->bottom - this->top;
    coords->origBmp = this->bmp;
    coords->origX   = 0;
    coords->origY   = index * BUTTON_HEIGHT;
    return 1;
}

void CButton::Repaint()
{
    if (toolkit)
    {
        toolkit->InvalidateRect(this->left, this->top, this->right - this->left, this->bottom - this->top);
    }
}

void CButton::Paint(BLContext &ctx, const BLFont &fontSmall, const BLFont &fontNormal)
{
    float w = right - left;
    float h = bottom - top;
    BLRoundRect rr(left, top, w, h, 3.5, 3.5);

    // Button background
    ctx.fill_round_rect(rr, BLRgba32(0x1e, 0x25, 0x32));
    ctx.set_stroke_width(1.0);
    ctx.stroke_round_rect(rr, BLRgba32(0x32, 0x3d, 0x4f));

    const char* label = "";
    switch (type)
    {
        case BT_BANK:     label = "Bank"; break;
        case BT_PROGRAM:  label = "Program"; break;
        case BT_MINUS_10: label = "-10"; break;
        case BT_MINUS_1:  label = "-1"; break;
        case BT_PLUS_1:   label = "+1"; break;
        case BT_PLUS_10:  label = "+10"; break;
        case BT_NAME:     label = "Name"; break;
        case BT_STORE:    label = "Store"; break;
    }

    // Small status LED for Bank and Program
    if (type == BT_BANK || type == BT_PROGRAM)
    {
        float ledX = left + 7.5f;
        float ledY = top + h * 0.5f;
        bool isActive = (type == BT_BANK) ? synthesizer->GetBankMode() : !synthesizer->GetBankMode();
        if (isActive)
        {
            ctx.fill_circle(ledX, ledY, 2.8, BLRgba32(0x00, 0xf0, 0xff));
        }
        else
        {
            ctx.fill_circle(ledX, ledY, 2.0, BLRgba32(0x2d, 0x38, 0x46));
        }
    }

    // Centered label text
    if (label[0] && fontSmall.is_valid())
    {
        float tw = strlen(label) * 5.8f;
        float tx = left + (w - tw) * 0.5f;
        if (type == BT_BANK || type == BT_PROGRAM)
        {
            tx += 4.0f;
        }
        float ty = top + h * 0.5f + 3.5f;
        ctx.fill_utf8_text(BLPoint(tx, ty), fontSmall, label, SIZE_MAX, BLRgba32(0xc8, 0xd4, 0xe4));
    }
}

bool CButton::GetName(char* str)
{
    if (synthesizer->GetBankMode())
    {
        strncpy(str, "SoundBank", TEXT_SIZE);
    }
    else if (synthesizer->GetStandBy(*channel))
    {
        strncpy(str, "Store current", TEXT_SIZE);
    }
    else
    {
        snprintf(str, TEXT_SIZE, "Program %03i", synthesizer->GetNumProgr(*channel));
    }
    return true;
}

int CButton::GetType()
{
    return this->type;
}
