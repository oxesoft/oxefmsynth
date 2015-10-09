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

#import "constants.h"
#import "cocoawrapper.h"
#import <Cocoa/Cocoa.h>

@interface PluginView : NSView
{
    void* toolkit;
}
- (id)   initWithFrame:(NSRect)frame:(void*)toolkitPtr;
- (void) mouseDown:(NSEvent *)event;
@end

@interface CocoaToolkit : NSObject
{
    void* toolkit;
    NSAutoreleasePool* pool;
    NSApplication* app;
    NSWindow* window;
    PluginView* view;
}
- (id)   init:(void*)toolkitPtr;
- (void) createWindow:(id)parent;
- (void) showWindow;
- (void) waitWindowClosed;
@end

//----------------------------------------------------------------------

@implementation PluginView

- (id)initWithFrame:(NSRect)frame:(void*)toolkitPtr
{
    self = [super initWithFrame:frame];
    if (self)
    {
        toolkit = toolkitPtr;
    }
    return self;
}

- (void) mouseDown:(NSEvent *)event
{
    NSLog(@"mouseDown");
}

@end

//----------------------------------------------------------------------

@implementation CocoaToolkit

/**
  * wrappers start
**/
void* CocoaToolkitCreate(void* toolkit)
{
    return [[CocoaToolkit alloc] init:toolkit];
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
/**
  * wrappers end
**/

- (id) init:(void*)toolkitPtr
{
    self = [super init];
    if (self)
    {
        toolkit = toolkitPtr;
    }
    return self;
}

- (void) createWindow:(id)parent
{
    int width  = 0;
    int height = 0;
    CppGetDimension(toolkit, &width, &height);
    NSRect rect = NSMakeRect(0, 0, width, height);
    view = [[PluginView alloc] initWithFrame:rect :toolkit
    ];
    if (parent)
    {
        NSView* parentView = [(NSView*) parent retain];
        [[parentView window] setAcceptsMouseMovedEvents: YES];
        [parentView addSubview: view];
    }
    else
    {
        pool = [[NSAutoreleasePool alloc] init];
        app = [NSApplication sharedApplication];
        window = [[NSWindow alloc]
            initWithContentRect: rect
            styleMask: NSClosableWindowMask | NSTitledWindowMask
            backing: NSBackingStoreBuffered
            defer:NO
        ];
        [window setTitle:@TITLE_FULL];
        [window center];
        [window setAcceptsMouseMovedEvents:YES];
        [window setAutodisplay: YES];
        [window setContentView: view];
        [NSApp setDelegate:self];
    }
}

- (void) showWindow
{
    [window makeKeyAndOrderFront:nil];
}

- (void) waitWindowClosed
{
    [app run];
    [pool release];
}

- (BOOL) applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication
{
    return YES;
}

@end
