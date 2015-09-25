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
#include <X11/Xlib.h>
#include "xlibtoolkit.h"
#include <X11/Xutil.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define TIMER_RESOLUTION_MS 20
#define BMP_PATH "skins/default/"

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
    unsigned int time = 0;
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
            {
                XButtonEvent *e = (XButtonEvent*)&event;
                switch (e->button)
                {
                case 1:
                    if (e->time - time > 400)
                    {
                        toolkit->editor->OnLButtonDown(e->x, e->y);
                    }
                    else
                    {
                        toolkit->editor->OnLButtonDblClick(e->x, e->y);
                    }
                    time = e->time;
                    break;
                case 4:
                    toolkit->editor->OnMouseWheel(e->x, e->y,  1);
                    break;
                case 5:
                    toolkit->editor->OnMouseWheel(e->x, e->y, -1);
                    break;
                }
                break;
            }
            case ButtonRelease:
            {
                toolkit->editor->OnLButtonUp();
                break;
            }
            case MotionNotify:
            {
                XMotionEvent *e = (XMotionEvent*)&event;
                toolkit->editor->OnMouseMove(e->x, e->y);
                break;
            }
            case Expose:
            {
                XGraphicsExposeEvent *e = (XGraphicsExposeEvent*)&event;
                XCopyArea(toolkit->display, toolkit->offscreen, toolkit->window, toolkit->gc, e->x, e->y, e->width, e->height, e->x, e->y);
                break;
            }
            case ClientMessage:
            {
                XClientMessageEvent *message = (XClientMessageEvent *)&event;
                if (message->data.l[0] == toolkit->WM_DELETE_WINDOW)
                {
                    stopThread = true;
                    break;
                }
                else if (message->data.l[0] == toolkit->WM_TIMER)
                {
                    toolkit->editor->Update();
                }
            }
        }
    }
    toolkit->threadFinished = true;
    return NULL;
}

void* updateProc(void* ptr)
{
    CXlibToolkit *toolkit = (CXlibToolkit*)ptr;
    while (!toolkit->threadFinished)
    {
        XClientMessageEvent event;
        event.display      = toolkit->display;
        event.window       = toolkit->window;
        event.type         = ClientMessage;
        event.format       = 8;
        event.data.l[0]    = toolkit->WM_TIMER;
        event.message_type = toolkit->WM_TIMER;
        XSendEvent(toolkit->display, toolkit->window, false, 0L, (XEvent*)&event);
        XFlush(toolkit->display);
        usleep(1000 * TIMER_RESOLUTION_MS);
    }
}

CXlibToolkit::CXlibToolkit(void *parentWindow, CEditor *editor)
{
    this->parentWindow  = parentWindow;
    this->editor        = editor;

    char *displayName = getenv("DISPLAY");
    if (!displayName || !strlen(displayName))
    {
        displayName = (char*)":0.0";
    }
    if (!XInitThreads())
    {
        fprintf(stderr, "Xlib threads support unavailable");
        return;
    }
    this->display = XOpenDisplay(displayName);
    
    if (!parentWindow)
    {
        parentWindow = (void*)RootWindow(this->display, DefaultScreen(this->display));
    }
    
    window = XCreateWindow(this->display, (Window)parentWindow, 0, 0, GUI_WIDTH, GUI_HEIGHT, 0, 24, InputOutput, CopyFromParent, 0, 0);
    
    gc = XCreateGC(this->display, window, 0, 0);
    XSelectInput(this->display, window, ButtonPressMask | ButtonReleaseMask | PointerMotionMask | ExposureMask);
    XMapWindow(this->display, window);
    XFlush(this->display);
        
    this->WM_TIMER         = XInternAtom(this->display, "WM_TIMER"        , false); 
    this->WM_DELETE_WINDOW = XInternAtom(this->display, "WM_DELETE_WINDOW", false); 
    XSetWMProtocols(this->display, window, &WM_DELETE_WINDOW, 1);
    
    offscreen = XCreatePixmap(this->display, window, GUI_WIDTH, GUI_HEIGHT, 24);

    memset(bmps, 0, sizeof(bmps));
    bmps[BMP_CHARS]   = LoadImage(BMP_PATH"chars.bmp");
    bmps[BMP_KNOB]    = LoadImage(BMP_PATH"knob.bmp");
    bmps[BMP_KNOB2]   = LoadImage(BMP_PATH"knob2.bmp");
    bmps[BMP_KNOB3]   = LoadImage(BMP_PATH"knob3.bmp");
    bmps[BMP_KEY]     = LoadImage(BMP_PATH"key.bmp");
    bmps[BMP_BG]      = LoadImage(BMP_PATH"bg.bmp");
    bmps[BMP_BUTTONS] = LoadImage(BMP_PATH"buttons.bmp");
    bmps[BMP_OPS]     = LoadImage(BMP_PATH"ops.bmp");

    threadFinished = true;
}

