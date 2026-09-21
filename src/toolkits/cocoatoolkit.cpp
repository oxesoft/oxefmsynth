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

#include "editor.h"
#include "cocoawrapper.h"
#include "cocoatoolkit.h"
#include "bitmaps.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef __USE_GNU
#define __USE_GNU
#endif
#include <dlfcn.h>
#define PATH_MAX 512

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

struct CCocoaToolkitImpl
{
    OxeBitmap bmps[BMP_COUNT];
    uint32_t *screenPixels;
};

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

    this->objcInstance  = CocoaToolkitCreate((void*)this);
    CocoaToolkitCreateWindow(this->objcInstance, parentWindow);
}

CCocoaToolkit::~CCocoaToolkit()
{
    CocoaToolkitDestroy(this->objcInstance);

    CCocoaToolkitImpl *pImpl = (CCocoaToolkitImpl*)this->impl;
    if (pImpl)
    {
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

    CocoaToolkitInvalidateRect(this->objcInstance, destX, destY, width, height);
}

void CCocoaToolkit::StartMouseCapture()
{
}

void CCocoaToolkit::StopMouseCapture()
{
}

void CCocoaToolkit::StartWindowProcesses()
{
    CocoaToolkitShowWindow(this->objcInstance);
}

int CCocoaToolkit::WaitWindowClosed()
{
    CocoaToolkitWaitWindowClosed(this->objcInstance);
    return 0;
}

void* CCocoaToolkit::GetScreenPixels()
{
    CCocoaToolkitImpl *pImpl = (CCocoaToolkitImpl*)this->impl;
    return pImpl ? pImpl->screenPixels : NULL;
}

void* CppGetScreenPixels(void *toolkit)
{
    return ((CCocoaToolkit*)toolkit)->GetScreenPixels();
}

void CppOnLButtonDown(void *toolkit, int x, int y)
{
    ((CCocoaToolkit*)toolkit)->editor->OnLButtonDown(x, y);
}

void CppOnLButtonUp(void *toolkit)
{
    ((CCocoaToolkit*)toolkit)->editor->OnLButtonUp();
}

void CppOnDblClick(void *toolkit, int x, int y)
{
    ((CCocoaToolkit*)toolkit)->editor->OnLButtonDblClick(x, y);
}

void CppOnMouseMove(void *toolkit, int x, int y)
{
    ((CCocoaToolkit*)toolkit)->editor->OnMouseMove(x, y);
}

void CppOnMouseWheel(void *toolkit, int x, int y, int delta)
{
    ((CCocoaToolkit*)toolkit)->editor->OnMouseWheel(x, y, delta);
}

void CppOnChar(void *toolkit, int c)
{
    ((CCocoaToolkit*)toolkit)->editor->OnChar(c);
}

void CppUpdate(void *toolkit)
{
    CCocoaToolkit *t = (CCocoaToolkit*)toolkit;
    if (t && t->editor)
    {
        t->editor->Update();
    }
}
