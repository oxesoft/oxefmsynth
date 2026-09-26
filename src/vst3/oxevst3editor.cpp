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

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

COxeVst3Editor::COxeVst3Editor(Steinberg::Vst::COxeVst3* plugin, CSynthesizer* synth)
    : refCount(1), plugin(plugin), synth(synth), toolkit(nullptr), hostinterface(nullptr), plugFrame(nullptr)
{
    oxeeditor = new CEditor(synth);
    if (plugin)
    {
        plugin->setActiveEditor(this);
    }
}

COxeVst3Editor::~COxeVst3Editor()
{
    if (plugin)
    {
        plugin->onEditorDestroyed(this);
    }
    delete oxeeditor;
}

Steinberg::tresult PLUGIN_API COxeVst3Editor::isPlatformTypeSupported(Steinberg::FIDString type)
{
#if defined(__APPLE__)
    if (strcmp(type, Steinberg::kPlatformTypeNSView) == 0)
        return Steinberg::kResultTrue;
#elif defined(_WIN32)
    if (strcmp(type, Steinberg::kPlatformTypeHWND) == 0)
        return Steinberg::kResultTrue;
#elif defined(__linux__)
    if (strcmp(type, Steinberg::kPlatformTypeX11EmbedWindowID) == 0)
        return Steinberg::kResultTrue;
#endif
    return Steinberg::kResultFalse;
}

Steinberg::tresult PLUGIN_API COxeVst3Editor::attached(void* parent, Steinberg::FIDString type)
{
    if (isPlatformTypeSupported(type) != Steinberg::kResultTrue)
        return Steinberg::kResultFalse;

#if defined(_WIN32)
    extern HINSTANCE ghInst;
    extern void* hInstance;
    if (!hInstance)
        hInstance = (void*)ghInst;
    if (!hInstance)
        hInstance = (void*)GetModuleHandle(NULL);
#endif

    hostinterface = new CVst3HostInterface(plugin);
    toolkit = new COSToolkit(parent, oxeeditor);
    oxeeditor->SetToolkit(toolkit);
    oxeeditor->SetHostInterface(hostinterface);
    toolkit->StartWindowProcesses();
    return Steinberg::kResultTrue;
}

Steinberg::tresult PLUGIN_API COxeVst3Editor::removed()
{
    if (oxeeditor)
    {
        oxeeditor->SetToolkit(nullptr);
        oxeeditor->SetHostInterface(nullptr);
    }
    delete toolkit;
    toolkit = nullptr;
    delete hostinterface;
    hostinterface = nullptr;
    return Steinberg::kResultTrue;
}

Steinberg::tresult PLUGIN_API COxeVst3Editor::getSize(Steinberg::ViewRect* size)
{
    if (!size)
        return Steinberg::kInvalidArgument;
    size->left = 0;
    size->top = 0;
    size->right = GUI_WIDTH;
    size->bottom = GUI_HEIGHT;
    return Steinberg::kResultTrue;
}

Steinberg::tresult PLUGIN_API COxeVst3Editor::onSize(Steinberg::ViewRect* newSize)
{
    if (!newSize) return Steinberg::kInvalidArgument;
    int w = newSize->right - newSize->left;
    int h = newSize->bottom - newSize->top;
    if (toolkit)
    {
        toolkit->Resize(w, h);
    }
    return Steinberg::kResultTrue;
}

Steinberg::tresult PLUGIN_API COxeVst3Editor::onFocus(Steinberg::TBool state)
{
    return Steinberg::kResultTrue;
}

Steinberg::tresult PLUGIN_API COxeVst3Editor::setFrame(Steinberg::IPlugFrame* frame)
{
    plugFrame = frame;
    return Steinberg::kResultTrue;
}

Steinberg::tresult PLUGIN_API COxeVst3Editor::canResize()
{
    return Steinberg::kResultTrue;
}

Steinberg::tresult PLUGIN_API COxeVst3Editor::checkSizeConstraint(Steinberg::ViewRect* rect)
{
    if (!rect)
        return Steinberg::kInvalidArgument;
    int width = rect->right - rect->left;
    if (width < 475) width = 475;
    int height = (int)((width * (double)GUI_HEIGHT) / (double)GUI_WIDTH);
    rect->right = rect->left + width;
    rect->bottom = rect->top + height;
    return Steinberg::kResultTrue;
}

Steinberg::tresult PLUGIN_API COxeVst3Editor::onWheel(float distance)
{
    return Steinberg::kResultFalse;
}

Steinberg::tresult PLUGIN_API COxeVst3Editor::onKeyDown(Steinberg::char16 key, Steinberg::int16 keyCode, Steinberg::int16 modifiers)
{
    return Steinberg::kResultFalse;
}

Steinberg::tresult PLUGIN_API COxeVst3Editor::onKeyUp(Steinberg::char16 key, Steinberg::int16 keyCode, Steinberg::int16 modifiers)
{
    return Steinberg::kResultFalse;
}

Steinberg::tresult PLUGIN_API COxeVst3Editor::queryInterface(const Steinberg::TUID iid, void** obj)
{
    if (!obj)
        return Steinberg::kInvalidArgument;
    if (Steinberg::FUnknownPrivate::iidEqual(iid, Steinberg::IPlugView::iid) ||
        Steinberg::FUnknownPrivate::iidEqual(iid, Steinberg::FUnknown::iid))
    {
        addRef();
        *obj = static_cast<Steinberg::IPlugView*>(this);
        return Steinberg::kResultOk;
    }
    *obj = nullptr;
    return Steinberg::kNoInterface;
}

Steinberg::uint32 PLUGIN_API COxeVst3Editor::addRef()
{
    return ++refCount;
}

Steinberg::uint32 PLUGIN_API COxeVst3Editor::release()
{
    if (--refCount == 0)
    {
        delete this;
        return 0;
    }
    return refCount;
}
