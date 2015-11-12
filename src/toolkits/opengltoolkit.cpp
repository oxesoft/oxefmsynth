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
#if defined(__APPLE__)
    #include <OpenGL/gl.h>
#elif defined(__linux)
    #define GL_GLEXT_PROTOTYPES
    #include <GL/gl.h>
#endif
#include "opengltoolkit.h"
#include <stdlib.h>

#define TEX_WIDTH     (633 + 250       + 5 /* to align */)
#define TEX_HEIGHT    (250 + 200 + 200 + 6 /* to align */)
#define PIXEL_BYTES   4 /* RGBA */
#define TOTAL_INDICES ((1 /*<- bg */ + COORDS_COUNT) * TWO_TRIANGLES)

int texCoods[][2] =
{
    {  0, 437}, // BMP_CHARS
    {633,   0}, // BMP_KNOB
    {633, 250}, // BMP_KNOB2
    {633, 450}, // BMP_KNOB3
    { 80, 437}, // BMP_KEY
    {  0,   0}, // BMP_BG
    {100, 437}, // BMP_BUTTONS
    {142, 437}  // BMP_OPS
};

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

void COpenGLToolkit::Init(CEditor *editor)
{
    this->editor = editor;

    unsigned char *rawBigTexture = (unsigned char*)malloc(TEX_WIDTH * TEX_HEIGHT * PIXEL_BYTES);
    loadImageToBuffer(rawBigTexture, texCoods[BMP_CHARS  ][0], texCoods[BMP_CHARS  ][1], (unsigned char *)chars_bmp  );
    loadImageToBuffer(rawBigTexture, texCoods[BMP_KNOB   ][0], texCoods[BMP_KNOB   ][1], (unsigned char *)knob_bmp   );
    loadImageToBuffer(rawBigTexture, texCoods[BMP_KNOB2  ][0], texCoods[BMP_KNOB2  ][1], (unsigned char *)knob2_bmp  );
    loadImageToBuffer(rawBigTexture, texCoods[BMP_KNOB3  ][0], texCoods[BMP_KNOB3  ][1], (unsigned char *)knob3_bmp  );
    loadImageToBuffer(rawBigTexture, texCoods[BMP_KEY    ][0], texCoods[BMP_KEY    ][1], (unsigned char *)key_bmp    );
    loadImageToBuffer(rawBigTexture, texCoods[BMP_BG     ][0], texCoods[BMP_BG     ][1], (unsigned char *)bg_bmp     );
    loadImageToBuffer(rawBigTexture, texCoods[BMP_BUTTONS][0], texCoods[BMP_BUTTONS][1], (unsigned char *)buttons_bmp);
    loadImageToBuffer(rawBigTexture, texCoods[BMP_OPS    ][0], texCoods[BMP_OPS    ][1], (unsigned char *)ops_bmp    );

    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TEX_WIDTH, TEX_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, rawBigTexture);
    free(rawBigTexture);

    editor->GetCoordinates(this->coords);

    oxeCoords *c;
    GLfloat *v;
    int i;

    c = this->coords;
    v = this->vertices;
    i = COORDS_COUNT;
    v = updateVerticesXYZ(0, 0, GUI_WIDTH, GUI_HEIGHT, GUI_WIDTH, GUI_HEIGHT, v); // bg
    while (i--)
    {
        v = updateVerticesXYZ(c->destX, c->destY, c->width, c->height, GUI_WIDTH, GUI_HEIGHT, v);
        c++;
    }

    c = this->coords;
    v = this->texCoords;
    i = COORDS_COUNT;
    v = updateVerticesUV(0, 0, GUI_WIDTH, GUI_HEIGHT, BMP_BG, v); // bg
    while (i--)
    {
        v = updateVerticesUV(c->origX, c->origY, c->width, c->height, c->origBmp, v);
        c++;
    }

    glEnableClientState(GL_VERTEX_ARRAY);
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexPointer(3, GL_FLOAT, 0, 0);

    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glGenBuffers(1, &coordsBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, coordsBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_DYNAMIC_DRAW);
    glTexCoordPointer(2, GL_FLOAT, 0, 0);
}

