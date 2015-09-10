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

#define  FMAXINT 2147483647.f // = 0x7FFFFFFF
#ifndef   MAXINT
#define   MAXINT 0x7FFFFFFF
#endif

class CEnvelop
{
private:
	float  sr;
	char   es;         // envelop state
	int    counter;    // to the next state
	int    coef;       // coeficient
	int    sa;         // output signal
	float  dl;         // delay time
	float  at;         // attack time
	float  de;         // decay time
	float  su;         // sustain level
	float  st;         // sustein time
	float  re;         // release time
	float  ss;         // single sample mode
	int CalcCoef();
public:
	void  Init();
	float Process();   // single sample mode
	void  SendEvent(char event, int remainingSamples);
	char  GetState(void);
	void  SetPar(char param, float value);
	void  Process(int *b, int size, int offset, float volume);
};
