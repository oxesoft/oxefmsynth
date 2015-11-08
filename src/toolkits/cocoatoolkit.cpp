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
#include "cocoawrapper.h"
#include "cocoatoolkit.h"
#include <stdio.h>

CCocoaToolkit::CCocoaToolkit(void *parentWindow, CEditor *editor)
{
    this->parentWindow  = parentWindow;
    this->editor        = editor;
    this->objcInstance  = CocoaToolkitCreate((void*)this);
    CocoaToolkitCreateWindow(this->objcInstance, parentWindow);
}

CCocoaToolkit::~CCocoaToolkit()
{
    CocoaToolkitDestroy(this->objcInstance);
}

void CCocoaToolkit::StartWindowProcesses()
{
    CocoaToolkitShowWindow(this->objcInstance);
}

void CCocoaToolkit::CopyRect(int destX, int destY, int width, int height, int origBmp, int origX, int origY)
{
    CocoaToolkitCopyRect(this->objcInstance, destX, destY, width, height, origBmp, origX, origY);
}

int CCocoaToolkit::WaitWindowClosed()
{
    CocoaToolkitWaitWindowClosed(this->objcInstance);
    return 0;
}

void CppOnLButtonDown(void *toolkit, int x, int y)
{
    ((CCocoaToolkit*)toolkit)->editor->OnLButtonDown(x, y);
}

void CppOnLButtonUp(void *toolkit)
{
    ((CCocoaToolkit*)toolkit)->editor->OnLButtonUp();
}

void CppOnDblClick(void *toolkit, int x, int y)
{
    ((CCocoaToolkit*)toolkit)->editor->OnLButtonDblClick(x, y);
}

void CppOnMouseMove(void *toolkit, int x, int y)
{
    ((CCocoaToolkit*)toolkit)->editor->OnMouseMove(x, y);
}

void CppOnChar(void *toolkit, int c)
{
    ((CCocoaToolkit*)toolkit)->editor->OnChar(c);
}

void CppUpdate(void *toolkit)
{
    ((CCocoaToolkit*)toolkit)->editor->Update();
}
