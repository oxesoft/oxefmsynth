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

class CDelay
{
private:
    float ti;  // time
    float fe;  // feedback
    float lf;  // LFO rate
    float la;  // LFO amount
    // 1.486 seconds buffer
    int bdelay[0x10000];
    // LFO
    COscillator osc;
    // buffer iterator
    unsigned short idelay; // here is the trick
    // auxiliar
    int  tempoant;
public:
    void Init(short *b);
    void Process(int *b, int size);
    void SetPar(char param, float value);
};
