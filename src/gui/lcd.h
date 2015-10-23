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

#define LCD_COLS    16 // columns
#define LCD_LINES   2  // lines
#define LCD_COORDS  (LCD_COLS * LCD_LINES)

class CLcd : public CControl
{
private:
    char     text0[LCD_COLS];
    char     text1[LCD_COLS];
    int      lcdx;
    int      lcdy;
    void Repaint();
public:
    CLcd(int bmpchars, int x, int y);
    bool SetText(char lineIndex, const char *text);
    int  GetCoordinates(oxeCoords *coords);
};
