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

#include "editor.h"
#include "ostoolkit.h"

#ifdef _WIN32
void* hInstance;
#endif

class CDummyHostInterface : public CHostInterface
{
private:
    CSynthesizer *synth;
public:
    CDummyHostInterface(CSynthesizer *synth)
    {
        this->synth = synth;
    }
    void ReceiveMessageFromPlugin(unsigned int messageID, unsigned int par1, unsigned int par2)
    {
        switch (messageID)
        {
            case UPDATE_DISPLAY:
                break;
            case SET_PROGRAM:
            {
                char channel = (char)par1;
                unsigned char numprog = (unsigned char)par2;
                if (synth)
                {
                    synth->SendEvent(0xC0 + channel, numprog, 0, 0);
                }
                break;
            }
            case SET_PARAMETER:
                break;
        }
    }
};

int main(void)
{
#ifdef _WIN32
    hInstance = GetModuleHandle(NULL);
#endif
    CSynthesizer s;
    CEditor e(&s);
    COSToolkit t(0, &e);
    e.SetToolkit(&t);
    CDummyHostInterface h(&s);
    e.SetHostInterface(&h);
    t.StartWindowProcesses();
    return t.WaitWindowClosed();
}
