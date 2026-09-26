/*
Oxe FM Synth: a software synthesizer
Copyright (C) 2004-2026  Daniel Moura <oxesoft@gmail.com>

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

#include "oxevst3editor.h"
#include "oxevst3.h"
#include "oxevst3cids.h"
#include "public.sdk/source/main/pluginfactory.h"

using namespace Steinberg::Vst;

BEGIN_FACTORY_DEF ("Oxe Music Software", "https://github.com/oxesoft/oxefmsynth", "oxesoft@gmail.com")

    DEF_CLASS2 (INLINE_UID (0x6F786566, 0x6D73796E, 0x74687673, 0x74333031),
                PClassInfo::kManyInstances,
                kVstAudioEffectClass,
                "Oxe FM Synth",
                0,
                PlugType::kInstrumentSynth,
                VERSION_STR,
                kVstVersionString,
                Steinberg::Vst::COxeVst3::createInstance)

END_FACTORY

#if defined(_WIN32)
void* hInstance = nullptr;
#endif
