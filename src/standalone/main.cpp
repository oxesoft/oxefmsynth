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

#ifdef __linux
#include "xlibtoolkit.h"
#else
#include "windowstoolkit.h"
void* hInstance;
#endif

#ifdef __linux
int main(void)
#else
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif
{
    CSynthesizer s;
    CEditor e(&s);
#ifdef __linux
    CXlibToolkit t(0, &e, NULL, &s);
#else
    hInstance = (void*)hInst;
    CWindowsToolkit t(0, &e, NULL, &s);
#endif
    e.SetToolkit(&t);
    return t.WaitWindowClosed();
}
