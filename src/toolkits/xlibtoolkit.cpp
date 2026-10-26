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

static void GetViewport(int cw, int ch, int &vpX, int &vpY, int &vpW, int &vpH, double &scale)
{
    if (cw <= 0 || ch <= 0)
    {
        vpX = 0;
        vpY = 0;
        vpW = GUI_WIDTH;
        vpH = GUI_HEIGHT;
        scale = 1.0;
        return;
    }
    scale = fmin((double)cw / (double)GUI_WIDTH, (double)ch / (double)GUI_HEIGHT);
    vpW = (int)round(GUI_WIDTH * scale);
    vpH = (int)round(GUI_HEIGHT * scale);
    if (vpW < 1) vpW = 1;
    if (vpH < 1) vpH = 1;
    vpX = (cw - vpW) / 2;
    vpY = (ch - vpH) / 2;
}

static void GetMouseScaled(CXlibToolkit *toolkit, int inX, int inY, int &outX, int &outY)
{
    XWindowAttributes wa;
    XGetWindowAttributes(toolkit->display, toolkit->window, &wa);
    int vpX, vpY, vpW, vpH;
    double scale;
    GetViewport(wa.width, wa.height, vpX, vpY, vpW, vpH, scale);
    outX = (int)floor((inX - vpX) / scale);
    outY = (int)floor((inY - vpY) / scale);
}

static void SendClientAtom(Display *display, Window window, Atom atom)
{
    XClientMessageEvent event;
    event.display      = display;
    event.window       = window;
    event.type         = ClientMessage;
    event.format       = 8;
    event.data.l[0]    = atom;
    event.message_type = atom;
    XSendEvent(display, window, false, 0L, (XEvent*)&event);
    XFlush(display);
}

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
                int mx, my;
                GetMouseScaled(toolkit, e->x, e->y, mx, my);

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
                int mx, my;
                GetMouseScaled(toolkit, e->x, e->y, mx, my);
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
        SendClientAtom(toolkit->display, toolkit->window, toolkit->WM_TIMER);
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

    Window parent = parentWindow ? (Window)parentWindow : RootWindow(display, screen);
    int winX = parentWindow ? 0 : 100;
    int winY = parentWindow ? 0 : 100;

    this->window = XCreateWindow(
        display,
        parent,
        winX, winY,
        GUI_WIDTH, GUI_HEIGHT,
        0,
        CopyFromParent,
        InputOutput,
        CopyFromParent,
        CWBackPixmap | CWEventMask,
        &attr
    );

    if (!parentWindow)
    {
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
        SendClientAtom(display, window, WM_DELETE_WINDOW);
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

    int vpX, vpY, vpW, vpH;
    double scale;
    GetViewport(winW, winH, vpX, vpY, vpW, vpH, scale);

    bool isFull = (w <= 0 || h <= 0);

    if (blImage.width() != vpW || blImage.height() != vpH)
    {
        blImage.create(vpW, vpH, BL_FORMAT_PRGB32);
        isFull = true;
    }

    if (vpX > 0 || vpY > 0)
    {
        XSetForeground(display, gc, 0x0b0e14);
        if (vpY > 0)
        {
            XFillRectangle(display, window, gc, 0, 0, winW, vpY);
            XFillRectangle(display, window, gc, 0, vpY + vpH, winW, winH - (vpY + vpH));
        }
        if (vpX > 0)
        {
            XFillRectangle(display, window, gc, 0, vpY, vpX, vpH);
            XFillRectangle(display, window, gc, vpX + vpW, vpY, winW - (vpX + vpW), vpH);
        }
    }

    int dirtyX = 0, dirtyY = 0, dirtyW = GUI_WIDTH, dirtyH = GUI_HEIGHT;
    if (!isFull)
    {
        dirtyX = (int)floor((x - vpX) / scale);
        dirtyY = (int)floor((y - vpY) / scale);
        dirtyW = (int)ceil(w / scale) + 1;
        dirtyH = (int)ceil(h / scale) + 1;
        if (dirtyX < 0) dirtyX = 0;
        if (dirtyY < 0) dirtyY = 0;
        if (dirtyX + dirtyW > GUI_WIDTH) dirtyW = GUI_WIDTH - dirtyX;
        if (dirtyY + dirtyH > GUI_HEIGHT) dirtyH = GUI_HEIGHT - dirtyY;
        if (dirtyW <= 0 || dirtyH <= 0) return;
    }

    BLContext ctx(blImage);
    ctx.scale(scale, scale);
    editor->Paint(ctx, dirtyX, dirtyY, dirtyW, dirtyH);
    ctx.end();

    BLImageData imgData;
    blImage.get_data(&imgData);

    Visual *visual = DefaultVisual(display, DefaultScreen(display));
    int depth = DefaultDepth(display, DefaultScreen(display));
    XImage *ximage = XCreateImage(display, visual, depth, ZPixmap, 0, (char*)imgData.pixel_data, vpW, vpH, 32, imgData.stride);
    if (ximage)
    {
        if (isFull)
        {
            XPutImage(display, window, gc, ximage, 0, 0, vpX, vpY, vpW, vpH);
        }
        else
        {
            int bltX = (int)floor(dirtyX * scale);
            int bltY = (int)floor(dirtyY * scale);
            int bltW = (int)ceil(dirtyW * scale) + 1;
            int bltH = (int)ceil(dirtyH * scale) + 1;
            if (bltX + bltW > vpW) bltW = vpW - bltX;
            if (bltY + bltH > vpH) bltH = vpH - bltY;
            if (bltW > 0 && bltH > 0)
            {
                XPutImage(display, window, gc, ximage, bltX, bltY, vpX + bltX, vpY + bltY, bltW, bltH);
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
    int vpX, vpY, vpW, vpH;
    double scale;
    GetViewport(wa.width, wa.height, vpX, vpY, vpW, vpH, scale);
    int vx = vpX + (int)floor((x - 1) * scale);
    int vy = vpY + (int)floor((y - 1) * scale);
    int vw = (int)ceil((width + 2) * scale);
    int vh = (int)ceil((height + 2) * scale);
    if (vx < 0) { vw += vx; vx = 0; }
    if (vy < 0) { vh += vy; vy = 0; }
    XClearArea(display, window, vx, vy, vw, vh, True);
    XFlush(display);
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
    int vpX, vpY, vpW, vpH;
    double scale;
    GetViewport(wa.width, wa.height, vpX, vpY, vpW, vpH, scale);
    return (float)scale;
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
