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

#pragma once

#include <blend2d/blend2d.h>

inline char GetOpLetter(int index)
{
    static const char opLetters[8] = {'A', 'B', 'C', 'D', 'E', 'F', 'X', 'Z'};
    return (index >= 0 && index < 8) ? opLetters[index] : '?';
}

inline BLRgba32 GetOpAccentColor(int index)
{
    static const BLRgba32 accents[8] = {
        BLRgba32(0x00, 0xf0, 0xff), // A: cyan
        BLRgba32(0x00, 0xe6, 0x76), // B: emerald green
        BLRgba32(0xff, 0xd6, 0x00), // C: amber
        BLRgba32(0xff, 0x91, 0x00), // D: orange
        BLRgba32(0xb3, 0x88, 0xff), // E: violet
        BLRgba32(0xff, 0x40, 0x81), // F: magenta
        BLRgba32(0x40, 0xc4, 0xff), // X: ice blue
        BLRgba32(0xee, 0xf2, 0xf6)  // Z: white
    };
    return (index >= 0 && index < 8) ? accents[index] : accents[0];
}

class CControl
{
protected:
    int left;
    int top;
    int right;
    int bottom;
    char           *channel;       // MIDI channel
    int            bmp;            // image id
    CSynthesizer   *synthesizer;   // object to get/set the value
    CToolkit       *toolkit;       // graphical toolkit
    CHostInterface *hostinterface;
    virtual void  Repaint        ()
    {
        if (toolkit)
        {
            int rx, ry, rw, rh;
            GetRepaintBounds(rx, ry, rw, rh);
            toolkit->InvalidateRect(rx, ry, rw, rh);
        }
    }
public:
    virtual ~CControl() {}
    virtual void  OnClick        (int x, int y)          {             }
    virtual bool  GetName        (char* str)             {return false;}
    virtual bool  Update         (void)                  {return false;}
    virtual bool  IsKnob         (void)                  {return false;}
    virtual bool  IncreaseValue  (int delta)             {return false;}
    virtual int   GetIndex       (void)                  {return -1   ;}
    virtual int   GetType        (void)                  {return -1   ;}
    virtual void  Paint          (BLContext &ctx, const BLFont &fontSmall, const BLFont &fontNormal) {}
    void SetToolkit(CToolkit *toolkit)
    {
        this->toolkit = toolkit;
        if (toolkit)
        {
            Repaint();
        }
    }
    void SetHostInterface(CHostInterface *hostinterface)
    {
        this->hostinterface = hostinterface;
    }
    bool IsMouseOver(int x, int y)
    {
        return ((x >= left) && (x < right) && (y >= top) && (y < bottom));
    }
    int GetLeft()   const { return left; }
    int GetTop()    const { return top; }
    int GetRight()  const { return right; }
    int GetBottom() const { return bottom; }
    int GetWidth()  const { return right - left; }
    int GetHeight() const { return bottom - top; }
    virtual void GetRepaintBounds(int &rx, int &ry, int &rw, int &rh) const
    {
        rx = left;
        ry = top;
        rw = right - left;
        rh = bottom - top;
    }
};

enum
{
    VL_ZERO_TO_ONE,
    VL_MINUS1_2_PLUS1,
    VL_COARSE_TUNE,
    VL_FINE_TUNE,
    VL_TIME,
    VL_PORTAMENTO,
    VL_WAVEFORM,
    VL_FILTER,
    VL_MOD,
    VL_PAN,
    VL_PITCH_CURVE,
    VL_LFO_RATE,
    VL_LFO_DEST,
    VL_MOD_DEST,
    VL_ON_OFF,
    VL_CHANNELS,
    VL_FILTER_CUTOFF,
    BT_BANK,
    BT_PROGRAM,
    BT_MINUS_10,
    BT_MINUS_1,
    BT_PLUS_1,
    BT_PLUS_10,
    BT_NAME,
    BT_STORE
};
