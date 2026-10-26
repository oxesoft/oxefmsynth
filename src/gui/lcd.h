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

#define LCD_COLS    16 // columns
#define LCD_LINES   2  // lines

class CLcd : public CControl
{
private:
    char text[LCD_LINES][TEXT_SIZE];
public:
    CLcd(int bmpchars, int x, int y);
    void ShowWelcome();
    bool SetText(char lineIndex, const char *text);
    void GetRepaintBounds(int &rx, int &ry, int &rw, int &rh) const override;
    void Paint(BLContext &ctx, const BLFont &fontSmall, const BLFont &fontNormal) override;
};
