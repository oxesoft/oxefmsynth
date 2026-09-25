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

#ifndef __OXEDMO__

class CToolkit
{
public:
    virtual ~CToolkit() {}
    virtual void Invalidate() {}
    virtual void InvalidateRect(int x, int y, int width, int height) { Invalidate(); }
    virtual void CopyRect(int destX, int destY, int width, int height, int origBmp, int origX, int origY) {}
    virtual void StartMouseCapture() {}
    virtual void StopMouseCapture() {}
    virtual void StartWindowProcesses() {}
    virtual int  WaitWindowClosed() {return 0;}   // standalone only
    virtual float GetScale() { return 1.0f; }
    virtual void Resize(int width, int height) {}
};

#endif
