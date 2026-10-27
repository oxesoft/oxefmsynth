/*
Oxe FM Synth: a software synthesizer
Copyright (C) 2004-2015  Daniel Moura <oxesoft@gmail.com>

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

class CEditor;

class CCocoaToolkit : public CToolkit
{
private:
    void *objcInstance;
    void *impl;
public:
    void        *parentWindow;
    CEditor     *editor;
    CCocoaToolkit(void *parentWindow, CEditor *editor);
    virtual ~CCocoaToolkit() override;
    virtual void Invalidate() override;
    virtual void InvalidateRect(int x, int y, int width, int height) override;
    virtual void StartMouseCapture() override;
    virtual void StopMouseCapture() override;
    virtual void StartWindowProcesses() override;
    virtual int  WaitWindowClosed() override;
    virtual float GetScale() override;
    virtual void Resize(int width, int height) override;
    virtual bool IsShiftDown() override;
};

