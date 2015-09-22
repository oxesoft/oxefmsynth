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
#include <X11/Xutil.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "editor.h"
#include "xlibtoolkit.h"

struct BITMAPFILEHEADER
{
    char         signature[2];
    unsigned int fileSize;
    short        reserved[2];
    unsigned int fileOffsetToPixelArray;
} __attribute__((packed));

struct BITMAPV5HEADER
{
    unsigned int   dibHeaderSize;
    unsigned int   width;
    unsigned int   height;
    unsigned short planes;
    unsigned short bitsPerPixel;
    unsigned int   compression;
    unsigned int   imageSize;
} __attribute__((packed));

struct BITMAPHEADER
{
    BITMAPFILEHEADER fh;
    BITMAPV5HEADER   v5;
};

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
            {
                XGraphicsExposeEvent *e = (XGraphicsExposeEvent*)&event;
                printf("Expose x=%d y=%d w=%d h=%d\n", e->x, e->y, e->width, e->height);
                XCopyArea(toolkit->display, toolkit->offscreen, toolkit->window, toolkit->gc, e->x, e->y, e->width, e->height, e->x, e->y);
                break;
            }
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
    this->editor  = editor;
    window = XCreateSimpleWindow(this->display, (Window)parentWindow, 0, 0, GUI_WIDTH, GUI_HEIGHT, 0, 0, 0);
    
    gc = XCreateGC(this->display, window, 0, 0);
    XSelectInput(this->display, window, ButtonPressMask | ButtonReleaseMask | ExposureMask | KeyPressMask);
    XMapWindow(this->display, window);
    XFlush(this->display);
    
    this->customMessage = XInternAtom(display, "_customMessage", false);
    
    threadFinished = false;
    pthread_t thread;
    pthread_create(&thread, NULL, &eventProc, (void*)this);
   
    offscreen = XCreatePixmap(this->display, window, GUI_WIDTH, GUI_HEIGHT, 24);
    if (XCreatePixmap)

    memset(bmps    , 0, sizeof(bmps));
    memset(bmpsData, 0, sizeof(bmpsData));

    FILE *f = fopen("skins/default/bg.bmp", "rb");
    if (!f)
    {
        printf("file not found\n");
        return;                                                                                                         
    }
    BITMAPHEADER header = {0};
    if (!fread(&header, sizeof(header), 1, f))
    {
        fclose(f);
        return;                                                                                                         
    }
    fclose(f);

    printf("signature              %c%c\n", header.fh.signature[0], header.fh.signature[1]);
    printf("fileSize               %u\n", header.fh.fileSize);
    printf("fileOffsetToPixelArray %u\n", header.fh.fileOffsetToPixelArray);
    printf("width                  %u\n", header.v5.width);
    printf("height                 %u\n", header.v5.height);
    printf("bitsPerPixel           %u\n", header.v5.bitsPerPixel);
    printf("imageSize              %u\n", header.v5.imageSize);
    
    bmpsData[BMP_BG] = (char*)malloc(header.v5.imageSize);
    f = fopen("skins/default/bg.bmp", "rb");
    fseek(f, header.fh.fileOffsetToPixelArray, SEEK_SET);
    if (!fread(bmpsData[BMP_BG], header.v5.imageSize, 1, f))
    {
        fclose(f);
        return;
    }
    fclose(f);
    //bmps[BMP_BG] = XCreateImage(this->display, CopyFromParent, header.v5.bitsPerPixel, ZPixmap, 0, bmpsData[BMP_BG], header.v5.width, header.v5.height, 32, 0);
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
    XFreePixmap(display, offscreen);
    XCloseDisplay(display);
    for (int i = 0; i < BMP_COUNT; i++)
    {
        if (bmps[i])
        {
            XDestroyImage(bmps[i]);
        }
        if (bmpsData[i])
        {
            free(bmpsData[i]);
        }
    }
}

void CXlibToolkit::CopyRect(int destX, int destY, int width, int height, int origBmp, int origX, int origY)
{
    if (!bmps[origBmp])
    {
        return;
    }
    //printf("XPutImage\n");
    //GC gc = XCreateGC(display, offscreen, 0, 0);
    //XPutImage(display, offscreen, gc, bmps[origBmp], origX, origY, destX, destY, width, height);
    //XFreeGC(display, gc);
    printf("1\n");
    XClearArea(display, window, destX, destY, width, height, true);
    printf("2\n");
    //XSync(display, false);
    printf("3\n");
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
