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

#pragma once

#include "pluginterfaces/gui/iplugview.h"

#ifdef RELEASE
#undef RELEASE
#endif

#include "editor.h"
#include "vst3hostinterface.h"
#include "ostoolkit.h"
#include <atomic>

namespace Steinberg {
namespace Vst {
class COxeVst3;
}
}

class COxeVst3Editor : public Steinberg::IPlugView
{
public:
    COxeVst3Editor(Steinberg::Vst::COxeVst3* plugin, CSynthesizer* synth);
    virtual ~COxeVst3Editor();

    // IPlugView
    Steinberg::tresult PLUGIN_API isPlatformTypeSupported(Steinberg::FIDString type) override;
    Steinberg::tresult PLUGIN_API attached(void* parent, Steinberg::FIDString type) override;
    Steinberg::tresult PLUGIN_API removed() override;
    Steinberg::tresult PLUGIN_API onWheel(float distance) override;
    Steinberg::tresult PLUGIN_API onKeyDown(Steinberg::char16 key, Steinberg::int16 keyCode, Steinberg::int16 modifiers) override;
    Steinberg::tresult PLUGIN_API onKeyUp(Steinberg::char16 key, Steinberg::int16 keyCode, Steinberg::int16 modifiers) override;
    Steinberg::tresult PLUGIN_API getSize(Steinberg::ViewRect* size) override;
    Steinberg::tresult PLUGIN_API onSize(Steinberg::ViewRect* newSize) override;
    Steinberg::tresult PLUGIN_API onFocus(Steinberg::TBool state) override;
    Steinberg::tresult PLUGIN_API setFrame(Steinberg::IPlugFrame* frame) override;
    Steinberg::tresult PLUGIN_API canResize() override;
    Steinberg::tresult PLUGIN_API checkSizeConstraint(Steinberg::ViewRect* rect) override;

    // FUnknown
    Steinberg::tresult PLUGIN_API queryInterface(const Steinberg::TUID iid, void** obj) override;
    Steinberg::uint32 PLUGIN_API addRef() override;
    Steinberg::uint32 PLUGIN_API release() override;

    CEditor* getEditor() { return oxeeditor; }

private:
    std::atomic<Steinberg::uint32> refCount;
    Steinberg::Vst::COxeVst3* plugin;
    CSynthesizer* synth;
    CEditor* oxeeditor;
    CToolkit* toolkit;
    CVst3HostInterface* hostinterface;
    Steinberg::IPlugFrame* plugFrame;
};
