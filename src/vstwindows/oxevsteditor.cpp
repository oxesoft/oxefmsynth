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
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <strsafe.h>
#include "oxevsteditor.h"
#include "oxevst.h"
#include "windowstoolkit.h"

//-----------------------------------------------------------------------------

static int g_useCount = 0;
extern void* hInstance;

//-----------------------------------------------------------------------------

LRESULT CALLBACK WindowProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
#ifdef _WIN64
    COxeVstEditor* editor = (COxeVstEditor*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
#else
    COxeVstEditor* editor = (COxeVstEditor*)GetWindowLong (hwnd, GWL_USERDATA);
#endif
    if (!editor)
    {
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
    if (((CWindowsToolkit*)editor->getEditor()->GetToolkit())->CommonWindowProc(hwnd, message, wParam, lParam, editor->getEditor(), (HWND)editor->getSystemWindow()) == 0)
    {
        return 0L;
    }
    switch (message)
    {
    case WM_USER + UPDATE_DISPLAY:
    {
        editor->getEffectX()->updateDisplay();
        return 0L;
    }
    case WM_USER + SET_PROGRAM:
    {
        char channel = (char)wParam;
        unsigned char numprog = (unsigned char)lParam;
        if (channel == 0)
        {
            editor->getEffectX()->setProgram(numprog);
            editor->getEffectX()->updateDisplay();
        }
        else
        {
            editor->getSynth()->SendEvent(0xC0 + channel, numprog, 0, 0);
        }
        return 0L;
    }
    case WM_USER + SET_PARAMETER:
    {
        int index = (int)wParam;
        float value = (float)lParam / MAXPARVALUE;
        editor->getEffect()->setParameterAutomated(index, value);
        return 0L;
    }
    default:
        break;
    }
    return DefWindowProc (hwnd, message, wParam, lParam);
}

//-----------------------------------------------------------------------------

COxeVstEditor::COxeVstEditor (AudioEffectX *effectx, CSynthesizer *synth)
    :
    AEffEditor(effectx),
    effectx(effectx),
    synth(synth)
{
    oxeeditor = new CEditor(synth);
    effect->setEditor(this);
    this->toolkit = NULL;
}

//-----------------------------------------------------------------------------

COxeVstEditor::~COxeVstEditor ()
{
    delete oxeeditor;
}

//-----------------------------------------------------------------------------

bool COxeVstEditor::getRect (ERect **erect)
{
    static ERect r = {0, 0, GUI_HEIGHT, GUI_WIDTH };
    *erect = &r;
    return true;
}

//-----------------------------------------------------------------------------

bool COxeVstEditor::open (VstIntPtr value, void *ptr)
{
    // Remember the parent window
    systemWindow = ptr;

    // Create window class, if we are called the first time
    g_useCount++;
    if (g_useCount == 1)
    {
        WNDCLASSW windowClass;
        windowClass.style = CS_DBLCLKS;
        windowClass.lpfnWndProc = WindowProc;
        windowClass.cbClsExtra = 0;
        windowClass.cbWndExtra = 0;
        windowClass.hInstance = (HINSTANCE)hInstance;
        windowClass.hIcon = 0;
        windowClass.hCursor = 0;
        windowClass.hbrBackground = NULL;
        windowClass.lpszMenuName = 0;
        windowClass.lpszClassName = L"OxeVstEditorClass";
        ATOM a = RegisterClassW(&windowClass);
    }

    // Create our base window
    HWND hwnd = CreateWindowExW
                (
                0,
                L"OxeVstEditorClass",
                L"",
                WS_CHILD | WS_VISIBLE,
                0,
                0,
                GUI_WIDTH,
                GUI_HEIGHT, 
                (HWND)systemWindow,
                NULL,
                (HINSTANCE)hInstance,
                NULL
                );

#ifdef _WIN64
    SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)this);
#else
    SetWindowLong(hwnd, GWL_USERDATA, (LONG)this);
#endif
    this->toolkit = new CWindowsToolkit((HINSTANCE)hInstance, hwnd);
    oxeeditor->SetToolkit(this->toolkit);

    return true;
}

//-----------------------------------------------------------------------------

void COxeVstEditor::close ()
{
    g_useCount--;
    if (g_useCount == 0)
    {
        UnregisterClassW(L"OxeVstEditorClass", (HINSTANCE)hInstance);
    }
    oxeeditor->SetToolkit(NULL);
    delete this->toolkit;
    this->toolkit = NULL;
}
