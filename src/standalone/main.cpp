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
#include "ostoolkit.h"
#include "nonguitoolkit.h"
#include <stdio.h>

#ifdef _WIN32
void* hInstance;
#endif

int main(void)
{
#ifdef _WIN32
    hInstance = GetModuleHandle(NULL);
#endif
    CSynthesizer s;
    CEditor e(&s);
    COSToolkit t(0, &e);
    unsigned int time = GetTick();
    e.SetToolkit(&t);
    time = GetTick() - time;
    printf("full blitting time: %ums\n", time);
    t.StartWindowProcesses();
    return t.WaitWindowClosed();
}
