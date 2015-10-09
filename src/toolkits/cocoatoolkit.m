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
#import "cocoawrapper.h"
#import <Cocoa/Cocoa.h>

@interface CocoaToolkit : NSObject
{
    NSAutoreleasePool* pool;
    NSApplication* app;
    NSWindow* window;
}
- (void) createWindow:(id)parent;
- (void) showWindow;
- (void) waitWindowClosed;
@end

@implementation CocoaToolkit

void* CocoaToolkitCreate()
{
    return [[CocoaToolkit alloc] init];
}

void CocoaToolkitDestroy(void *self)
{
    [(id)self dealloc];
}

void CocoaToolkitCreateWindow(void *self, void* parent)
{
    [(id)self createWindow:(id)parent];
}

void CocoaToolkitShowWindow(void *self)
{
    [(id)self showWindow];
}

void CocoaToolkitWaitWindowClosed(void *self)
{
    [(id)self waitWindowClosed];
}

- (void) createWindow:(id)parent
{
    if (parent)
    {
        window = parent;
    }
    else
    {
        pool = [[NSAutoreleasePool alloc] init];
        app = [NSApplication sharedApplication];
        window = [[NSWindow alloc]
            initWithContentRect: NSMakeRect(0, 0, GUI_WIDTH, GUI_HEIGHT)
            styleMask: NSClosableWindowMask | NSTitledWindowMask
            backing: NSBackingStoreBuffered
            defer:NO
        ];
        [window setTitle:@"title test"];
        [window center];
        [window setAcceptsMouseMovedEvents:YES];
        [window makeKeyAndOrderFront:nil];
    }
}

- (void) showWindow
{
    [window setIsVisible:YES];
}

- (void) waitWindowClosed
{
    [app run];
    [pool release];
}

@end
