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

#define TEXT_SIZE 32

class CControl
{
public:
	virtual void  OnClick        (POINT point)                  {             }
	virtual bool  IsMouseOver    (POINT point)                  {return false;}
	virtual bool  Repaint        (HDC dc, HDC memdc)            {return false;}
	virtual bool  GetName        (char* str)                    {return false;}
	virtual bool  Update         (void)                         {return false;}
	virtual bool  IsKnob         (void)                         {return false;}
	virtual bool  IncreaseValue  (int delta)                    {return false;}
	virtual int   GetIndex       (void)                         {return -1   ;}
	virtual int   GetType        (void)                         {return -1   ;}
	virtual void  SetHandlers    (HWND hwnd, HDC dc, HDC memdc) {             }
};

enum
{
	VL_ZERO_TO_ONE,
	VL_MINUS1_2_PLUS1,
	VL_COARSE_TUNE,
	VL_FINE_TUNE,
	VL_TEMPO,
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
