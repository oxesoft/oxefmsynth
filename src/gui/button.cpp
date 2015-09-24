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

#define BUTTON_WIDTH   42
#define BUTTON_HEIGHT  16

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
                if (hostinterface)
                {
                    hostinterface->ReceiveMessageFromPlugin(SET_PROGRAM, *channel, numprog);
                }
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
                if (hostinterface)
                {
                    hostinterface->ReceiveMessageFromPlugin(SET_PROGRAM, *channel, numprog);
                }
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
                if (hostinterface)
                {
                    hostinterface->ReceiveMessageFromPlugin(SET_PROGRAM, *channel, numprog);
                }
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
                if (hostinterface)
                {
                    hostinterface->ReceiveMessageFromPlugin(SET_PROGRAM, *channel, numprog);
                }
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

void CButton::Repaint()
{
    if (toolkit)
    {
        toolkit->CopyRect(this->left, this->top, this->right - this->left, this->bottom - this->top, this->bmp, 0, index * BUTTON_HEIGHT);
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
