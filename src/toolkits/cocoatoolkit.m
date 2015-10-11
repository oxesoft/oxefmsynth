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

#import "bitmaps.h"
#import "constants.h"
#import "cocoawrapper.h"
#import <Cocoa/Cocoa.h>

@interface PluginView : NSImageView
{
    void* toolkit;
}
- (id)   init:(void*)toolkitPtr withSize:(NSSize)size;
- (void) mouseDown:(NSEvent *)event;
- (void) mouseUp:(NSEvent *)event;
- (void) mouseMoved:(NSEvent *)event;
- (void) mouseDragged:(NSEvent *)event;
- (BOOL) isOpaque;
@end

@interface CocoaToolkit : NSObject
{
    void* toolkit;
    NSAutoreleasePool* pool;
    NSApplication* app;
    NSWindow* window;
    PluginView* view;
    NSImage* bmps[BMP_COUNT];
    int bmps_height[BMP_COUNT];
    NSTimer* timer;
}
- (id)   init:(void*)toolkitPtr;
- (void) createWindow:(id)parent;
- (void) showWindow;
- (void) copyRectFromImageIndex:(int)index to:(NSPoint)point from:(NSRect)rect;
- (void) waitWindowClosed;
- (void) update;
@end

//----------------------------------------------------------------------

@implementation PluginView

- (id)init:(void*)toolkitPtr withSize:(NSSize)size
{
    self = [super init];
    if (self)
    {
        toolkit = toolkitPtr;
        [self setImage:[[NSImage alloc] initWithSize:size]];
    }
    return self;
}

- (BOOL) isOpaque
{
    return YES;
}

- (void) mouseDown:(NSEvent *)event
{
    NSPoint loc = [event locationInWindow];
    CppOnLButtonDown(toolkit, (int)loc.x, GUI_HEIGHT - (int)loc.y);
}

- (void) mouseUp:(NSEvent *)event
{
    CppOnLButtonUp(toolkit);
}

- (void) mouseMoved:(NSEvent *)event
{
    NSPoint loc = [event locationInWindow];
    CppOnMouseMove(toolkit, (int)loc.x, GUI_HEIGHT - (int)loc.y);
}

- (void) mouseDragged:(NSEvent *)event;
{
    NSPoint loc = [event locationInWindow];
    CppOnMouseMove(toolkit, (int)loc.x, GUI_HEIGHT - (int)loc.y);
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

void CocoaToolkitCopyRect(void *self, int destX, int destY, int width, int height, int origBmp, int origX, int origY)
{
    [(id)self copyRectFromImageIndex:origBmp to:NSMakePoint(destX, destY) from:NSMakeRect(origX, origY, width, height)];
}
/**
  * wrappers end
**/

- (id) init:(void*)toolkitPtr
{
    self = [super init];
    if (self)
    {
        pool = [[NSAutoreleasePool alloc] init];
        toolkit = toolkitPtr;
        bmps[BMP_CHARS  ] = [[NSImage alloc] initByReferencingFile:@BMP_PATH"/chars.bmp"  ];
        bmps[BMP_KNOB   ] = [[NSImage alloc] initByReferencingFile:@BMP_PATH"/knob.bmp"   ];
        bmps[BMP_KNOB2  ] = [[NSImage alloc] initByReferencingFile:@BMP_PATH"/knob2.bmp"  ];
        bmps[BMP_KNOB3  ] = [[NSImage alloc] initByReferencingFile:@BMP_PATH"/knob3.bmp"  ];
        bmps[BMP_KEY    ] = [[NSImage alloc] initByReferencingFile:@BMP_PATH"/key.bmp"    ];
        bmps[BMP_BG     ] = [[NSImage alloc] initByReferencingFile:@BMP_PATH"/bg.bmp"     ];
        bmps[BMP_BUTTONS] = [[NSImage alloc] initByReferencingFile:@BMP_PATH"/buttons.bmp"];
        bmps[BMP_OPS    ] = [[NSImage alloc] initByReferencingFile:@BMP_PATH"/ops.bmp"    ];
        if (![bmps[BMP_CHARS  ] isValid]) bmps[BMP_CHARS  ] = [[NSImage alloc] initWithData:[NSData dataWithBytes:(void*)chars_bmp   length:sizeof(chars_bmp)  ]];
        if (![bmps[BMP_KNOB   ] isValid]) bmps[BMP_KNOB   ] = [[NSImage alloc] initWithData:[NSData dataWithBytes:(void*)knob_bmp    length:sizeof(knob_bmp)   ]];
        if (![bmps[BMP_KNOB2  ] isValid]) bmps[BMP_KNOB2  ] = [[NSImage alloc] initWithData:[NSData dataWithBytes:(void*)knob2_bmp   length:sizeof(knob2_bmp)  ]];
        if (![bmps[BMP_KNOB3  ] isValid]) bmps[BMP_KNOB3  ] = [[NSImage alloc] initWithData:[NSData dataWithBytes:(void*)knob3_bmp   length:sizeof(knob3_bmp)  ]];
        if (![bmps[BMP_KEY    ] isValid]) bmps[BMP_KEY    ] = [[NSImage alloc] initWithData:[NSData dataWithBytes:(void*)key_bmp     length:sizeof(key_bmp)    ]];
        if (![bmps[BMP_BG     ] isValid]) bmps[BMP_BG     ] = [[NSImage alloc] initWithData:[NSData dataWithBytes:(void*)bg_bmp      length:sizeof(bg_bmp)     ]];
        if (![bmps[BMP_BUTTONS] isValid]) bmps[BMP_BUTTONS] = [[NSImage alloc] initWithData:[NSData dataWithBytes:(void*)buttons_bmp length:sizeof(buttons_bmp)]];
        if (![bmps[BMP_OPS    ] isValid]) bmps[BMP_OPS    ] = [[NSImage alloc] initWithData:[NSData dataWithBytes:(void*)ops_bmp     length:sizeof(ops_bmp)    ]];
        int i;
        for (i = 0; i < BMP_COUNT; i++)
        {
            NSImageRep *rep = [[bmps[i] representations] objectAtIndex:0];
            bmps_height[i] = rep.pixelsHigh;
        }
    }
    return self;
}

-(void)dealloc {
    [timer invalidate];
    [pool release];
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
        NSView* parentView = [(NSView*) parent retain];
        [[parentView window] setAcceptsMouseMovedEvents: YES];
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
        [window setAcceptsMouseMovedEvents:YES];
        [window setAutodisplay: YES];
        [window setContentView: view];
        [NSApp setDelegate:(id)self];
    }
}

- (void) showWindow
{
    [window makeKeyAndOrderFront:nil];
    timer = [NSTimer scheduledTimerWithTimeInterval:(0.001 * TIMER_RESOLUTION_MS)
                                             target:self
                                           selector:@selector(update)
                                           userInfo:nil
                                            repeats:YES];
}

- (void) copyRectFromImageIndex:(int)index to:(NSPoint)point from:(NSRect)rect
{
    rect.origin.y  = bmps_height[index] - rect.origin.y - rect.size.height;
    point.y        = GUI_HEIGHT         - point.y       - rect.size.height;
    NSImage* image = [view image];
    NSRect dest    = NSMakeRect(point.x, point.y, rect.size.width, rect.size.height);
    [image lockFocus];
    [bmps[index] drawAtPoint:point fromRect:rect operation:NSCompositeCopy fraction:1.0];
    [image unlockFocus];
    [view setNeedsDisplayInRect: dest];
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
    CppUpdate(toolkit);
}

@end
