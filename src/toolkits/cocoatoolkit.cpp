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
#if defined(__linux)
    #include <GL/gl.h>
#else
    #include <OpenGL/gl.h>
#endif
#include "opengltoolkit.h"
#include "cocoatoolkit.h"
#include <stdio.h>
#include <string.h>
#ifndef __USE_GNU
#define __USE_GNU
#endif
#include <dlfcn.h>
#define PATH_MAX 512

void GetResourcesPath(char *path, int size)
{
    Dl_info info;
    dladdr((void*)GetResourcesPath, &info);
    strncpy(path, info.dli_fname, PATH_MAX);
    char* tmp = strrchr(path, '/');
    *tmp = 0;
    strcat(path, "/../../../"BMP_PATH"/");
}

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

void CppOpenGLInit(void *toolkit)
{
    CCocoaToolkit *t = (CCocoaToolkit*)toolkit;
    char path[PATH_MAX];
    GetResourcesPath(path, sizeof(path));
    t->Init(t->editor, path);
}

void CppOpenGLDeinit(void *toolkit)
{
    ((CCocoaToolkit*)toolkit)->Deinit();
}

void CppOpenGLDraw(void *toolkit)
{
    ((CCocoaToolkit*)toolkit)->Draw();
}
