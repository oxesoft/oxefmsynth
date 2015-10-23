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

#if defined(__APPLE__)
    #if defined(__GLUT__)
        #include <GLUT/glut.h>
        #include "gluttoolkit.h"
        #define COSToolkit CGlutToolkit
    #else
        #include "cocoatoolkit.h"
        #define COSToolkit CCocoaToolkit
    #endif
#elif defined(__linux)
    #if defined(__GLUT__)
        #define GL_GLEXT_PROTOTYPES
        #include <GL/glut.h>
        #include "gluttoolkit.h"
        #define COSToolkit CGlutToolkit
    #else
        #include <X11/Xlib.h>
        #include "xlibtoolkit.h"
        #define COSToolkit CXlibToolkit
    #endif
#else
    #include <windows.h>
    #include "windowstoolkit.h"
    #define COSToolkit CWindowsToolkit
#endif
