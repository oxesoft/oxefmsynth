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

#define VERTEX_SIZE      3
#define TEXVER_SIZE      2
#define TWO_TRIANGLES    6 // indices
#define VERTEX_STRIDE    (VERTEX_SIZE * TWO_TRIANGLES)
#define TEXVER_STRIDE    (TEXVER_SIZE * TWO_TRIANGLES)
#define TOTAL_VALUES_VER (VERTEX_STRIDE * (1 /*<- bg */ + COORDS_COUNT))
#define TOTAL_VALUES_TEX (TEXVER_STRIDE * (1 /*<- bg */ + COORDS_COUNT))

class CGlutToolkit : public CToolkit
{
private:
    void loadImageToBuffer(unsigned char *destB, int destX, int destY, unsigned char *buffer);
public:
    bool        stopUpdate;
    bool        updateStopped;
    GLuint      texture;
    GLuint      vertexBuffer;
    GLuint      texverBuffer;
    oxeCoords   coords[COORDS_COUNT];
    GLfloat     vertices   [TOTAL_VALUES_VER];
    GLfloat     texVertices[TOTAL_VALUES_TEX];
    void        *parentWindow;
    CEditor     *editor;
    CGlutToolkit(void *parentWindow, CEditor *editor);
    ~CGlutToolkit();
    GLfloat* updateVerticesXYZ(GLfloat x, GLfloat y, GLfloat w, GLfloat h, GLfloat iW, GLfloat iH, GLfloat *v);
    GLfloat* updateVerticesUV(GLfloat x, GLfloat y, GLfloat w, GLfloat h, int origBmp, GLfloat *v);
    void     StartWindowProcesses();
    void     Debug(char *text);
    int      WaitWindowClosed();
};