void COpenGLToolkit::Deinit()
{
    glDeleteTextures(1, &texture);
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &coordsBuffer);
}

void COpenGLToolkit::loadImageToBuffer(unsigned char *destB, int destX, int destY, unsigned char *buffer)
{
    BITMAPHEADER *header = (BITMAPHEADER *)buffer;
    if (header->fh.signature[0] != 'B' || header->fh.signature[1] != 'M')
    {
        return;
    }
    unsigned char *data = (unsigned char *)buffer + header->fh.fileOffsetToPixelArray;
    unsigned int bytesPerLine = header->v5.imageSize / header->v5.height;
    unsigned char *d;
    unsigned char *s;
    int r, g, b;
    int c;
    int l = header->v5.height;
    while (l)
    {
        d = destB + (TEX_WIDTH * ((TEX_HEIGHT - 1 - destY) - (header->v5.height - l)) + destX) * PIXEL_BYTES;
        c = header->v5.width;
        s = data + (--l * bytesPerLine);
        while (c--)
        {
            b = *(s++);
            g = *(s++);
            r = *(s++);
            *(d++) = r;
            *(d++) = g;
            *(d++) = b;
            *(d++) = 0xFF;
        }
    }
}

GLfloat* COpenGLToolkit::updateVerticesXYZ(GLfloat x, GLfloat y, GLfloat w, GLfloat h, GLfloat iW, GLfloat iH, GLfloat *v)
{
    // flipping
    y = iH - y - h;
    // ajusts to range -1.0 to 1.0
    x /= GUI_WIDTH  / 2.f;
    y /= GUI_HEIGHT / 2.f;
    w /= GUI_WIDTH  / 2.f;
    h /= GUI_HEIGHT / 2.f;
    x -= 1.f;
    y -= 1.f;
    // triangle 1
    *(v++) = x    ; *(v++) = y + h; *(v++) = 0;
    *(v++) = x + w; *(v++) = y + h; *(v++) = 0;
    *(v++) = x + w; *(v++) = y    ; *(v++) = 0;
    // triangle 2
    *(v++) = x + w; *(v++) = y    ; *(v++) = 0;
    *(v++) = x    ; *(v++) = y    ; *(v++) = 0;
    *(v++) = x    ; *(v++) = y + h; *(v++) = 0;
    return v;
}

GLfloat* COpenGLToolkit::updateVerticesUV(GLfloat x, GLfloat y, GLfloat w, GLfloat h, int origBmp, GLfloat *v)
{
    // adjusts to the big-composited-texture coordinate
    x += texCoods[origBmp][0];
    y += texCoods[origBmp][1];
    // flipping
    y = TEX_HEIGHT - y - h;
    // ajusts to range 0.0 to 1.0
    x /= TEX_WIDTH;
    y /= TEX_HEIGHT;
    w /= TEX_WIDTH;
    h /= TEX_HEIGHT;
    // triangle 1
    *(v++) = x    ; *(v++) = y + h;
    *(v++) = x + w; *(v++) = y + h;
    *(v++) = x + w; *(v++) = y    ;
    // triangle 2
    *(v++) = x + w; *(v++) = y    ;
    *(v++) = x    ; *(v++) = y    ;
    *(v++) = x    ; *(v++) = y + h;
    return v;
}

void COpenGLToolkit::Draw()
{
    GLfloat *v = this->texCoords + COORDS_STRIDE;
    this->editor->GetCoordinates(this->coords);
    oxeCoords *c = this->coords;
    int i = COORDS_COUNT;
    while (i--)
    {
        v = this->updateVerticesUV(c->origX, c->origY, c->width, c->height, c->origBmp, v);
        c++;
    }
    glClear(GL_COLOR_BUFFER_BIT);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(this->texCoords), this->texCoords);
    glDrawArrays(GL_TRIANGLES, 0, TOTAL_INDICES);
    glFlush();
}
