/*
Oxe FM Synth: a software synthesizer
Copyright (C) 2004-2026  Daniel Moura <oxe@oxesoft.com>

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

#include "oxeclap.h"
#include "claphostinterface.h"

CClapHostInterface::CClapHostInterface(COxeClapPlugin *plugin)
    : plugin(plugin)
{
}

void CClapHostInterface::ReceiveMessageFromPlugin(unsigned int messageID, unsigned int par1, unsigned int par2)
{
    if (!plugin) return;

    switch (messageID)
    {
        case UPDATE_DISPLAY:
        {
            plugin->updateHostDisplay();
            break;
        }
        case SET_PROGRAM:
        {
            unsigned char numprog = (unsigned char)par2;
            plugin->setProgramFromGUI(numprog);
            break;
        }
        case SET_PARAMETER:
        {
            int index = (int)par1;
            float value = (float)par2 / MAXPARVALUE;
            plugin->setParameterFromGUI(index, value);
            break;
        }
    }
}
