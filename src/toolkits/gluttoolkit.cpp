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
#define GL_GLEXT_PROTOTYPES
#include <GL/glut.h>  // GLUT, include glu.h and gl.h
#include "gluttoolkit.h"
#include <stdio.h>

//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------

typedef struct
{
    GLfloat positionCoordinates[3];
    GLfloat textureCoordinates[2];
} VertexData;

VertexData vertices[] = {
    {{0.0f     , 0.0f      , 0.0f}, {0.0f, 0.0f}},
    {{GUI_WIDTH, 0.0f      , 0.0f}, {1.0f, 0.0f}},
    {{GUI_WIDTH, GUI_HEIGHT, 0.0f}, {1.0f, 1.0f}},
    {{0.0f     , GUI_HEIGHT, 0.0f}, {0.0f, 1.0f}}
};

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_QUADS, 0, 4);
    glFlush();
}

CGlutToolkit::CGlutToolkit(void *parentWindow, CEditor *editor)
{
    this->parentWindow  = parentWindow;
    this->editor        = editor;

    int argc = 1;
    char *argv[1] = {0};
    glutInit(&argc, argv);
    glutInitWindowSize(GUI_WIDTH, GUI_HEIGHT);
    glutCreateWindow(TITLE_FULL);
    glutDisplayFunc(display);

    glEnable(GL_TEXTURE_2D);

    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0, GUI_WIDTH, 0, GUI_HEIGHT);

    glGenBuffers(1, &vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, sizeof(VertexData),(GLvoid*) offsetof(VertexData, positionCoordinates));

    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, sizeof(VertexData), (GLvoid*)offsetof(VertexData, textureCoordinates));

    textureBufferID = loadTexture(bg_bmp);
}

CGlutToolkit::~CGlutToolkit()
{
    glDeleteTextures(1, &textureBufferID);
    glDeleteBuffers(1, &vertexBufferID);
}

GLuint CGlutToolkit::loadTexture(const char *buffer)
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    return textureBufferID;
}

void CGlutToolkit::StartWindowProcesses()
{
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
