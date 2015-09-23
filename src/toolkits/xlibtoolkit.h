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

class CXlibToolkit : public CToolkit
{
private:
    XImage* LoadImage(const char *path);
public:
    Display *display;
    Window   window;
    GC       gc;
    Atom     customMessage;
    Pixmap   offscreen;
    XImage   *bmps[BMP_COUNT];
    bool     threadFinished;
    CEditor  *editor;
    CXlibToolkit(void *parentWindow, CEditor *editor);
    ~CXlibToolkit();
    void CopyRect(int destX, int destY, int width, int height, int origBmp, int origX, int origY);
    void SendMessageToHost(unsigned int messageID, unsigned int par1, unsigned int par2);
    void GetMousePosition(int *x, int *y);
    void StartMouseCapture();
    void StopMouseCapture();
    void OutputDebugString(char *text);
    void *GetImageBuffer();
};
