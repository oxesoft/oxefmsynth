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

#ifdef __WITH_DSOUND__
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include "controller.h"
#include <stdio.h>
#include <string.h>
#include "nonguitoolkit.h"

int main(int argc, char* argv[])
{
    CController controller;
    printf(TITLE_FULL);
    printf("\n");
#ifdef __RENDER_TO_MEMORY__ONLY__
    unsigned int time = GetTick();
    controller.RenderToProfiling();
    time = GetTick() - time;
    printf("Total time: %u\n", time);
#else
    if (argc>1)
    {
        if (!strcmp(argv[1], "-f"))
        {
            printf("Writing the file...\n");
            unsigned int time = GetTick();
            controller.RenderToFile();
            time = GetTick() - time;
            printf("Total time: %u\n", time);
        }
        else if (!strcmp(argv[1], "-m"))
        {
            printf("Rendering to memory...\n");
            unsigned int time = GetTick();
            controller.RenderToProfiling();
            time = GetTick() - time;
            printf("Total time: %u\n", time);
        }
        else
        {
            printf("Invalid option.\n");
        }
#ifdef __DSOUND_INCLUDED__
    }
    else
    {
        controller.Start(GetForegroundWindow());
        printf("Press ESC to quit...\n");
        while (GetAsyncKeyState(VK_ESCAPE)>=0)
        {
            static const char* anim[]={"|","/","-","\\"};
            static       char  pos = 0;
            printf(anim[pos++&3]);
            printf("\r");
            Sleep(100);
        }
        controller.Stop();
#else
    }
    else
    {
        printf("Usage:\n    \"%s -f\" to render to a wav file\n    \"%s -m\" to render to memory (to measure performance)\n", EXECUTABLE_NAME, EXECUTABLE_NAME);
#endif
    }
#endif
    return 0;
}
