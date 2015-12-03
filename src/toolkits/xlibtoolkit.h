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

class CXlibToolkit : public COpenGLToolkit
{
private:
    Pixmap bmps[BMP_COUNT];
    Pixmap LoadImageFromFile(const char *path, XVisualInfo *v);
    Pixmap LoadImageFromBuffer(const char *buffer, XVisualInfo *v);
public:
    void        *parentWindow;
    CEditor     *editor;
    Display      *display;
    Window       window;
    GC           gc;
    Atom         WM_DELETE_WINDOW;
    Atom         WM_TIMER;
    Pixmap       offscreen;
    bool         thread1Finished;
    bool         thread2Finished;
    GLXContext   glxContext;
    bool         openGLmode;
    CXlibToolkit(void *parentWindow, CEditor *editor);
    ~CXlibToolkit();
    void StartWindowProcesses();
    void CopyRect(int destX, int destY, int width, int height, int origBmp, int origX, int origY);
    int  WaitWindowClosed();
};
