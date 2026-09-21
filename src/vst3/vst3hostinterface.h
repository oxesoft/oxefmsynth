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

#pragma once

namespace Steinberg {
namespace Vst {
class COxeVst3;
}
}

class CVst3HostInterface : public CHostInterface
{
private:
    Steinberg::Vst::COxeVst3 *plugin;
public:
    CVst3HostInterface(Steinberg::Vst::COxeVst3 *plugin);
    void ReceiveMessageFromPlugin(unsigned int messageID, unsigned int par1, unsigned int par2) override;
};
