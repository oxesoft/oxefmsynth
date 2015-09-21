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

#include <X11/Xlib.h>
#include <stdio.h>
#include "editor.h"
#include "xlibtoolkit.h"

CXlibToolkit::CXlibToolkit(Display *display, Window parentWindow)
{
    this->display = display;
    window = XCreateSimpleWindow(display, parentWindow, 0, 0, GUI_WIDTH, GUI_HEIGHT, 0, 0, 0);
    XGCValues gcvalues = {0};
    gc = XCreateGC(display, window, 0, &gcvalues);
    XSelectInput(display, window, ExposureMask | KeyPressMask);
    XMapWindow(display, window);
}

CXlibToolkit::~CXlibToolkit()
{
    XFreeGC(display, gc);
    XDestroyWindow(display, window);
}

void CXlibToolkit::CopyRect(int destX, int destY, int width, int height, int origBmp, int origX, int origY)
{
}

void CXlibToolkit::SendMessageToHost(unsigned int messageID, unsigned int par1, unsigned int par2)
{
}

void CXlibToolkit::GetMousePosition(int *x, int *y)
{
}

void CXlibToolkit::StartMouseCapture()
{
}

void CXlibToolkit::StopMouseCapture()
{
}

void CXlibToolkit::OutputDebugString(char *text)
{
    printf("%s\n", text);
}

void *CXlibToolkit::GetImageBuffer()
{
    return 0;
}

int CXlibToolkit::CommonWindowProc(XEvent *e)
{
    return -1;
}
