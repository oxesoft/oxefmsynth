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

@interface PluginView : NSImageView
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
    self = [super initWithFrame:NSMakeRect(0, 0, size.width, size.height)];
    if (self)
    {
        toolkit = toolkitPtr;
        NSBitmapImageRep *bitmap = [[NSBitmapImageRep alloc]
            initWithBitmapDataPlanes:NULL
            pixelsWide:size.width
            pixelsHigh:size.height
            bitsPerSample:8
            samplesPerPixel:3
            hasAlpha:NO
            isPlanar:NO
            colorSpaceName:NSDeviceRGBColorSpace
            bytesPerRow:size.width * 4
            bitsPerPixel:32
        ];
        NSImage *image = [[NSImage alloc] initWithSize:size];
        [image addRepresentation:bitmap];
        [self setImage:image];
    }
    return self;
}

- (void)viewDidMoveToWindow {
    [self addTrackingRect:NSMakeRect(0, 0, GUI_WIDTH, GUI_HEIGHT) owner:self userData:NULL assumeInside:NO];
}

- (BOOL) isOpaque
{
    return YES;
}

- (void)mouseEntered:(NSEvent *)theEvent
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

NSImage* LoadImageFromBuffer(const unsigned char *buffer)
{
    BITMAPHEADER *header = (BITMAPHEADER *)buffer;
    if (header->fh.signature[0] != 'B' || header->fh.signature[1] != 'M')
    {
        return 0;
    }
    if (!header->v5.imageSize)
    {
        header->v5.imageSize = header->fh.fileSize - sizeof(BITMAPFILEHEADER) - header->v5.dibHeaderSize;
    }
    unsigned int bytesPerRow = (header->v5.imageSize / header->v5.height);
    unsigned char *data = (unsigned char*)malloc(header->v5.width * header->v5.height * 4);
    unsigned char* dest = data;
    int line;
    unsigned char r;
    unsigned char g;
    unsigned char b;
    for (line = header->v5.height - 1; line >= 0; line--)
    {
        unsigned char* src = (unsigned char*)buffer + header->fh.fileOffsetToPixelArray + (line * bytesPerRow);
        unsigned int i = header->v5.width;
        while (i--)
        {
            b = *(src++);
            g = *(src++);
            r = *(src++);
            *(dest++) = r;
            *(dest++) = g;
            *(dest++) = b;
            *(dest++) = 0xFF;
        }
    }
    NSBitmapImageRep *bitmap = [[NSBitmapImageRep alloc]
        initWithBitmapDataPlanes:(unsigned char **)&data
        pixelsWide:header->v5.width
        pixelsHigh:header->v5.height
        bitsPerSample:8
        samplesPerPixel:3
        hasAlpha:NO
        isPlanar:NO
        colorSpaceName:NSDeviceRGBColorSpace
        bytesPerRow:header->v5.width * 4
        bitsPerPixel:32
    ];
    NSImage *image = [[NSImage alloc] initWithSize:NSMakeSize(header->v5.width, header->v5.height)];
    [image addRepresentation:bitmap];
    return image;
}

NSImage* LoadImageFromFile(const char *path)
{
    FILE *f = fopen(path, "rb");
    if (!f)
    {
        return 0;
    }
    fseek(f, 0, SEEK_END);
    int size = ftell(f);
    fseek(f, 0, SEEK_SET);
    unsigned char *tmp = (unsigned char*)malloc(size);
    if (!fread(tmp, size, 1, f))
    {
        free(tmp);
        fclose(f);
        return 0;
    }
    fclose(f);
    NSImage* result = LoadImageFromBuffer((const unsigned char*)tmp);
    free(tmp);
    return result;
}

- (id) init:(void*)toolkitPtr
{
    self = [super init];
    if (self)
    {
        pool = [[NSAutoreleasePool alloc] init];
        toolkit = toolkitPtr;
        bmps[BMP_CHARS  ] = LoadImageFromFile((const char*)BMP_PATH"/chars.bmp"  );
        bmps[BMP_KNOB   ] = LoadImageFromFile((const char*)BMP_PATH"/knob.bmp"   );
        bmps[BMP_KNOB2  ] = LoadImageFromFile((const char*)BMP_PATH"/knob2.bmp"  );
        bmps[BMP_KNOB3  ] = LoadImageFromFile((const char*)BMP_PATH"/knob3.bmp"  );
        bmps[BMP_KEY    ] = LoadImageFromFile((const char*)BMP_PATH"/key.bmp"    );
        bmps[BMP_BG     ] = LoadImageFromFile((const char*)BMP_PATH"/bg.bmp"     );
        bmps[BMP_BUTTONS] = LoadImageFromFile((const char*)BMP_PATH"/buttons.bmp");
        bmps[BMP_OPS    ] = LoadImageFromFile((const char*)BMP_PATH"/ops.bmp"    );
        if (!bmps[BMP_CHARS  ]) bmps[BMP_CHARS  ] = LoadImageFromBuffer((unsigned char*)chars_bmp   );
        if (!bmps[BMP_KNOB   ]) bmps[BMP_KNOB   ] = LoadImageFromBuffer((unsigned char*)knob_bmp    );
        if (!bmps[BMP_KNOB2  ]) bmps[BMP_KNOB2  ] = LoadImageFromBuffer((unsigned char*)knob2_bmp   );
        if (!bmps[BMP_KNOB3  ]) bmps[BMP_KNOB3  ] = LoadImageFromBuffer((unsigned char*)knob3_bmp   );
        if (!bmps[BMP_KEY    ]) bmps[BMP_KEY    ] = LoadImageFromBuffer((unsigned char*)key_bmp     );
        if (!bmps[BMP_BG     ]) bmps[BMP_BG     ] = LoadImageFromBuffer((unsigned char*)bg_bmp      );
        if (!bmps[BMP_BUTTONS]) bmps[BMP_BUTTONS] = LoadImageFromBuffer((unsigned char*)buttons_bmp );
        if (!bmps[BMP_OPS    ]) bmps[BMP_OPS    ] = LoadImageFromBuffer((unsigned char*)ops_bmp     );
        int i;
        for (i = 0; i < BMP_COUNT; i++)
        {
            NSImageRep *rep = [[bmps[i] representations] objectAtIndex:0];
            bmps_height[i] = rep.pixelsHigh;
            [rep release];
        }
    }
    return self;
}

-(void) dealloc {
    [timer invalidate];
    [view release];
    if (window)
    {
        [window release];
    }
    int i;
    for (i = 0; i < BMP_COUNT; i++)
    {
        [bmps[i] release];
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

- (void) copyRectFromImageIndex:(int)index to:(NSPoint)point from:(NSRect)rect
{
    rect.origin.y  = bmps_height[index] - rect.origin.y - rect.size.height;
    point.y        = GUI_HEIGHT         - point.y       - rect.size.height;
    NSImage* image = [view image];
    NSRect dest    = NSMakeRect(point.x, point.y, rect.size.width, rect.size.height);
    [image lockFocus];
    [bmps[index] drawInRect:dest fromRect:rect operation:NSCompositeCopy fraction:1.0];
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
