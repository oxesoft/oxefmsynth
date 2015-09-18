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
    int           bmp;            // bitmap
    int           w;              // width in pixels
    int           h;              // height in pixels
    CSynthesizer *synthesizer;    // object to get/set the value
    CToolkit     *toolkit;        // graphical toolkit
    int           par;            // synth parameter assigned
    int           index;          // bitmap index in bitmap table
    void Repaint();
public:
    CKey(int bmp, int index, int w, int h, const char *name, CSynthesizer *synthesizer, char &channel, int par, int x, int y);
    void  OnClick        (int x, int y);
    bool  Update         (void);
    bool  GetName        (char* str);
    int   GetIndex       (void);
    int   GetType        (void);
    bool  SetValue       (char channel, char value);
    float GetValue       (char channel);
    void  SetToolkit     (CToolkit *toolkit);
};
