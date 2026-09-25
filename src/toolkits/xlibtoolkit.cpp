/*
Oxe FM Synth: a software synthesizer
Copyright (C) 2004-2026  Daniel Moura <oxesoft@gmail.com>

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
#include "xlibtoolkit.h"
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

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
                XWindowAttributes wa;
                XGetWindowAttributes(toolkit->display, toolkit->window, &wa);
                float sx = wa.width > 0 ? (float)wa.width / (float)GUI_WIDTH : 1.0f;
                float sy = wa.height > 0 ? (float)wa.height / (float)GUI_HEIGHT : 1.0f;
                int mx = (int)(e->x / sx);
                int my = (int)(e->y / sy);

                switch (e->button)
                {
                case 1:
                    if (e->time - time > 400)
                    {
                        toolkit->editor->OnLButtonDown(mx, my);
                    }
                    else
                    {
                        toolkit->editor->OnLButtonDblClick(mx, my);
                    }
                    time = e->time;
                    break;
                case 4:
                    toolkit->editor->OnMouseWheel(mx, my,  1);
                    break;
                case 5:
                    toolkit->editor->OnMouseWheel(mx, my, -1);
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
                XWindowAttributes wa;
                XGetWindowAttributes(toolkit->display, toolkit->window, &wa);
                float sx = wa.width > 0 ? (float)wa.width / (float)GUI_WIDTH : 1.0f;
                float sy = wa.height > 0 ? (float)wa.height / (float)GUI_HEIGHT : 1.0f;
                int mx = (int)(e->x / sx);
                int my = (int)(e->y / sy);
                toolkit->editor->OnMouseMove(mx, my);
                break;
            }
            case KeyPress:
            {
                char buffer[64];
                int count = XLookupString((XKeyEvent*)&event, buffer, sizeof(buffer), NULL, NULL);
                if (count > 0 && buffer[0])
                {
                    toolkit->editor->OnChar(buffer[0]);
                }
                break;
            }
            case MappingNotify:
            {
                XRefreshKeyboardMapping((XMappingEvent*)&event);
                break;
            }
            case Expose:
            {
                XExposeEvent *e = (XExposeEvent*)&event;
                toolkit->Draw(e->x, e->y, e->width, e->height);
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
                break;
            }
        }
    }
    toolkit->thread1Finished = true;
    return NULL;
}

void* updateProc(void* ptr)
{
    CXlibToolkit *toolkit = (CXlibToolkit*)ptr;
    while (!toolkit->thread1Finished)
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
    toolkit->thread2Finished = true;
    return NULL;
}

CXlibToolkit::CXlibToolkit(void *parentWindow, CEditor *editor)
{
    this->parentWindow    = parentWindow;
    this->editor          = editor;
    this->thread1Finished = true;
    this->thread2Finished = true;
    this->display         = XOpenDisplay(NULL);

    if (!display)
    {
        return;
    }

    int screen = DefaultScreen(display);
    XSetWindowAttributes attr;
    attr.background_pixmap = None;
    attr.event_mask = ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask;

    if (!parentWindow)
    {
        this->window = XCreateWindow(
            display,
            RootWindow(display, screen),
            100, 100,
            GUI_WIDTH, GUI_HEIGHT,
            0,
            CopyFromParent,
            InputOutput,
            CopyFromParent,
            CWBackPixmap | CWEventMask,
            &attr
        );

        XStoreName(display, window, "Oxe FM Synth");

        // Lock aspect ratio for standalone window
        XSizeHints hints;
        hints.flags = PAspect | PMinSize;
        hints.min_width = GUI_WIDTH / 2;
        hints.min_height = GUI_HEIGHT / 2;
        hints.min_aspect.x = GUI_WIDTH;
        hints.min_aspect.y = GUI_HEIGHT;
        hints.max_aspect.x = GUI_WIDTH;
        hints.max_aspect.y = GUI_HEIGHT;
        XSetWMNormalHints(display, window, &hints);
    }
    else
    {
        this->window = XCreateWindow(
            display,
            (Window)parentWindow,
            0, 0,
            GUI_WIDTH, GUI_HEIGHT,
            0,
            CopyFromParent,
            InputOutput,
            CopyFromParent,
            CWBackPixmap | CWEventMask,
            &attr
        );
    }

    WM_DELETE_WINDOW = XInternAtom(display, "WM_DELETE_WINDOW", false);
    WM_TIMER         = XInternAtom(display, "WM_TIMER",         false);
    XSetWMProtocols(display, window, &WM_DELETE_WINDOW, 1);

    this->gc = XCreateGC(display, window, 0, 0);

    XMapWindow(this->display, this->window);
}

CXlibToolkit::~CXlibToolkit()
{
    if (display && window)
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
        while (!thread1Finished || !thread2Finished)
        {
            usleep(1000 * 1);
        }
    }
    if (gc)
    {
        XFreeGC(display, gc);
    }
    if (window)
    {
        XDestroyWindow(display, window);
    }
    if (display)
    {
        XSync(display, false);
        XCloseDisplay(display);
    }
    blImage.reset();
}

void CXlibToolkit::StartWindowProcesses()
{
    if (!this->window)
    {
        return;
    }
    thread1Finished = false;
    thread2Finished = false;
    pthread_t thread1;
    pthread_create(&thread1, NULL, &eventProc,  (void*)this);
    pthread_t thread2;
    pthread_create(&thread2, NULL, &updateProc, (void*)this);
}

void CXlibToolkit::Draw(int x, int y, int w, int h)
{
    if (!editor || !display || !window) return;
    XWindowAttributes wa;
    XGetWindowAttributes(display, window, &wa);
    int winW = wa.width;
    int winH = wa.height;
    if (winW <= 0 || winH <= 0) return;

    bool isFull = (w <= 0 || h <= 0);

    if (blImage.width() != winW || blImage.height() != winH)
    {
        blImage.create(winW, winH, BL_FORMAT_PRGB32);
        isFull = true;
    }

    double sx = (double)winW / (double)GUI_WIDTH;
    double sy = (double)winH / (double)GUI_HEIGHT;

    int dirtyX = 0, dirtyY = 0, dirtyW = GUI_WIDTH, dirtyH = GUI_HEIGHT;
    if (!isFull)
    {
        dirtyX = (int)floor(x / sx);
        dirtyY = (int)floor(y / sy);
        dirtyW = (int)ceil(w / sx) + 1;
        dirtyH = (int)ceil(h / sy) + 1;
        if (dirtyX < 0) dirtyX = 0;
        if (dirtyY < 0) dirtyY = 0;
        if (dirtyX + dirtyW > GUI_WIDTH) dirtyW = GUI_WIDTH - dirtyX;
        if (dirtyY + dirtyH > GUI_HEIGHT) dirtyH = GUI_HEIGHT - dirtyY;
    }

    BLContext ctx(blImage);
    ctx.scale(sx, sy);
    editor->Paint(ctx, dirtyX, dirtyY, dirtyW, dirtyH);
    ctx.end();

    BLImageData imgData;
    blImage.get_data(&imgData);

    Visual *visual = DefaultVisual(display, DefaultScreen(display));
    int depth = DefaultDepth(display, DefaultScreen(display));
    XImage *ximage = XCreateImage(display, visual, depth, ZPixmap, 0, (char*)imgData.pixel_data, winW, winH, 32, imgData.stride);
    if (ximage)
    {
        if (isFull)
        {
            XPutImage(display, window, gc, ximage, 0, 0, 0, 0, winW, winH);
        }
        else
        {
            int bltX = (int)floor(dirtyX * sx);
            int bltY = (int)floor(dirtyY * sy);
            int bltW = (int)ceil(dirtyW * sx) + 1;
            int bltH = (int)ceil(dirtyH * sy) + 1;
            if (bltX + bltW > winW) bltW = winW - bltX;
            if (bltY + bltH > winH) bltH = winH - bltY;
            if (bltW > 0 && bltH > 0)
            {
                XPutImage(display, window, gc, ximage, bltX, bltY, bltX, bltY, bltW, bltH);
            }
        }
        ximage->data = NULL; // prevent XDestroyImage from freeing Blend2D memory
        XDestroyImage(ximage);
    }
}

void CXlibToolkit::Invalidate()
{
    if (!display || !window) return;
    XClearArea(display, window, 0, 0, 0, 0, True);
    XFlush(display);
}

void CXlibToolkit::InvalidateRect(int x, int y, int width, int height)
{
    if (!display || !window) return;
    XWindowAttributes wa;
    XGetWindowAttributes(display, window, &wa);
    double sx = wa.width > 0 ? (double)wa.width / (double)GUI_WIDTH : 1.0;
    double sy = wa.height > 0 ? (double)wa.height / (double)GUI_HEIGHT : 1.0;
    int vx = (int)floor((x - 1) * sx);
    int vy = (int)floor((y - 1) * sy);
    int vw = (int)ceil((width + 2) * sx);
    int vh = (int)ceil((height + 2) * sy);
    if (vx < 0) { vw += vx; vx = 0; }
    if (vy < 0) { vh += vy; vy = 0; }
    XClearArea(display, window, vx, vy, vw, vh, True);
    XFlush(display);
}

void CXlibToolkit::CopyRect(int destX, int destY, int width, int height, int origBmp, int origX, int origY)
{
    InvalidateRect(destX, destY, width, height);
}

void CXlibToolkit::StartMouseCapture()
{
}

void CXlibToolkit::StopMouseCapture()
{
}

float CXlibToolkit::GetScale()
{
    if (!display || !window) return 1.0f;
    XWindowAttributes wa;
    XGetWindowAttributes(display, window, &wa);
    return wa.width > 0 ? (float)wa.width / (float)GUI_WIDTH : 1.0f;
}

void CXlibToolkit::Resize(int width, int height)
{
    if (!display || !window) return;
    XResizeWindow(display, window, width, height);
    Invalidate();
}

int CXlibToolkit::WaitWindowClosed()
{
    while (!thread1Finished || !thread2Finished)
    {
        usleep(1000 * 100);
    }
    return 0;
}
