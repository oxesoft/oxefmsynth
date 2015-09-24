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

#include "constants.h"
#include "persist.h"
#include "toolkit.h"
#include "programs.h"
#include "bank0.h"
#include "bank1.h"
#include <string.h>

CPersist::CPersist()
{
    bank_from_host = -1;
    banks_count = 0;
    memset(banks, 0, sizeof(banks));
    for (int i = 0; i < MAX_BANKS; i++)
    {
        memset(SoundBankNames[i], 0, TEXT_SIZE);
    }
    AddBank((void*)bank0, sizeof(bank0), (char*)"bank0 (internal)", false);
    AddBank((void*)bank1, sizeof(bank1), (char*)"bank1 (internal)", false);
}

int CPersist::AddBank(void *bank, unsigned int size, char *name, bool from_host)
{
    int index = banks_count;
    if (from_host == true)
    {
        if (bank_from_host > -1)
        {
            index = bank_from_host;
        }
        else
        {
            bank_from_host = banks_count++;
        }
    }
    else if (banks_count >= MAX_BANKS - 1)
    {
        return -1;
    }
    else
    {
        banks_count++;
    }
    if (bank != NULL && size == SOUNDBANK_SIZE)
    {
        memcpy(banks[index], bank, size);
        strncpy(SoundBankNames[index], name, TEXT_SIZE);
    }
    return index;
}

int CPersist::GetNumberBanks()
{
    return banks_count;
}

void CPersist::GetBankName(char *str, int index)
{
    strncpy(str, SoundBankNames[index], TEXT_SIZE);
}

void* CPersist::GetSoundBank(int index)
{
    return banks[index];
}
