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

class CKnob : public CControl
{
private:
    signed char value;     // integer value
    float fvalue;          // float value
    char  name[TEXT_SIZE]; // knob name
    int   knobSize;        // size in pixels
    int   par;             // synth parameter assigned
    int   type;            // control type
    const char* shortLabel;
    bool  isMatrix;
    bool  isOutput;
    bool  isPan;
    bool  isSelfMod;
    void  Repaint() override;
public:
    CKnob(int bmp, int knobSize, const char *name, CSynthesizer *synthesizer, char &channel, int type, int par, int x, int y);
    bool  Update         (void) override;
    bool  GetName        (char* str) override;
    bool  IsKnob()       override {return true;}
    bool  IncreaseValue  (int delta) override;
    int   GetIndex       (void) override;
    int   GetType        (void) override;
    float GetValue       (char channel);
    int   GetCoordinates (oxeCoords *coords) override;
    void  Paint          (BLContext &ctx, const BLFont &fontSmall, const BLFont &fontNormal) override;
};
