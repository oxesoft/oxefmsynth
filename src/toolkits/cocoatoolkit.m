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

@interface PluginView : NSOpenGLView
{
    void* toolkit;
}
- (id)   init:(void*)toolkitPtr withSize:(NSSize)size;
- (void) viewDidMoveToWindow;
- (void) mouseDown:(NSEvent *)event;
- (void) mouseUp:(NSEvent *)event;
- (void) mouseMoved:(NSEvent *)event;
- (void) mouseDragged:(NSEvent *)event;
- (void) keyDown:(NSEvent *)event;
- (BOOL) isOpaque;
@end

@interface CocoaToolkit : NSObject
{
    void* toolkit;
    NSView* parentView;
    NSAutoreleasePool* pool;
    NSApplication* app;
    NSWindow* window;
    PluginView* view;
    NSImage* bmps[BMP_COUNT];
    int bmps_height[BMP_COUNT];
    NSTimer* timer;
}
- (id)   initWithToolkit:(void*)toolkitPtr;
- (void) createWindow:(id)parent;
- (void) showWindow;
- (void) waitWindowClosed;
- (void) update;
@end

//----------------------------------------------------------------------

@implementation PluginView

- (id) init:(void*)toolkitPtr withSize:(NSSize)size
{
    toolkit = toolkitPtr;

    NSRect frame = NSMakeRect(0, 0, size.width, size.height);

    NSOpenGLPixelFormatAttribute pixelAttribs[16] =
    {
        NSOpenGLPFADoubleBuffer,
        NSOpenGLPFAAccelerated,
        NSOpenGLPFAColorSize, 32,
        NSOpenGLPFADepthSize, 32,
        0
    };

    NSOpenGLPixelFormat* pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:pixelAttribs];
    if (pixelFormat)
    {
        self = [super initWithFrame:frame pixelFormat:pixelFormat];
        [pixelFormat release];
    }
    else
    {
        self = [super initWithFrame:frame];
    }
    if (self)
    {
        [[self openGLContext] makeCurrentContext];
        CppOpenGLInit(toolkit);
    }
    return self;
}

- (void) dealloc
{
    CppOpenGLDeinit(toolkit);
    [super dealloc];
}

- (void) drawRect:(NSRect)rect
{
    CppOpenGLDraw(toolkit);
    [[self openGLContext] flushBuffer];
}

- (void)viewDidMoveToWindow {
    [self addTrackingRect:NSMakeRect(0, 0, GUI_WIDTH, GUI_HEIGHT) owner:self userData:NULL assumeInside:NO];
}

- (BOOL) isOpaque
{
    return YES;
}

- (void) mouseEntered:(NSEvent *)theEvent
{
    [[self window] setAcceptsMouseMovedEvents: YES];
    [[self window] makeFirstResponder:self];
}

- (void) mouseDown:(NSEvent *)event
{
    NSPoint loc = [self convertPoint:[event locationInWindow] fromView:nil];
    CppOnLButtonDown(toolkit, (int)loc.x, GUI_HEIGHT - (int)loc.y);
}

- (void) mouseUp:(NSEvent *)event
{
    CppOnLButtonUp(toolkit);
    if ([event clickCount] == 2)
    {
        NSPoint loc = [self convertPoint:[event locationInWindow] fromView:nil];
        CppOnDblClick(toolkit, (int)loc.x, GUI_HEIGHT - (int)loc.y);
    }
}

- (void) mouseMoved:(NSEvent *)event
{
    NSPoint loc = [self convertPoint:[event locationInWindow] fromView:nil];
    CppOnMouseMove(toolkit, (int)loc.x, GUI_HEIGHT - (int)loc.y);
}

- (void) mouseDragged:(NSEvent *)event;
{
    NSPoint loc = [self convertPoint:[event locationInWindow] fromView:nil];
    CppOnMouseMove(toolkit, (int)loc.x, GUI_HEIGHT - (int)loc.y);
}

- (void) keyDown:(NSEvent *)event
{
    const char *c = [[event characters] UTF8String];
    CppOnChar(toolkit, (int)c[0]);
}

@end

//----------------------------------------------------------------------

@implementation CocoaToolkit

/**
  * wrappers start
**/
void* CocoaToolkitCreate(void* toolkit)
{
    return [[CocoaToolkit alloc] initWithToolkit:toolkit];
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

- (id) initWithToolkit:(void*)toolkitPtr
{
    self = [super init];
    if (self)
    {
        pool = [[NSAutoreleasePool alloc] init];
        toolkit = toolkitPtr;
    }
    return self;
}

- (void) dealloc
{
    [timer invalidate];
    [view release];
    if (window)
    {
        [window release];
    }
    if (pool)
    {
        [pool release];
    }
    if (parentView)
    {
        [parentView release];
    }
    [super dealloc];
}

- (void) createWindow:(id)parent
{
    if (!parent)
    {
        app = [NSApplication sharedApplication];
    }
    view = [[PluginView alloc] init:toolkit withSize:NSMakeSize(GUI_WIDTH, GUI_HEIGHT)];
    if (parent)
    {
        [pool release];
        pool = NULL;
        parentView = [(NSView*) parent retain];
        [parentView addSubview: view];
    }
    else
    {
        NSRect rect = NSMakeRect(0, 0, GUI_WIDTH, GUI_HEIGHT);
        window = [[NSWindow alloc]
            initWithContentRect: rect
            styleMask: NSClosableWindowMask | NSTitledWindowMask
            backing: NSBackingStoreBuffered
            defer:NO
        ];
        [window setTitle:@TITLE_FULL];
        [window center];
        [window setContentView: view];
        [NSApp setDelegate:(id)self];
    }
}

- (void) showWindow
{
    [[view window] setAutodisplay: YES];
    if (window)
    {
        [window makeKeyAndOrderFront:nil];
    }
    else
    {
        [[view window] orderFront:nil];
    }
    [[view window] makeKeyAndOrderFront:nil];
    timer = [NSTimer scheduledTimerWithTimeInterval:(0.001 * TIMER_RESOLUTION_MS)
                                             target:self
                                           selector:@selector(update)
                                           userInfo:nil
                                            repeats:YES];
}

- (void) waitWindowClosed
{
    [app run];
}

- (BOOL) applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication
{
    return YES;
}

- (void) update
{
    [view setNeedsDisplay: YES];
}

@end
