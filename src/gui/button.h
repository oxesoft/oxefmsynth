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

#define BOTAO_H   42 // Tamanho horizontal do bitmap em pixels
#define BOTAO_V   16 // Tamanho vertical   do bitmap em pixels

class CButton : public CControl
{
private:
	char          *channel;       // MIDI channel
	RECT          rect;           // button rect
	HWND          hwnd;           // window handle
	HDC           dc;             // used by offscreen buffer
	HDC           memdc;          // used by offscreen buffer
	HBITMAP       bmp;            // bitmap
	CSynthesizer  *synthesizer;   // object to get/set the value
	int           type;           // button type
	int           index;
	void Repaint();
public:
	CButton(HBITMAP bmp, int index, CSynthesizer *synthesizer, char &channel, int type, int x, int y);
	void OnClick(POINT point);
	bool IsMouseOver(POINT point);
	bool GetName(char* str);
	int  GetType(void);
	void SetHandlers(HWND hwnd, HDC dc, HDC memdc);
};
