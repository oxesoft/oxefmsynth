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

class CGlutToolkit : public CToolkit
{
private:
    GLuint loadTextureFromBuffer(const char *buffer);
public:
    bool        stopUpdate;
    bool        updateStopped;
    GLuint      bmps[BMP_COUNT];
    void        *parentWindow;
    CEditor     *editor;
    CGlutToolkit(void *parentWindow, CEditor *editor);
    ~CGlutToolkit();
    void StartWindowProcesses();
    void CopyRect(int destX, int destY, int width, int height, int origBmp, int origX, int origY);
    void Debug(char *text);
    int  WaitWindowClosed();
};
