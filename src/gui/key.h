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

class CKey : public CControl
{
private:
    char          *channel;       // MIDI channel
    char          value;          // value
    char          name[TEXT_SIZE];// key name
    HWND          hwnd;           // window handle
    HDC           dc;             // used by offscreen buffer
    HDC           memdc;          // used by offscreen buffer
    HBITMAP       bmp;            // bitmap
    int           chave_h;        // width in pixels
    int           chave_v;        // height in pixels
    CSynthesizer *synthesizer;    // object to get/set the value
    int           par;            // synth parameter assigned
    int           index;          // bitmap index in bitmap table
    void Repaint();
public:
    CKey(HBITMAP bmp, int index, int chave_h, int chave_v, const char *name, CSynthesizer *synthesizer, char &channel, int par, int x, int y);
    void  OnClick        (POINT point);
    bool  Update         (void);
    bool  GetName        (char* str);
    int   GetIndex       (void);
    int   GetType        (void);
    bool  SetValue       (char channel, char value);
    float GetValue       (char channel);
    void  SetHandlers    (HWND hwnd, HDC dc, HDC memdc);
};
