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

class CKey : public CControl
{
private:
    char value;           // value
    char name[TEXT_SIZE]; // key name
    int  w;               // width in pixels
    int  h;               // height in pixels
    int  par;             // synth parameter assigned
    int  index;           // bitmap index in bitmap table
    void Repaint() override;
public:
    CKey(int bmp, int index, int w, int h, const char *name, CSynthesizer *synthesizer, char &channel, int par, int x, int y);
    void  OnClick        (int x, int y) override;
    bool  Update         (void) override;
    bool  GetName        (char* str) override;
    int   GetIndex       (void) override;
    int   GetType        (void) override;
    bool  SetValue       (char channel, char value);
    float GetValue       (char channel);
    int   GetCoordinates (oxeCoords *coords) override;
    void  Paint          (BLContext &ctx, const BLFont &fontSmall, const BLFont &fontNormal) override;
};
