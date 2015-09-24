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
    char  value;           // integer value
    float fvalue;          // float value
    char  name[TEXT_SIZE]; // knob name
    int   knobSize;        // size in pixels
    int   par;             // synth parameter assigned
    int   type;            // control type
    void  Repaint();
public:
    CKnob(int bmp, int knobSize, const char *name, CSynthesizer *synthesizer, char &channel, int type, int par, int x, int y);
    bool  Update         (void);
    bool  GetName        (char* str);
    bool  IsKnob()       {return true;}
    bool  IncreaseValue  (int delta);
    int   GetIndex       (void);
    int   GetType        (void);
    float GetValue       (char channel);
};
