/*
Oxe FM Synth: a software synthesizer
Copyright (C) 2004-2026  Daniel Moura <oxe@oxesoft.com>

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

#import <Cocoa/Cocoa.h>
#include <blend2d/blend2d.h>
#include "editor.h"
#include "cocoatoolkit.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

@interface PluginView : NSView
{
    CCocoaToolkit* toolkit;
    BLImage        blImage;
}
- (id)   initWithToolkit:(CCocoaToolkit*)toolkitPtr size:(NSSize)size;
- (void) clearToolkit;
- (void) viewDidMoveToWindow;
- (void) mouseDown:(NSEvent *)event;
- (void) mouseUp:(NSEvent *)event;
- (void) mouseMoved:(NSEvent *)event;
- (void) mouseDragged:(NSEvent *)event;
- (void) scrollWheel:(NSEvent *)event;
- (void) keyDown:(NSEvent *)event;
- (BOOL) acceptsFirstResponder;
- (BOOL) isOpaque;
- (NSMenu *) menuForEvent:(NSEvent *)event;
- (void) scale100:(id)sender;
- (void) scale150:(id)sender;
- (void) scale200:(id)sender;
@end

@interface CocoaWindowController : NSObject <NSApplicationDelegate, NSWindowDelegate>
{
    CCocoaToolkit*     toolkit;
    NSView*            parentView;
    NSAutoreleasePool* pool;
    NSApplication*     app;
    NSWindow*          window;
    PluginView*        view;
    NSTimer*           timer;
}
- (id)   initWithToolkit:(CCocoaToolkit*)toolkitPtr view:(PluginView*)viewPtr parent:(id)parent;
- (void) showWindow;
- (void) closeWindow;
- (void) waitWindowClosed;
- (void) update;
- (void) invalidate;
- (void) invalidateRect:(NSRect)rect;
- (void) resizeToWidth:(int)w height:(int)h;
- (void) scale100:(id)sender;
- (void) scale150:(id)sender;
- (void) scale200:(id)sender;
@end

struct CCocoaToolkitImpl
{
    CocoaWindowController *controller;
    PluginView            *view;
};

//----------------------------------------------------------------------

@implementation PluginView

- (id) initWithToolkit:(CCocoaToolkit*)toolkitPtr size:(NSSize)size
{
    NSRect frame = NSMakeRect(0, 0, size.width, size.height);
    self = [super initWithFrame:frame];
    if (self)
    {
        toolkit = toolkitPtr;
        [self setWantsLayer:YES];
        [self setLayerContentsRedrawPolicy:NSViewLayerContentsRedrawOnSetNeedsDisplay];
    }
    return self;
}

- (void) clearToolkit
{
    toolkit = nullptr;
}

- (void) dealloc
{
    blImage.reset();
    [super dealloc];
}

- (BOOL) wantsUpdateLayer
{
    return YES;
}

- (void) updateLayer
{
    if (!toolkit || !toolkit->editor)
    {
        return;
    }

    NSRect bounds = [self bounds];
    CGFloat backingScale = [self window] ? [[self window] backingScaleFactor] : 1.0;
    if (backingScale < 1.0) backingScale = 1.0;

    int pixelW = (int)ceil(bounds.size.width * backingScale);
    int pixelH = (int)ceil(bounds.size.height * backingScale);
    if (pixelW <= 0 || pixelH <= 0) return;

    if (blImage.width() != pixelW || blImage.height() != pixelH)
    {
        blImage.create(pixelW, pixelH, BL_FORMAT_PRGB32);
    }

    BLContext ctx(blImage);
    ctx.clear_all();

    double scaleX = (double)pixelW / (double)GUI_WIDTH;
    double scaleY = (double)pixelH / (double)GUI_HEIGHT;
    ctx.scale(scaleX, scaleY);

    toolkit->editor->Paint(ctx);
    ctx.end();

    BLImageData imgData;
    blImage.get_data(&imgData);

    CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, imgData.pixel_data, imgData.stride * pixelH, NULL);
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CGImageRef cgImage = CGImageCreate(
        pixelW,
        pixelH,
        8,
        32,
        imgData.stride,
        colorSpace,
        kCGImageAlphaPremultipliedFirst | kCGBitmapByteOrder32Little,
        provider,
        NULL,
        false,
        kCGRenderingIntentDefault
    );

    self.layer.contents = (__bridge id)cgImage;
    self.layer.contentsScale = backingScale;

    CGImageRelease(cgImage);
    CGColorSpaceRelease(colorSpace);
    CGDataProviderRelease(provider);
}

- (void) viewDidMoveToWindow
{
    [self addTrackingRect:[self bounds] owner:self userData:NULL assumeInside:NO];
}

- (BOOL) isOpaque
{
    return YES;
}

- (BOOL) acceptsFirstResponder
{
    return YES;
}

- (void) mouseEntered:(NSEvent *)theEvent
{
    if (!toolkit) return;
    [[self window] setAcceptsMouseMovedEvents:YES];
    [[self window] makeFirstResponder:self];
}

- (void) mouseDown:(NSEvent *)event
{
    if (!toolkit || !toolkit->editor) return;
    NSPoint loc = [self convertPoint:[event locationInWindow] fromView:nil];
    NSRect bounds = [self bounds];
    float sx = bounds.size.width / (float)GUI_WIDTH;
    float sy = bounds.size.height / (float)GUI_HEIGHT;
    int x = (int)(loc.x / sx);
    int y = GUI_HEIGHT - (int)(loc.y / sy);
    toolkit->editor->OnLButtonDown(x, y);
}

- (void) mouseUp:(NSEvent *)event
{
    if (!toolkit || !toolkit->editor) return;
    toolkit->editor->OnLButtonUp();
    if ([event clickCount] == 2)
    {
        NSPoint loc = [self convertPoint:[event locationInWindow] fromView:nil];
        NSRect bounds = [self bounds];
        float sx = bounds.size.width / (float)GUI_WIDTH;
        float sy = bounds.size.height / (float)GUI_HEIGHT;
        int x = (int)(loc.x / sx);
        int y = GUI_HEIGHT - (int)(loc.y / sy);
        toolkit->editor->OnLButtonDblClick(x, y);
    }
}

- (void) mouseMoved:(NSEvent *)event
{
    if (!toolkit || !toolkit->editor) return;
    NSPoint loc = [self convertPoint:[event locationInWindow] fromView:nil];
    NSRect bounds = [self bounds];
    float sx = bounds.size.width / (float)GUI_WIDTH;
    float sy = bounds.size.height / (float)GUI_HEIGHT;
    int x = (int)(loc.x / sx);
    int y = GUI_HEIGHT - (int)(loc.y / sy);
    toolkit->editor->OnMouseMove(x, y);
}

- (void) mouseDragged:(NSEvent *)event
{
    if (!toolkit || !toolkit->editor) return;
    NSPoint loc = [self convertPoint:[event locationInWindow] fromView:nil];
    NSRect bounds = [self bounds];
    float sx = bounds.size.width / (float)GUI_WIDTH;
    float sy = bounds.size.height / (float)GUI_HEIGHT;
    int x = (int)(loc.x / sx);
    int y = GUI_HEIGHT - (int)(loc.y / sy);
    toolkit->editor->OnMouseMove(x, y);
}

- (void) scrollWheel:(NSEvent *)event
{
    if (!toolkit || !toolkit->editor) return;
    NSPoint loc = [self convertPoint:[event locationInWindow] fromView:nil];
    NSRect bounds = [self bounds];
    float sx = bounds.size.width / (float)GUI_WIDTH;
    float sy = bounds.size.height / (float)GUI_HEIGHT;
    int x = (int)(loc.x / sx);
    int y = GUI_HEIGHT - (int)(loc.y / sy);
    CGFloat delta = [event deltaY];
    if (delta != 0.0)
    {
        toolkit->editor->OnMouseWheel(x, y, delta > 0.0 ? 1 : -1);
    }
}

- (void) keyDown:(NSEvent *)event
{
    if (!toolkit || !toolkit->editor) return;
    const char *c = [[event characters] UTF8String];
    if (c && c[0])
    {
        toolkit->editor->OnChar((int)c[0]);
    }
}

- (NSMenu *) menuForEvent:(NSEvent *)event
{
    if ([event type] == NSEventTypeRightMouseDown)
    {
        NSMenu *menu = [[[NSMenu alloc] initWithTitle:@"Zoom"] autorelease];
        NSMenuItem *i100 = [menu addItemWithTitle:@"Scale 100% (950 × 656)" action:@selector(scale100:) keyEquivalent:@"1"];
        [i100 setTarget:self];
        NSMenuItem *i150 = [menu addItemWithTitle:@"Scale 150% (1425 × 984)" action:@selector(scale150:) keyEquivalent:@"2"];
        [i150 setTarget:self];
        NSMenuItem *i200 = [menu addItemWithTitle:@"Scale 200% (1900 × 1312)" action:@selector(scale200:) keyEquivalent:@"3"];
        [i200 setTarget:self];
        return menu;
    }
    return [super menuForEvent:event];
}

- (void) scale100:(id)sender
{
    if (toolkit) toolkit->Resize(GUI_WIDTH, GUI_HEIGHT);
}

- (void) scale150:(id)sender
{
    if (toolkit) toolkit->Resize((int)(GUI_WIDTH * 1.5), (int)(GUI_HEIGHT * 1.5));
}

- (void) scale200:(id)sender
{
    if (toolkit) toolkit->Resize(GUI_WIDTH * 2, GUI_HEIGHT * 2);
}

@end

//----------------------------------------------------------------------

@implementation CocoaWindowController

- (id) initWithToolkit:(CCocoaToolkit*)toolkitPtr view:(PluginView*)viewPtr parent:(id)parent
{
    self = [super init];
    if (self)
    {
        toolkit = toolkitPtr;
        view = [viewPtr retain];
        if (!parent)
        {
            pool = [[NSAutoreleasePool alloc] init];
            app = [NSApplication sharedApplication];

            NSRect rect = NSMakeRect(0, 0, GUI_WIDTH, GUI_HEIGHT);
            window = [[NSWindow alloc]
                initWithContentRect: rect
                styleMask: NSWindowStyleMaskClosable | NSWindowStyleMaskTitled | NSWindowStyleMaskResizable
                backing: NSBackingStoreBuffered
                defer: NO
            ];
            [window setContentAspectRatio:NSMakeSize(GUI_WIDTH, GUI_HEIGHT)];
            [window setMinSize:NSMakeSize(475, 328)];
            [window setTitle:@TITLE_FULL];
            [window center];
            [window setContentView: view];
            [window setDelegate:self];
            [NSApp setDelegate:self];

            NSMenu *mainMenu = [[[NSMenu alloc] init] autorelease];
            NSMenuItem *appMenuItem = [[[NSMenuItem alloc] init] autorelease];
            [mainMenu addItem:appMenuItem];
            NSMenu *appMenu = [[[NSMenu alloc] initWithTitle:@"App"] autorelease];
            [appMenu addItemWithTitle:@"Quit Oxe FM Synth" action:@selector(terminate:) keyEquivalent:@"q"];
            [appMenuItem setSubmenu:appMenu];

            NSMenuItem *viewMenuItem = [[[NSMenuItem alloc] init] autorelease];
            [mainMenu addItem:viewMenuItem];
            NSMenu *viewMenu = [[[NSMenu alloc] initWithTitle:@"View"] autorelease];
            NSMenuItem *mi100 = [viewMenu addItemWithTitle:@"Scale 100% (950 × 656)" action:@selector(scale100:) keyEquivalent:@"1"];
            [mi100 setTarget:self];
            NSMenuItem *mi150 = [viewMenu addItemWithTitle:@"Scale 150% (1425 × 984)" action:@selector(scale150:) keyEquivalent:@"2"];
            [mi150 setTarget:self];
            NSMenuItem *mi200 = [viewMenu addItemWithTitle:@"Scale 200% (1900 × 1312)" action:@selector(scale200:) keyEquivalent:@"3"];
            [mi200 setTarget:self];
            [viewMenuItem setSubmenu:viewMenu];

            [NSApp setMainMenu:mainMenu];
        }
        else
        {
            parentView = [(NSView*)parent retain];
            [parentView addSubview: view];
        }
    }
    return self;
}

- (void) windowDidResize:(NSNotification *)notification
{
    if (window && view)
    {
        NSRect contentRect = [[window contentView] bounds];
        [view setFrame:contentRect];
        [view setNeedsDisplay:YES];
    }
}

- (void) closeWindow
{
    if (timer)
    {
        [timer invalidate];
        timer = nil;
    }
    toolkit = nullptr;
    if (view)
    {
        [view clearToolkit];
        [view removeFromSuperview];
    }
    if (window)
    {
        [window close];
    }
    if (parentView)
    {
        [parentView release];
        parentView = nil;
    }
}

- (void) dealloc
{
    [self closeWindow];
    if (view)
    {
        [view release];
        view = nil;
    }
    if (window)
    {
        [window release];
        window = nil;
    }
    if (pool)
    {
        [pool release];
        pool = nil;
    }
    [super dealloc];
}

- (void) showWindow
{
    if (window)
    {
        [window makeKeyAndOrderFront:nil];
    }
    else
    {
        [[view window] orderFront:nil];
    }
    [[view window] makeKeyAndOrderFront:nil];
    if (!timer)
    {
        timer = [NSTimer scheduledTimerWithTimeInterval:(0.001 * TIMER_RESOLUTION_MS)
                                                 target:self
                                               selector:@selector(update)
                                               userInfo:nil
                                                repeats:YES];
    }
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
    if (toolkit && toolkit->editor)
    {
        toolkit->editor->Update();
    }
}

- (void) invalidate
{
    [view setNeedsDisplay:YES];
}

- (void) invalidateRect:(NSRect)rect
{
    [self invalidate];
}

- (void) resizeToWidth:(int)w height:(int)h
{
    if (window)
    {
        [window setContentSize:NSMakeSize(w, h)];
    }
    if (view)
    {
        [view setFrame:NSMakeRect(0, 0, w, h)];
        [view setNeedsDisplay:YES];
    }
}

- (void) scale100:(id)sender
{
    [self resizeToWidth:GUI_WIDTH height:GUI_HEIGHT];
}

- (void) scale150:(id)sender
{
    [self resizeToWidth:(int)(GUI_WIDTH * 1.5) height:(int)(GUI_HEIGHT * 1.5)];
}

- (void) scale200:(id)sender
{
    [self resizeToWidth:(GUI_WIDTH * 2) height:(GUI_HEIGHT * 2)];
}

@end

//----------------------------------------------------------------------

CCocoaToolkit::CCocoaToolkit(void *parentWindow, CEditor *editor)
{
    this->parentWindow  = parentWindow;
    this->editor        = editor;

    CCocoaToolkitImpl *pImpl = new CCocoaToolkitImpl();
    this->impl = pImpl;

    pImpl->view = [[PluginView alloc] initWithToolkit:this size:NSMakeSize(GUI_WIDTH, GUI_HEIGHT)];
    pImpl->controller = [[CocoaWindowController alloc] initWithToolkit:this view:pImpl->view parent:(id)parentWindow];
}

CCocoaToolkit::~CCocoaToolkit()
{
    this->editor = nullptr;
    CCocoaToolkitImpl *pImpl = (CCocoaToolkitImpl*)this->impl;
    if (pImpl)
    {
        if (pImpl->controller)
        {
            [pImpl->controller closeWindow];
            [pImpl->controller release];
            pImpl->controller = nil;
        }
        if (pImpl->view)
        {
            [pImpl->view release];
            pImpl->view = nil;
        }
        delete pImpl;
        this->impl = NULL;
    }
}

void CCocoaToolkit::Invalidate()
{
    CCocoaToolkitImpl *pImpl = (CCocoaToolkitImpl*)this->impl;
    if (pImpl && pImpl->controller)
    {
        [pImpl->controller invalidate];
    }
}

void CCocoaToolkit::InvalidateRect(int x, int y, int width, int height)
{
    Invalidate();
}

void CCocoaToolkit::CopyRect(int destX, int destY, int width, int height, int origBmp, int origX, int origY)
{
    InvalidateRect(destX, destY, width, height);
}

void CCocoaToolkit::StartMouseCapture()
{
}

void CCocoaToolkit::StopMouseCapture()
{
}

void CCocoaToolkit::StartWindowProcesses()
{
    CCocoaToolkitImpl *pImpl = (CCocoaToolkitImpl*)this->impl;
    if (pImpl && pImpl->controller)
    {
        [pImpl->controller showWindow];
    }
}

int CCocoaToolkit::WaitWindowClosed()
{
    CCocoaToolkitImpl *pImpl = (CCocoaToolkitImpl*)this->impl;
    if (pImpl && pImpl->controller)
    {
        [pImpl->controller waitWindowClosed];
    }
    return 0;
}

float CCocoaToolkit::GetScale()
{
    CCocoaToolkitImpl *pImpl = (CCocoaToolkitImpl*)this->impl;
    if (pImpl && pImpl->view)
    {
        NSRect bounds = [pImpl->view bounds];
        return bounds.size.width / (float)GUI_WIDTH;
    }
    return 1.0f;
}

void CCocoaToolkit::Resize(int width, int height)
{
    CCocoaToolkitImpl *pImpl = (CCocoaToolkitImpl*)this->impl;
    if (pImpl && pImpl->controller)
    {
        [pImpl->controller resizeToWidth:width height:height];
    }
}
