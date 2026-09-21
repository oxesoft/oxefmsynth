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
#include "editor.h"
#include "cocoatoolkit.h"
#include "bitmaps.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef __USE_GNU
#define __USE_GNU
#endif
#include <dlfcn.h>
#ifndef PATH_MAX
#define PATH_MAX 512
#endif

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

static void GetResourcesPath(char *path, int size)
{
    Dl_info info;
    dladdr((void*)GetResourcesPath, &info);
    strncpy(path, info.dli_fname, size);
    char* tmp = strrchr(path, '/');
    if (tmp) *tmp = 0;
    strncat(path, "/../../../" BMP_PATH "/", size - strlen(path) - 1);
}

struct OxeBitmap
{
    int width;
    int height;
    uint32_t *pixels;
};

@interface PluginView : NSView
{
    CCocoaToolkit*  toolkit;
    CGContextRef    bitmapContext;
    CGColorSpaceRef colorSpace;
}
- (id)   initWithToolkit:(CCocoaToolkit*)toolkitPtr pixels:(uint32_t*)pixels size:(NSSize)size;
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
@end

@interface CocoaWindowController : NSObject <NSApplicationDelegate>
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
- (void) invalidateRect:(NSRect)rect;
@end

struct CCocoaToolkitImpl
{
    CocoaWindowController *controller;
    PluginView            *view;
    OxeBitmap             bmps[BMP_COUNT];
    uint32_t              *screenPixels;
};

//----------------------------------------------------------------------

@implementation PluginView

- (id) initWithToolkit:(CCocoaToolkit*)toolkitPtr pixels:(uint32_t*)pixels size:(NSSize)size
{
    NSRect frame = NSMakeRect(0, 0, size.width, size.height);
    self = [super initWithFrame:frame];
    if (self)
    {
        toolkit = toolkitPtr;
        [self setWantsLayer:YES];
        [self setLayerContentsRedrawPolicy:NSViewLayerContentsRedrawOnSetNeedsDisplay];

        colorSpace = CGColorSpaceCreateDeviceRGB();
        bitmapContext = CGBitmapContextCreate(
            pixels,
            GUI_WIDTH,
            GUI_HEIGHT,
            8,
            GUI_WIDTH * sizeof(uint32_t),
            colorSpace,
            kCGImageAlphaNoneSkipLast | kCGBitmapByteOrder32Big
        );
    }
    return self;
}

- (void) clearToolkit
{
    toolkit = nullptr;
}

- (void) dealloc
{
    if (bitmapContext)
    {
        CGContextRelease(bitmapContext);
        bitmapContext = NULL;
    }
    if (colorSpace)
    {
        CGColorSpaceRelease(colorSpace);
        colorSpace = NULL;
    }
    [super dealloc];
}

- (void) drawRect:(NSRect)dirtyRect
{
    if (!toolkit || !bitmapContext)
    {
        return;
    }
    CGImageRef image = CGBitmapContextCreateImage(bitmapContext);
    if (image)
    {
        CGContextRef context = [[NSGraphicsContext currentContext] CGContext];
        CGContextSetInterpolationQuality(context, kCGInterpolationNone);
        CGContextDrawImage(context, CGRectMake(0, 0, GUI_WIDTH, GUI_HEIGHT), image);
        CGImageRelease(image);
    }
}

- (void) viewDidMoveToWindow
{
    [self addTrackingRect:NSMakeRect(0, 0, GUI_WIDTH, GUI_HEIGHT) owner:self userData:NULL assumeInside:NO];
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
    toolkit->editor->OnLButtonDown((int)loc.x, GUI_HEIGHT - (int)loc.y);
}

- (void) mouseUp:(NSEvent *)event
{
    if (!toolkit || !toolkit->editor) return;
    toolkit->editor->OnLButtonUp();
    if ([event clickCount] == 2)
    {
        NSPoint loc = [self convertPoint:[event locationInWindow] fromView:nil];
        toolkit->editor->OnLButtonDblClick((int)loc.x, GUI_HEIGHT - (int)loc.y);
    }
}