CXlibToolkit::~CXlibToolkit()
{
    XClientMessageEvent event;
    event.display      = display;
    event.window       = window;
    event.type         = ClientMessage;
    event.format       = 8;
    event.data.l[0]    = WM_DELETE_WINDOW;
    event.message_type = WM_DELETE_WINDOW;
    XSendEvent(display, window, false, 0L, (XEvent*)&event);
    XFlush(display);
    while (!threadFinished)
    {
        usleep(1000 * 1);
    }
    XFreeGC(display, gc);
    XDestroyWindow(display, window);
    XSync(display, false);
    XFreePixmap(display, offscreen);
    for (int i = 0; i < BMP_COUNT; i++)
    {
        if (bmps[i])
        {
            XFreePixmap(display, bmps[i]);
        }
    }
    XCloseDisplay(display);
}

void CXlibToolkit::StartWindowProcesses()
{
    threadFinished = false;
    pthread_t thread1;
    pthread_create(&thread1, NULL, &eventProc,  (void*)this);
    pthread_t thread2;
    pthread_create(&thread2, NULL, &updateProc, (void*)this);
}

Pixmap CXlibToolkit::LoadImage(const char *path)
{
    FILE *f = fopen(path, "rb");
    if (!f)
    {
        return 0;
    }
    BITMAPHEADER header = {0};
    if (!fread(&header, sizeof(header), 1, f))
    {
        fclose(f);
        return 0;
    }
    if (header.fh.signature[0] != 'B' || header.fh.signature[1] != 'M')
    {
        fclose(f);
        return 0;
    }
    
    char *data = (char*)malloc(header.v5.width * header.v5.height * 4);
    char *tmp  = (char*)malloc(header.v5.imageSize);

    fseek(f, header.fh.fileOffsetToPixelArray, SEEK_SET);
    if (!fread(tmp, header.v5.imageSize, 1, f))
    {
        free(tmp);
        fclose(f);
        return 0;
    }
    fclose(f);
    
    char* dest = data;
    for (int line = header.v5.height - 1; line >= 0; line--)
    {
        char* src  = tmp + (line * (header.v5.imageSize / header.v5.height));
        int i = header.v5.width;
        while (i--)
        {
            *(dest++) = *(src++);
            *(dest++) = *(src++);
            *(dest++) = *(src++);
            *(dest++) = 0;
        }
    }
    free(tmp);
    
    XImage *image = XCreateImage(this->display, CopyFromParent, header.v5.bitsPerPixel, ZPixmap, 0, data, header.v5.width, header.v5.height, 32, 0);
    if (!image)
    {
        free(data);
    }
    Pixmap pixmap = XCreatePixmap(this->display, window, header.v5.width, header.v5.height, 24);
    GC gc = XCreateGC(display, pixmap, 0, 0);
    XPutImage(display, pixmap, gc, image, 0, 0, 0, 0, header.v5.width, header.v5.height);
    XFreeGC(display, gc);
    XDestroyImage(image);
    return pixmap;
}

void CXlibToolkit::CopyRect(int destX, int destY, int width, int height, int origBmp, int origX, int origY)
{
    if (!bmps[origBmp])
    {
        return;
    }
    XCopyArea(display, bmps[origBmp], window   , gc, origX, origY, width, height, destX, destY);
    XCopyArea(display, bmps[origBmp], offscreen, gc, origX, origY, width, height, destX, destY);
}

void CXlibToolkit::OutputDebugString(char *text)
{
    printf("%s\n", text);
}

int CXlibToolkit::WaitWindowClosed()
{
    while (!threadFinished)
    {
        usleep(1000 * 100);
    }
    return 0;
}
