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

#include "synthesizer.h"
#include "control.h"
#include "lcd.h"
#include "knob.h"
#include "button.h"
#include "key.h"
#include "channels.h"

#define GUI_CONTROLS 189
#define GUI_WIDTH    633
#define GUI_HEIGHT   437

class CEditor
{
private:
    bool          changingControl;
    HCURSOR       cursor;
    POINT         prevpoint;
    HBITMAP       bmpchars;
    HBITMAP       bmpknob;
    HBITMAP       bmpknob2;
    HBITMAP       bmpknob3;
    HBITMAP       bmpkey;
    HBITMAP       bmpbg;
    HBITMAP       bmpbuttons;
    HBITMAP       bmpops;
    HWND          hwnd;
    CLcd          *lcd;
    CControl      *ctl[GUI_CONTROLS];
    CSynthesizer  *synthesizer;
    int           cID;
    char          channel;
    int           TranslateNote(int cod);
    // double buffering stuff
    HDC           hdc;
    HDC           hdcMem;
    HDC           hdcAux;
    HBITMAP       bitmap;
public:
    CEditor(HINSTANCE hInstance, CSynthesizer *synthesizer);
    ~CEditor();
    void  ProgramChanged();
    void  ProgramChangedWaiting();
    void  OnLButtonDblClick(POINT point);
    void  OnLButtonDown    (POINT point);
    void  OnLButtonUp      ();
    void  OnRButtonDown    ();
    bool  OnChar           (int cod);
    void  OnMouseMove      (POINT point);
    void  OnMouseWheel     (POINT point, int delta);
    void  OnPaint          (HDC dc, RECT rect);
    void  OnTimer          (HWND hWnd);
    void  SetPar           (int index, float value);
    float GetPar           (int index);
    void  GetParLabel      (int index, char* label);
    void  GetParDisplay    (int index, char* text);
    void  GetParName       (int index, char* text);
    void  SetHandle        (HWND hwnd);
};