- (void) mouseMoved:(NSEvent *)event
{
    if (!toolkit || !toolkit->editor) return;
    NSPoint loc = [self convertPoint:[event locationInWindow] fromView:nil];
    toolkit->editor->OnMouseMove((int)loc.x, GUI_HEIGHT - (int)loc.y);
}

- (void) mouseDragged:(NSEvent *)event
{
    if (!toolkit || !toolkit->editor) return;
    NSPoint loc = [self convertPoint:[event locationInWindow] fromView:nil];
    toolkit->editor->OnMouseMove((int)loc.x, GUI_HEIGHT - (int)loc.y);
}

- (void) scrollWheel:(NSEvent *)event
{
    if (!toolkit || !toolkit->editor) return;
    NSPoint loc = [self convertPoint:[event locationInWindow] fromView:nil];
    CGFloat delta = [event deltaY];
    if (delta != 0.0)
    {
        toolkit->editor->OnMouseWheel((int)loc.x, GUI_HEIGHT - (int)loc.y, delta > 0.0 ? 1 : -1);
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
                styleMask: NSWindowStyleMaskClosable | NSWindowStyleMaskTitled
                backing: NSBackingStoreBuffered
                defer: NO
            ];
            [window setTitle:@TITLE_FULL];
            [window center];
            [window setContentView: view];
            [NSApp setDelegate:self];
        }
        else
        {
            parentView = [(NSView*)parent retain];
            [parentView addSubview: view];
        }
    }
    return self;
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

- (void) invalidateRect:(NSRect)rect
{
    [view setNeedsDisplayInRect:rect];
}

@end

//----------------------------------------------------------------------

static bool LoadBitmap(OxeBitmap *bmp, const unsigned char *buffer, const char *skinPath, const char *filename)
{
    unsigned char *fileBuf = NULL;
    if (skinPath && skinPath[0])
    {
        char fullPath[PATH_MAX];
        snprintf(fullPath, sizeof(fullPath), "%s%s", skinPath, filename);
        FILE *f = fopen(fullPath, "rb");
        if (f)
        {
            fseek(f, 0, SEEK_END);
            long sz = ftell(f);
            fseek(f, 0, SEEK_SET);
            fileBuf = (unsigned char*)malloc(sz);
            if (fread(fileBuf, sz, 1, f))
            {
                buffer = fileBuf;
            }
            else
            {
                free(fileBuf);
                fileBuf = NULL;
            }
            fclose(f);
        }
    }

    BITMAPHEADER *header = (BITMAPHEADER *)buffer;
    if (!buffer || header->fh.signature[0] != 'B' || header->fh.signature[1] != 'M')
    {
        if (fileBuf) free(fileBuf);
        return false;
    }

    int width = header->v5.width;
    int height = header->v5.height;
    bmp->width = width;
    bmp->height = height;
    bmp->pixels = (uint32_t*)malloc(width * height * sizeof(uint32_t));

    unsigned int imageSize = header->v5.imageSize;
    if (!imageSize)
    {
        imageSize = header->fh.fileSize - sizeof(BITMAPFILEHEADER) - header->v5.dibHeaderSize;
    }
    unsigned int bytesPerLine = ((width * 3 + 3) / 4) * 4;
    const unsigned char *data = buffer + header->fh.fileOffsetToPixelArray;

    for (int y = 0; y < height; y++)
    {
        const unsigned char *src = data + ((height - 1 - y) * bytesPerLine);
        uint32_t *dst = bmp->pixels + (y * width);
        for (int x = 0; x < width; x++)
        {
            unsigned char b = *(src++);
            unsigned char g = *(src++);
            unsigned char r = *(src++);
            *dst++ = (uint32_t)r | ((uint32_t)g << 8) | ((uint32_t)b << 16) | (0xFF000000U);
        }
    }

    if (fileBuf) free(fileBuf);
    return true;
}

