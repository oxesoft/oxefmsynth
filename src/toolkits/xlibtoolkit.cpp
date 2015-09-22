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
#include <stdio.h>
#include "editor.h"
#include "xlibtoolkit.h"

#define EDITOR_ATOM "_editor"
#define CUSTOM_ATOM "_custom"

void eventProc()
{
    XEvent        event;
    Atom          type;
    int           format  = 0;
    unsigned long items   = 0;
    unsigned long bytes   = 0;
    unsigned char *data   = 0;
    CEditor       *editor = 0;
    while (true)
    {
        XNextEvent(event.xany.display, &event);
        printf("---event received\n");
        XGetWindowProperty(
            event.xany.display,
            event.xany.window,
            XInternAtom(event.xany.display, EDITOR_ATOM, false),
            0,
            1,
            false,
            AnyPropertyType,
            &type,
            &format,
            &items,
            &bytes,
            &data
        );
        editor = (CEditor*)*(long*)data;
        if (items != 1)
        {
            continue;
        }
        switch (event.type) 
        {
            case ButtonPress: 
                break;
            case ButtonRelease:
                break;
            case Expose:
                break;
            case KeyPress: 
                break;
            case ClientMessage:
            {
                XClientMessageEvent *message = (XClientMessageEvent *)&event;
                if (message->message_type == XInternAtom(event.xany.display, CUSTOM_ATOM, false))
                {
                    unsigned int messageID = message->data.l[0];
                    unsigned int par1      = message->data.l[1];
                    unsigned int par2      = message->data.l[2];
                    if (messageID == KILL_EDITOR)
                    {
                        return;
                    }
                }
            }
        }
    }
}

CXlibToolkit::CXlibToolkit(Display *display, Window parentWindow, CEditor *editor)
{
    this->display = display;
    window = XCreateSimpleWindow(display, parentWindow, 0, 0, GUI_WIDTH, GUI_HEIGHT, 0, 0, 0);
    
    void* data = editor;
    Atom atom = XInternAtom(display, EDITOR_ATOM, false);
    XChangeProperty(display, window, atom, atom, 32, PropModeReplace, (unsigned char*)&data, 1);

    XGCValues gcvalues = {0};
    gc = XCreateGC(display, window, 0, &gcvalues);
    XSelectInput(display, window, ButtonPressMask | ButtonReleaseMask | ExposureMask | KeyPressMask);
    XMapWindow(display, window);

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
    event.message_type = XInternAtom(display, CUSTOM_ATOM, false);
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

int CXlibToolkit::CommonWindowProc(XEvent *e)
{
    return -1;
}
