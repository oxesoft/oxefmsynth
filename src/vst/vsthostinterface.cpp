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

#include "public.sdk/source/vst2.x/audioeffectx.h"
#include "synthesizer.h"
#include "vsthostinterface.h"

CVstHostInterface::CVstHostInterface(AudioEffectX *effectx, CSynthesizer *synth)
{
    this->effectx = effectx;
    this->synth = synth;
}

void CVstHostInterface::ReceiveMessageFromPlugin(unsigned int messageID, unsigned int par1, unsigned int par2)
{
    switch (messageID)
    {
        case UPDATE_DISPLAY:
        {
            if (effectx)
            {
                effectx->updateDisplay();
            }
            break;
        }
        case SET_PROGRAM:
        {
            char channel = (char)par1;
            unsigned char numprog = (unsigned char)par2;
            if (channel == 0 && effectx)
            {
                effectx->setProgram(numprog);
                effectx->updateDisplay();
            }
            else if (synth)
            {
                synth->SendEvent(0xC0 + channel, numprog, 0, 0);
            }
            break;
        }
        case SET_PARAMETER:
        {
            int index = (int)par1;
            float value = (float)par2 / MAXPARVALUE;
            if (effectx)
            {
                effectx->setParameterAutomated(index, value);
            }
            break;
        }
    }
}
