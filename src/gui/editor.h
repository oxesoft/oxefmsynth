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

enum
{
    BMP_CHARS  ,
    BMP_KNOB   ,
    BMP_KNOB2  ,
    BMP_KNOB3  ,
    BMP_KEY    ,
    BMP_BG     ,
    BMP_BUTTONS,
    BMP_OPS    ,
    BMP_COUNT
};

class CEditor
{
private:
    bool          changingControl;
    int           currentX;
    int           currentY;
    int           prevX;
    int           prevY;
    CLcd          *lcd;
    CControl      *ctl[GUI_CONTROLS];
    CSynthesizer  *synthesizer;
    int           cID;
    char          channel;
    int           TranslateNote(int cod);
    CToolkit      *toolkit;
public:
    CEditor(CSynthesizer *synthesizer);
    ~CEditor();
    void  ProgramChanged();
    void  ProgramChangedWaiting();
    void  OnLButtonDblClick(int x, int y);
    void  OnLButtonDown    (int x, int y);
    void  OnLButtonUp      ();
    void  OnRButtonDown    ();
    bool  OnChar           (int cod);
    void  OnMouseMove      (int x, int y);
    void  OnMouseWheel     (int x, int y, int delta);
    void  SetPar           (int index, float value);
    float GetPar           (int index);
    void  GetParLabel      (int index, char* label);
    void  GetParDisplay    (int index, char* text);
    void  GetParName       (int index, char* text);
    void  Update           ();
    void  SetToolkit       (CToolkit *toolkit);
};
