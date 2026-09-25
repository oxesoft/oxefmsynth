/*
Oxe FM Synth: a software synthesizer
Copyright (C) 2004-2015  Daniel Moura <oxesoft@gmail.com>

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

#define COORDS_COUNT (LCD_COORDS + MIDICHANNELS + GUI_CONTROLS - 1)

class CEditor
{
private:
    bool           changingControl;
    int            currentX;
    int            currentY;
    int            prevX;
    int            prevY;
    CLcd           *lcd;
    CControl       *ctl[GUI_CONTROLS];
    CSynthesizer   *synthesizer;
    int            cID;
    char           channel;
    int            TranslateNote(int cod);
    CToolkit       *toolkit;
    CHostInterface *hostinterface;
    BLImage        bgCache;
    void           RenderBackgroundCache(int targetW, int targetH);
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
    void  GetCoordinates   (oxeCoords *coords);
    void  SetToolkit       (CToolkit *toolkit);
    void  SetHostInterface (CHostInterface *hostinterface);
    void  Paint            (BLContext &ctx, int dirtyX = 0, int dirtyY = 0, int dirtyW = 0, int dirtyH = 0);
private:
    void  DrawPanelCards   (BLContext &ctx, const BLFont &fontSmall, const BLFont &fontNormal, const BLFont &fontHeader);
    void  DrawOperatorEnvelope(BLContext &ctx, int opIndex, float x, float y, float w, float h, const BLFont &fontSmall, const BLRgba32 &accentColor = BLRgba32(0x00, 0xf0, 0xff));
    void  DrawFilterCurve  (BLContext &ctx, float x, float y, float w, float h, const BLFont &fontSmall, const BLRgba32 &accentColor = BLRgba32(0x00, 0xe6, 0x76));
    void  DrawMatrixDecorations(BLContext &ctx, const BLFont &fontSmall, const BLFont &fontNormal);
    void  DrawOxeLogo      (BLContext &ctx, float x, float y, float scale = 0.58f);
};
