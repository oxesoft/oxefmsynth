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

class CNoise
{
private:
    float sr;
    float fr;  // central frequency
    float fr1; // previous central frequency
    float re;  // resonance
    float re1; // previous resonance
    // coeficients
    float b0a0;
    float b2a0;
    float a1a0;
    float a2a0;
    // in/out history
    float ou1;
    float ou2;
    float in1;
    float in2;
    // noise volume
    float am;
    int bp;
    // calculates the filter coefs
    void  CalcCoef(double const frequency, double const q);
public:
    void Init();
    void Process(float *b, int size, int offset);
    void SetPar(char param, float value);
};
