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
#include <GL/freeglut.h>
#include "gluttoolkit.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

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

CGlutToolkit *toolkit = NULL;

void drawBitmap(int imageW, int imageH, int destX, int destY, int width, int height, int origBmp, int origX, int origY)
{
    origY   = imageH     - origY - height;
    destY   = GUI_HEIGHT - destY - height;
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glScalef(1.f / (float)imageW, 1.f / (float)imageH, 1.f);
    glBindTexture(GL_TEXTURE_2D, toolkit->bmps[origBmp]);
    glBegin(GL_QUADS);
    glTexCoord2s(origX        , origY + height);
    glVertex2s  (destX        , destY + height);
    glTexCoord2s(origX + width, origY + height);
    glVertex2s  (destX + width, destY + height);
    glTexCoord2s(origX + width, origY         );
    glVertex2s  (destX + width, destY         );
    glTexCoord2s(origX        , origY         );
    glVertex2s  (destX        , destY         );
    glEnd();
}

void display()
{
    if (!toolkit)
    {
        return;
    }
    glClear(GL_COLOR_BUFFER_BIT);
    drawBitmap(633, 437,  0,  0, 633, 437, BMP_BG  , 0, 0);
    drawBitmap(250, 250, 33, 80,  25,  25, BMP_KNOB, 0, 0);
    glFlush();
}

void* updateProc(void* ptr)
{
    if (!toolkit)
    {
        return NULL;
    }
    CGlutToolkit *toolkit = (CGlutToolkit*)ptr;
    while (!toolkit->stopUpdate)
    {
        glutPostRedisplay();
        usleep(1000 * TIMER_RESOLUTION_MS);
    }
    toolkit->updateStopped = true;
    return NULL;
}

void stopUpdateThread(void)
{
    if (!toolkit)
    {
        return;
    }
    toolkit->stopUpdate = true;
    while (!toolkit->updateStopped)
    {
        usleep(1000 * 100);
    }
}

CGlutToolkit::CGlutToolkit(void *parentWindow, CEditor *editor)
{
    toolkit = this;
    this->parentWindow  = parentWindow;
    this->editor        = editor;

    int argc = 1;
    char *argv[1] = {0};
    glutInit(&argc, argv);
    glutInitWindowSize(GUI_WIDTH, GUI_HEIGHT);
    glutCreateWindow(TITLE_FULL);
    glutDisplayFunc(display);
    glutCloseFunc(stopUpdateThread);

    glEnable(GL_TEXTURE_2D);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, GUI_WIDTH, 0, GUI_HEIGHT);

    memset(bmps, 0, sizeof(bmps));
    if (!bmps[BMP_CHARS  ]) bmps[BMP_CHARS  ] = loadTextureFromBuffer(chars_bmp  );
    if (!bmps[BMP_KNOB   ]) bmps[BMP_KNOB   ] = loadTextureFromBuffer(knob_bmp   );
    if (!bmps[BMP_KNOB2  ]) bmps[BMP_KNOB2  ] = loadTextureFromBuffer(knob2_bmp  );
    if (!bmps[BMP_KNOB3  ]) bmps[BMP_KNOB3  ] = loadTextureFromBuffer(knob3_bmp  );
    if (!bmps[BMP_KEY    ]) bmps[BMP_KEY    ] = loadTextureFromBuffer(key_bmp    );
    if (!bmps[BMP_BG     ]) bmps[BMP_BG     ] = loadTextureFromBuffer(bg_bmp     );
    if (!bmps[BMP_BUTTONS]) bmps[BMP_BUTTONS] = loadTextureFromBuffer(buttons_bmp);
    if (!bmps[BMP_OPS    ]) bmps[BMP_OPS    ] = loadTextureFromBuffer(ops_bmp    );
}

CGlutToolkit::~CGlutToolkit()
{
    for (int i = 0; i < BMP_COUNT; i++)
    {
        if (bmps[i])
        {
            glDeleteTextures(1, &bmps[i]);
        }
    }
}

GLuint CGlutToolkit::loadTextureFromBuffer(const char *buffer)
{
    BITMAPHEADER *header = (BITMAPHEADER *)buffer;
    if (header->fh.signature[0] != 'B' || header->fh.signature[1] != 'M')
    {
        return 0;
    }
    unsigned char *data = (unsigned char *)buffer + header->fh.fileOffsetToPixelArray;
    //--------------------------------------------------------------------------
    GLuint textureBufferID;
    glGenTextures(1, &textureBufferID);
    glBindTexture(GL_TEXTURE_2D, textureBufferID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, header->v5.width, header->v5.height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    return textureBufferID;
}

void CGlutToolkit::StartWindowProcesses()
{
    toolkit->stopUpdate    = false;
    toolkit->updateStopped = false;
    pthread_t thread;
    pthread_create(&thread, NULL, &updateProc, (void*)this);
}

void CGlutToolkit::CopyRect(int destX, int destY, int width, int height, int origBmp, int origX, int origY)
{
}

void CGlutToolkit::Debug(char *text)
{
    printf("%s\n", text);
}

int CGlutToolkit::WaitWindowClosed()
{
    glutMainLoop();
    return 0;
}
