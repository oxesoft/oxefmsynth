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

#include "nonguitoolkit.h"
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

void display()
{
    if (!toolkit)
    {
        return;
    }
    unsigned int time = GetTick();
    toolkit->drawBitmap(0, 0, GUI_WIDTH, GUI_HEIGHT, BMP_BG, 0, 0);
    toolkit->editor->GetCoordinates(toolkit->coords);
    oxeCoords *c = toolkit->coords;
    int i = COORDS_COUNT;
    while (i--)
    {
        toolkit->drawBitmap(c->destX, c->destY, c->width, c->height, c->origBmp, c->origX, c->origY);
        c++;
    }
    glFlush();
    printf("opengl draw time: %ums\n", GetTick() - time);
}

void mouseClick(int button, int state, int x, int y)
{
    if (!toolkit)
    {
        return;
    }
    if (state == GLUT_DOWN)
    {
        int w = glutGet(GLUT_WINDOW_WIDTH);
        int h = glutGet(GLUT_WINDOW_HEIGHT);
        x = GUI_WIDTH * x / w;
        y = GUI_HEIGHT * y / h;
        toolkit->editor->OnLButtonDown(x, y);
    }
    else if (state == GLUT_UP)
    {
        toolkit->editor->OnLButtonUp();
    }
}

void mouseMove(int x, int y)
{
    if (!toolkit)
    {
        return;
    }
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);
    x = GUI_WIDTH * x / w;
    y = GUI_HEIGHT * y / h;
    toolkit->editor->OnMouseMove(x, y);
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
    glutMouseFunc(mouseClick);
    glutMotionFunc(mouseMove);
    glutPassiveMotionFunc(mouseMove);
    glutCloseFunc(stopUpdateThread);

    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, GUI_WIDTH, 0, GUI_HEIGHT);

    memset(bmps, 0, sizeof(bmps));
    if (!bmps[BMP_CHARS  ]) bmps[BMP_CHARS  ] = loadTextureFromBuffer(chars_bmp  , &bmps_width[BMP_CHARS  ], &bmps_height[BMP_CHARS  ]);
    if (!bmps[BMP_KNOB   ]) bmps[BMP_KNOB   ] = loadTextureFromBuffer(knob_bmp   , &bmps_width[BMP_KNOB   ], &bmps_height[BMP_KNOB   ]);
    if (!bmps[BMP_KNOB2  ]) bmps[BMP_KNOB2  ] = loadTextureFromBuffer(knob2_bmp  , &bmps_width[BMP_KNOB2  ], &bmps_height[BMP_KNOB2  ]);
    if (!bmps[BMP_KNOB3  ]) bmps[BMP_KNOB3  ] = loadTextureFromBuffer(knob3_bmp  , &bmps_width[BMP_KNOB3  ], &bmps_height[BMP_KNOB3  ]);
    if (!bmps[BMP_KEY    ]) bmps[BMP_KEY    ] = loadTextureFromBuffer(key_bmp    , &bmps_width[BMP_KEY    ], &bmps_height[BMP_KEY    ]);
    if (!bmps[BMP_BG     ]) bmps[BMP_BG     ] = loadTextureFromBuffer(bg_bmp     , &bmps_width[BMP_BG     ], &bmps_height[BMP_BG     ]);
    if (!bmps[BMP_BUTTONS]) bmps[BMP_BUTTONS] = loadTextureFromBuffer(buttons_bmp, &bmps_width[BMP_BUTTONS], &bmps_height[BMP_BUTTONS]);
    if (!bmps[BMP_OPS    ]) bmps[BMP_OPS    ] = loadTextureFromBuffer(ops_bmp    , &bmps_width[BMP_OPS    ], &bmps_height[BMP_OPS    ]);
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

GLuint CGlutToolkit::loadTextureFromBuffer(const char *buffer, int *w, int *h)
{
    BITMAPHEADER *header = (BITMAPHEADER *)buffer;
    if (header->fh.signature[0] != 'B' || header->fh.signature[1] != 'M')
    {
        return 0;
    }
    unsigned char *data = (unsigned char *)buffer + header->fh.fileOffsetToPixelArray;
    *w = header->v5.width;
    *h = header->v5.height;
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

void CGlutToolkit::drawBitmap(int destX, int destY, int width, int height, int origBmp, int origX, int origY)
{
    int imageW = bmps_width[origBmp];
    int imageH = bmps_height[origBmp];
    origY   = imageH     - origY - height;
    destY   = GUI_HEIGHT - destY - height;
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glScalef(1.f / (float)imageW, 1.f / (float)imageH, 1.f);
    glBindTexture(GL_TEXTURE_2D, bmps[origBmp]);
    GLshort vertices[] = {
        destX        , destY + height,
        destX + width, destY + height,
        destX + width, destY         ,
        destX        , destY
    };
    GLshort texVertices[] = {
        origX        , origY + height,
        origX + width, origY + height,
        origX + width, origY         ,
        origX        , origY
    };
    glVertexPointer  (2, GL_SHORT, 0, vertices   );
    glTexCoordPointer(2, GL_SHORT, 0, texVertices);
    glDrawArrays(GL_QUADS, 0, 4);
}

void CGlutToolkit::StartWindowProcesses()
{
    toolkit->stopUpdate    = false;
    toolkit->updateStopped = false;
    pthread_t thread;
    pthread_create(&thread, NULL, &updateProc, (void*)this);
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
