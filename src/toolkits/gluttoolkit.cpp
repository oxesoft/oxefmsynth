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

#include "nonguitoolkit.h"
#include "editor.h"
#if defined(__APPLE__)
    #include <GLUT/glut.h>
#elif defined(__linux)
    #include <GL/freeglut.h>
#endif
#include "opengltoolkit.h"
#include "gluttoolkit.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

CGlutToolkit *toolkit = NULL;

void draw()
{
    if (!toolkit)
    {
        return;
    }
    toolkit->Draw();
}

void display()
{
    unsigned int time = GetTick();
    draw();
    printf("opengl draw time: %ums\n", GetTick() - time);
}

void mouseClick(int button, int state, int x, int y)
{
    if (!toolkit)
    {
        return;
    }
    if (state == GLUT_DOWN)
    {
        int w = glutGet(GLUT_WINDOW_WIDTH);
        int h = glutGet(GLUT_WINDOW_HEIGHT);
        x = GUI_WIDTH * x / w;
        y = GUI_HEIGHT * y / h;
        toolkit->editor->OnLButtonDown(x, y);
    }
    else if (state == GLUT_UP)
    {
        toolkit->editor->OnLButtonUp();
    }
}

void mouseMove(int x, int y)
{
    if (!toolkit)
    {
        return;
    }
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);
    x = GUI_WIDTH * x / w;
    y = GUI_HEIGHT * y / h;
    toolkit->editor->OnMouseMove(x, y);
}

void* updateProc(void* ptr)
{
    if (!toolkit)
    {
        return NULL;
    }
    CGlutToolkit *toolkit = (CGlutToolkit*)ptr;
    while (!toolkit->stopUpdate)
    {
        glutPostRedisplay();
        usleep(1000 * TIMER_RESOLUTION_MS);
    }
    toolkit->updateStopped = true;
    return NULL;
}

void stopUpdateThread(void)
{
    if (!toolkit)
    {
        return;
    }
    toolkit->stopUpdate = true;
    while (!toolkit->updateStopped)
    {
        usleep(1000 * 100);
    }
}

CGlutToolkit::CGlutToolkit(void *parentWindow, CEditor *editor)
{
    toolkit = this;
    this->parentWindow  = parentWindow;
    this->editor        = editor;

    int argc = 1;
    char *argv[1] = {(char*)""};
    glutInit(&argc, argv);
    glutInitWindowSize(GUI_WIDTH, GUI_HEIGHT);
    glutCreateWindow(TITLE_FULL);
    glutDisplayFunc(display);
    glutMouseFunc(mouseClick);
    glutMotionFunc(mouseMove);
    glutPassiveMotionFunc(mouseMove);
#if defined(__linux)
    glutCloseFunc(stopUpdateThread);
#endif
    Init(editor);
}

CGlutToolkit::~CGlutToolkit()
{
    Deinit();
}

void CGlutToolkit::StartWindowProcesses()
{
    toolkit->stopUpdate    = false;
    toolkit->updateStopped = false;
    pthread_t thread;
    pthread_create(&thread, NULL, &updateProc, (void*)this);
}

int CGlutToolkit::WaitWindowClosed()
{
#ifdef PROFILING
    unsigned int time = GetTick();
    int i = 0;
    while (GetTick() - time < 5000)
    {
        draw();
        i++;
    }
    printf("%u fps\n", i / 5);
#else
    glutMainLoop();
#endif
    return 0;
}
