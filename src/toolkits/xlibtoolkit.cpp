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

#include "bitmaps.h"
#include "editor.h"
#include <X11/Xlib.h>
#include <GL/glx.h>
#include "opengltoolkit.h"
#include "xlibtoolkit.h"
#include <X11/Xutil.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dlfcn.h>
#include <limits.h>

typedef struct __attribute__((packed))
{
    char         signature[2];
    unsigned int fileSize;
    short        reserved[2];
    unsigned int fileOffsetToPixelArray;
} BITMAPFILEHEADER;

typedef struct __attribute__((packed))
{
    unsigned int   dibHeaderSize;
    unsigned int   width;
    unsigned int   height;
    unsigned short planes;
    unsigned short bitsPerPixel;
    unsigned int   compression;
    unsigned int   imageSize;
} BITMAPV5HEADER;

typedef struct
{
    BITMAPFILEHEADER fh;
    BITMAPV5HEADER   v5;
} BITMAPHEADER;

void* eventProc(void* ptr)
{
    XEvent event;
    CXlibToolkit *toolkit = (CXlibToolkit*)ptr;
    bool stopThread = false;
    unsigned int time = 0;
    if (toolkit->openGLmode)
    {
        glXMakeCurrent(toolkit->display, toolkit->window, toolkit->glxContext);
    }
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
            case KeyPress:
            {
                char buffer[64];
                int count = XLookupString((XKeyEvent*)&event, buffer, sizeof(buffer), NULL, NULL);
                if (buffer[0])
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
                if (toolkit->openGLmode)
                {
                    toolkit->Draw();
                    glXSwapBuffers(toolkit->display, toolkit->window);
                }
                else
                {
                    XGraphicsExposeEvent *e = (XGraphicsExposeEvent*)&event;
                    XCopyArea(toolkit->display, toolkit->offscreen, toolkit->window, toolkit->gc, e->x, e->y, e->width, e->height, e->x, e->y);
                }
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
                    if (toolkit->openGLmode)
                    {
                        toolkit->Draw();
                        glXSwapBuffers(toolkit->display, toolkit->window);
                    }
                    else
                    {
                        toolkit->editor->Update();
                    }
                }
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

void GetResourcesPath(char *path, int size)
{
    Dl_info info;
    dladdr((void*)GetResourcesPath, &info);
    strncpy(path, info.dli_fname, PATH_MAX);
    char* tmp = strrchr(path, '/');
    *tmp = 0;
    strcat(path, "/"BMP_PATH"/");
}

CXlibToolkit::CXlibToolkit(void *parentWindow, CEditor *editor)
{
    this->parentWindow  = parentWindow;
    this->editor        = editor;
    this->display       = NULL;
    this->window        = 0;
    this->offscreen     = 0;
    this->gc            = NULL;
    this->glxContext    = NULL;
    thread1Finished     = true;
    thread2Finished     = true;
    memset(bmps, 0, sizeof(bmps));

    char *displayName = getenv("DISPLAY");
    if (!displayName || !strlen(displayName))
    {
        displayName = (char*)":0.0";
    }
    if (!XInitThreads())
    {
        fprintf(stderr, "Xlib threads support unavailable\n");
        return;
    }
    this->display = XOpenDisplay(displayName);
    if (!this->display)
    {
        printf("Cannot open display\n");
        return;
    }
    int screen = DefaultScreen(this->display);

    bool isStandAlone = false;
    if (!parentWindow)
    {
        isStandAlone = true;
        parentWindow = (void*)RootWindow(this->display, screen);
    }

    XVisualInfo vinfo = {0};
    openGLmode = glXQueryExtension(this->display, NULL, NULL);

    if (openGLmode)
    {
        XVisualInfo *vi = NULL;
        int attrs[] =
        {
            GLX_RGBA,
            GLX_DOUBLEBUFFER,
            None
        };
        vi = glXChooseVisual(this->display, screen, attrs);
        if (vi)
        {
            vinfo = *vi;
            this->glxContext = glXCreateContext(this->display, vi, 0, GL_TRUE);
            XFree(vi);
        }
    }

    int depth = DefaultDepth(display, screen);

    if (isStandAlone)
    {
        printf("default depth=%d\n", depth);
    }

    if (!this->glxContext)
    {
        openGLmode = false;
        if (!XMatchVisualInfo(this->display, screen, depth, TrueColor, &vinfo))
        {
            fprintf(stderr, "No visual available\n");
            return;
        }
    }

    if (isStandAlone)
    {
        if (openGLmode)
        {
            printf("%s\n", "OpenGL mode");
        }
        else
        {
            printf("%s\n", "blitting mode");
        }
        printf("visual info values:\n");
        printf("    depth       : %d\n", vinfo.depth);
        printf("    red_mask    : %08X\n", (int)vinfo.red_mask);
        printf("    green_mask  : %08X\n", (int)vinfo.green_mask);
        printf("    blue_mask   : %08X\n", (int)vinfo.blue_mask);
    }

    XSetWindowAttributes wattrs;
    wattrs.colormap         = XCreateColormap(this->display, (Window)parentWindow, vinfo.visual, AllocNone);
    wattrs.event_mask       = ButtonPressMask | ButtonReleaseMask | PointerMotionMask | ExposureMask | KeyPressMask;
    window = XCreateWindow(this->display, (Window)parentWindow, 0, 0, GUI_WIDTH, GUI_HEIGHT, 0, vinfo.depth, InputOutput, vinfo.visual, CWColormap | CWEventMask, &wattrs);

    XStoreName(this->display, this->window, TITLE_FULL);

    XSizeHints sizeHints;
    memset(&sizeHints, 0, sizeof(sizeHints));
    sizeHints.flags      = PMinSize | PMaxSize;
    sizeHints.min_width  = GUI_WIDTH;
    sizeHints.min_height = GUI_HEIGHT;
    sizeHints.max_width  = GUI_WIDTH;
    sizeHints.max_height = GUI_HEIGHT;
    XSetNormalHints(this->display, this->window, &sizeHints);

    this->WM_TIMER         = XInternAtom(this->display, "WM_TIMER"        , false);
    this->WM_DELETE_WINDOW = XInternAtom(this->display, "WM_DELETE_WINDOW", false);
    XSetWMProtocols(this->display, this->window, &WM_DELETE_WINDOW, 1);

    char path[PATH_MAX];
    GetResourcesPath(path, PATH_MAX);

    if (openGLmode)
    {
        glXMakeCurrent(this->display, this->window, this->glxContext);
        Init(this->editor, path);
    }
    else
    {
        gc = XCreateGC(this->display, this->window, 0, 0);
        offscreen = XCreatePixmap(this->display, this->window, GUI_WIDTH, GUI_HEIGHT, vinfo.depth);

        char fullPath[PATH_MAX];
        snprintf(fullPath, PATH_MAX, "%s/%s", path, "chars.bmp"  );
        bmps[BMP_CHARS  ] = LoadImageFromFile(fullPath, &vinfo);
        snprintf(fullPath, PATH_MAX, "%s/%s", path, "knob.bmp"   );
        bmps[BMP_KNOB   ] = LoadImageFromFile(fullPath, &vinfo);
        snprintf(fullPath, PATH_MAX, "%s/%s", path, "knob2.bmp"  );
        bmps[BMP_KNOB2  ] = LoadImageFromFile(fullPath, &vinfo);
        snprintf(fullPath, PATH_MAX, "%s/%s", path, "knob3.bmp"  );
        bmps[BMP_KNOB3  ] = LoadImageFromFile(fullPath, &vinfo);
        snprintf(fullPath, PATH_MAX, "%s/%s", path, "key.bmp"    );
        bmps[BMP_KEY    ] = LoadImageFromFile(fullPath, &vinfo);
        snprintf(fullPath, PATH_MAX, "%s/%s", path, "bg.bmp"     );
        bmps[BMP_BG     ] = LoadImageFromFile(fullPath, &vinfo);
        snprintf(fullPath, PATH_MAX, "%s/%s", path, "buttons.bmp");
        bmps[BMP_BUTTONS] = LoadImageFromFile(fullPath, &vinfo);
        snprintf(fullPath, PATH_MAX, "%s/%s", path, "ops.bmp"    );
        bmps[BMP_OPS    ] = LoadImageFromFile(fullPath, &vinfo);

        if (!bmps[BMP_CHARS  ]) bmps[BMP_CHARS  ] = LoadImageFromBuffer(chars_bmp  , &vinfo);
        if (!bmps[BMP_KNOB   ]) bmps[BMP_KNOB   ] = LoadImageFromBuffer(knob_bmp   , &vinfo);
        if (!bmps[BMP_KNOB2  ]) bmps[BMP_KNOB2  ] = LoadImageFromBuffer(knob2_bmp  , &vinfo);
        if (!bmps[BMP_KNOB3  ]) bmps[BMP_KNOB3  ] = LoadImageFromBuffer(knob3_bmp  , &vinfo);
        if (!bmps[BMP_KEY    ]) bmps[BMP_KEY    ] = LoadImageFromBuffer(key_bmp    , &vinfo);
        if (!bmps[BMP_BG     ]) bmps[BMP_BG     ] = LoadImageFromBuffer(bg_bmp     , &vinfo);
        if (!bmps[BMP_BUTTONS]) bmps[BMP_BUTTONS] = LoadImageFromBuffer(buttons_bmp, &vinfo);
        if (!bmps[BMP_OPS    ]) bmps[BMP_OPS    ] = LoadImageFromBuffer(ops_bmp    , &vinfo);
    }

    XMapWindow(this->display, this->window);
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
    while (!thread1Finished || !thread2Finished)
    {
        usleep(1000 * 1);
    }
    if (openGLmode)
    {
        Deinit();
        glXDestroyContext(this->display, glxContext);
    }
    else
    {
        if (gc)
        {
            XFreeGC(display, gc);
        }
        if (offscreen)
        {
            XFreePixmap(display, offscreen);
        }
        for (int i = 0; i < BMP_COUNT; i++)
        {
            if (bmps[i])
            {
                XFreePixmap(display, bmps[i]);
            }
        }
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

Pixmap CXlibToolkit::LoadImageFromFile(const char *path, XVisualInfo *v)
{
    FILE *f = fopen(path, "rb");
    if (!f)
    {
        return 0;
    }
    fseek(f, 0, SEEK_END);
    int size = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *tmp = (char*)malloc(size);
    if (!fread(tmp, size, 1, f))
    {
        free(tmp);
        fclose(f);
        return 0;
    }
    fclose(f);
    Pixmap result = LoadImageFromBuffer(tmp, v);
    free(tmp);
    return result;
}

Pixmap CXlibToolkit::LoadImageFromBuffer(const char *buffer, XVisualInfo *v)
{
    BITMAPHEADER *header = (BITMAPHEADER *)buffer;
    if (header->fh.signature[0] != 'B' || header->fh.signature[1] != 'M')
    {
        return 0;
    }
    char  *data = (char*)malloc(header->v5.width * header->v5.height * 4);
    char  *dest      = data;
    short *destInt16 = (short*)data;
    if (!header->v5.imageSize)
    {
        header->v5.imageSize = header->fh.fileSize - sizeof(BITMAPFILEHEADER) - header->v5.dibHeaderSize;
    }
    int rMask = v->red_mask;
    int gMask = v->green_mask;
    int bMask = v->blue_mask;
    int rBitCount = __builtin_popcount(rMask);
    int gBitCount = __builtin_popcount(gMask);
    int bBitCount = __builtin_popcount(bMask);
    int r2Right = 8 - rBitCount;
    int g2Right = 8 - gBitCount;
    int b2Right = 8 - bBitCount;
    int r2Left  = 16 - rBitCount;
    int g2Left  = 16 - gBitCount;
    int b2Left  = 16 - bBitCount;
	while (!(rMask & 1))
	{
		rMask >>= 1;
		r2Left--;
	}
	while (!(gMask & 1))
	{
		gMask >>= 1;
		g2Left--;
	}
	while (!(bMask & 1))
	{
		bMask >>= 1;
		b2Left--;
	}
    int r;
    int g;
    int b;
    int i;
    int bitmapPad = v->depth == 16 ? 16 : 32;
    for (int line = header->v5.height - 1; line >= 0; line--)
    {
        char* src  = (char*)buffer + header->fh.fileOffsetToPixelArray + (line * (header->v5.imageSize / header->v5.height));
        i = header->v5.width;
        if (v->depth == 24 || v->depth == 32)
        {
			while (i--)
			{
				r = *(src++);
				g = *(src++);
				b = *(src++);
				*(dest++) = r;
				*(dest++) = g;
				*(dest++) = b;
				*(dest++) = 0xFF;
			}
		}
		else if (v->depth == 16)
		{
			while (i--)
			{
				r = *(src++);
				g = *(src++);
				b = *(src++);
				*(destInt16++) = ((r >> r2Right) << r2Left) | ((g >> g2Right) << g2Left) | ((b >> b2Right) << b2Left);
			}
		}
    }
    XImage *image = XCreateImage(this->display, CopyFromParent, v->depth, ZPixmap, 0, data, header->v5.width, header->v5.height, bitmapPad, 0);
    if (!image)
    {
        free(data);
        return 0;
    }
    Pixmap pixmap = XCreatePixmap(this->display, window, header->v5.width, header->v5.height, v->depth);
    GC gc = XCreateGC(display, pixmap, 0, 0);
    XPutImage(display, pixmap, gc, image, 0, 0, 0, 0, header->v5.width, header->v5.height);
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

int CXlibToolkit::WaitWindowClosed()
{
    while (!thread1Finished || !thread2Finished)
    {
        usleep(1000 * 100);
    }
    return 0;
}
