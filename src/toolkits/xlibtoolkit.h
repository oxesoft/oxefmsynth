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

#include "toolkit.h"
#include <blend2d/blend2d.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

class CEditor;

class CXlibToolkit : public CToolkit
{
private:
    BLImage      blImage;
public:
    void        *parentWindow;
    CEditor     *editor;
    Display     *display;
    Window       window;
    GC           gc;
    Atom         WM_DELETE_WINDOW;
    Atom         WM_TIMER;
    bool         thread1Finished;
    bool         thread2Finished;
    CXlibToolkit(void *parentWindow, CEditor *editor);
    virtual ~CXlibToolkit();
    virtual void Invalidate() override;
    virtual void InvalidateRect(int x, int y, int width, int height) override;
    virtual void CopyRect(int destX, int destY, int width, int height, int origBmp, int origX, int origY) override;
    virtual void StartMouseCapture() override;
    virtual void StopMouseCapture() override;
    virtual void StartWindowProcesses() override;
    virtual int  WaitWindowClosed() override;
    virtual float GetScale() override;
    void Resize(int width, int height);
    void Draw(int x = 0, int y = 0, int w = 0, int h = 0);
};