CCocoaToolkit::CCocoaToolkit(void *parentWindow, CEditor *editor)
{
    this->parentWindow  = parentWindow;
    this->editor        = editor;

    CCocoaToolkitImpl *pImpl = new CCocoaToolkitImpl();
    this->impl = pImpl;

    for (int i = 0; i < BMP_COUNT; i++)
    {
        pImpl->bmps[i].width = 0;
        pImpl->bmps[i].height = 0;
        pImpl->bmps[i].pixels = NULL;
    }

    pImpl->screenPixels = (uint32_t*)calloc(GUI_WIDTH * GUI_HEIGHT, sizeof(uint32_t));

    char skinPath[PATH_MAX];
    GetResourcesPath(skinPath, sizeof(skinPath));

    LoadBitmap(&pImpl->bmps[BMP_CHARS  ], (const unsigned char*)chars_bmp  , skinPath, "chars.bmp"  );
    LoadBitmap(&pImpl->bmps[BMP_KNOB   ], (const unsigned char*)knob_bmp   , skinPath, "knob.bmp"   );
    LoadBitmap(&pImpl->bmps[BMP_KNOB2  ], (const unsigned char*)knob2_bmp  , skinPath, "knob2.bmp"  );
    LoadBitmap(&pImpl->bmps[BMP_KNOB3  ], (const unsigned char*)knob3_bmp  , skinPath, "knob3.bmp"  );
    LoadBitmap(&pImpl->bmps[BMP_KEY    ], (const unsigned char*)key_bmp    , skinPath, "key.bmp"    );
    LoadBitmap(&pImpl->bmps[BMP_BG     ], (const unsigned char*)bg_bmp     , skinPath, "bg.bmp"     );
    LoadBitmap(&pImpl->bmps[BMP_BUTTONS], (const unsigned char*)buttons_bmp, skinPath, "buttons.bmp");
    LoadBitmap(&pImpl->bmps[BMP_OPS    ], (const unsigned char*)ops_bmp    , skinPath, "ops.bmp"    );

    pImpl->view = [[PluginView alloc] initWithToolkit:this pixels:pImpl->screenPixels size:NSMakeSize(GUI_WIDTH, GUI_HEIGHT)];
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

        for (int i = 0; i < BMP_COUNT; i++)
        {
            if (pImpl->bmps[i].pixels)
            {
                free(pImpl->bmps[i].pixels);
                pImpl->bmps[i].pixels = NULL;
            }
        }

        if (pImpl->screenPixels)
        {
            free(pImpl->screenPixels);
            pImpl->screenPixels = NULL;
        }

        delete pImpl;
        this->impl = NULL;
    }
}

void CCocoaToolkit::CopyRect(int destX, int destY, int width, int height, int origBmp, int origX, int origY)
{
    CCocoaToolkitImpl *pImpl = (CCocoaToolkitImpl*)this->impl;
    if (!pImpl || origBmp < 0 || origBmp >= BMP_COUNT || !pImpl->bmps[origBmp].pixels || !pImpl->screenPixels)
        return;

    const OxeBitmap &src = pImpl->bmps[origBmp];

    if (destX < 0) { width += destX; origX -= destX; destX = 0; }
    if (destY < 0) { height += destY; origY -= destY; destY = 0; }
    if (destX + width > GUI_WIDTH) width = GUI_WIDTH - destX;
    if (destY + height > GUI_HEIGHT) height = GUI_HEIGHT - destY;
    if (width <= 0 || height <= 0) return;

    if (origX < 0) { width += origX; destX -= origX; origX = 0; }
    if (origY < 0) { height += origY; destY -= origY; origY = 0; }
    if (origX + width > src.width) width = src.width - origX;
    if (origY + height > src.height) height = src.height - origY;
    if (width <= 0 || height <= 0) return;

    for (int y = 0; y < height; y++)
    {
        const uint32_t *s = src.pixels + ((origY + y) * src.width) + origX;
        uint32_t *d = pImpl->screenPixels + ((destY + y) * GUI_WIDTH) + destX;
        memcpy(d, s, width * sizeof(uint32_t));
    }

    [pImpl->controller invalidateRect:NSMakeRect(destX, GUI_HEIGHT - destY - height, width, height)];
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
