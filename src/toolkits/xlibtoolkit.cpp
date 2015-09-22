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
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "editor.h"
#include "xlibtoolkit.h"

void* eventProc(void* ptr)
{
    XEvent event;
    CXlibToolkit *toolkit = (CXlibToolkit*)ptr;
    bool stopThread = false;
    while (!stopThread)
    {
        XNextEvent(toolkit->display, &event);
        if (event.xany.display != toolkit->display || event.xany.window != toolkit->window)
        {
            continue;
        }
        switch (event.type) 
        {
            case ButtonPress: 
                printf("ButtonPress\n");
                break;
            case ButtonRelease:
                printf("ButtonRelease\n");
                break;
            case Expose:
                printf("Expose\n");
                break;
            case KeyPress: 
                printf("KeyPress\n");
                break;
            case ClientMessage:
            {
                printf("ClientMessage\n");
                XClientMessageEvent *message = (XClientMessageEvent *)&event;
                if (message->message_type == toolkit->customMessage)
                {
                    unsigned int messageID = message->data.l[0];
                    unsigned int par1      = message->data.l[1];
                    unsigned int par2      = message->data.l[2];
                    if (messageID == KILL_EDITOR)
                    {
                        stopThread = true;
                        break;
                    }
                }
            }
        }
    }
    printf("exit eventProc\n");
    toolkit->threadFinished = true;
    return NULL;
}

CXlibToolkit::CXlibToolkit(void *parentWindow, CEditor *editor)
{
    char *displayName = getenv("DISPLAY");
    if (!displayName || !strlen(displayName))
    {
        displayName = (char*)":0.0";
    }
    this->display = XOpenDisplay(displayName);
    window = XCreateSimpleWindow(this->display, (Window)parentWindow, 0, 0, GUI_WIDTH, GUI_HEIGHT, 0, 0, 0);
    
    XGCValues gcvalues = {0};
    gc = XCreateGC(this->display, window, 0, &gcvalues);
    XSelectInput(this->display, window, ButtonPressMask | ButtonReleaseMask | ExposureMask | KeyPressMask);
    XMapWindow(this->display, window);
    XFlush(this->display);
    
    this->customMessage = XInternAtom(display, "_customMessage", false);
    
    threadFinished = false;
    pthread_t thread;
    pthread_create(&thread, NULL, &eventProc, (void*)this);
    
    /*
    Generally you create your own loader to grab the pixels out of whatever
    image format you need. Then, you use XCreateImage to make an XImage, 
    which you put, using XPutImage, on an offscreen pixmap you generate with
    XCreatePixmap. Once you have your pixmap, you paint it to the window
    with XCopyArea. You must re-copy the image on any expose events.
    */
}

CXlibToolkit::~CXlibToolkit()
{
    SendMessageToHost(KILL_EDITOR, 0, 0);
    while (!threadFinished)
    {
        usleep(1000);
    }
    XFreeGC(display, gc);
    XDestroyWindow(display, window);
    XSync(display, false);
}

void CXlibToolkit::CopyRect(int destX, int destY, int width, int height, int origBmp, int origX, int origY)
{
    /*
    XCopyArea
    XClearArea
    XSync
    */
}

void CXlibToolkit::SendMessageToHost(unsigned int messageID, unsigned int par1, unsigned int par2)
{
    XClientMessageEvent event;
    event.display      = display;
    event.window       = window;
    event.type         = ClientMessage;
    event.format       = 8;
    event.data.l[0]    = messageID;
    event.data.l[1]    = par1;
    event.data.l[2]    = par2;
    event.message_type = customMessage;
    XSendEvent(display, window, false, 0L, (XEvent*)&event);
    XFlush(display);
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
