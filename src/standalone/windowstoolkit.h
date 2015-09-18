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

class CWindowsToolkit : public CToolkit
{
private:
    HWND    hWnd;
    HDC     hdc;
    HDC     hdcMem;
    HDC     hdcAux;
    HBITMAP bitmap;
    HBITMAP bmps[BMP_COUNT];
public:
    CWindowsToolkit(HINSTANCE hInstance, HWND hWnd);
    ~CWindowsToolkit();
    void CopyRect(int destX, int destY, int width, int height, int origBmp, int origX, int origY);
    void SendMessageToHost(unsigned int messageID, unsigned int par1, unsigned int par2);
    void GetMousePosition(int *x, int *y);
    void StartMouseCapture();
    void StopMouseCapture();
    void OutputDebugString(char *text);
    void *GetImageBuffer();
};
