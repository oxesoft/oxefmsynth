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

// buffers sizes
#define TAMCOMB1 1116 // 25ms
#define TAMCOMB2 1277 // 29ms
#define TAMCOMB3 1422 // 32ms
#define TAMCOMB4 1557 // 35ms
#define TAMALLP1 556  // 12ms
#define TAMALLP2 341  //  7ms

class CReverb
{
private:
    float sr;
    float ti;  // time
    float da;  // damp
    // combs buffers
    float bcomb1[TAMCOMB1];
    float bcomb2[TAMCOMB2];
    float bcomb3[TAMCOMB3];
    float bcomb4[TAMCOMB4];
    // allpasses buffers
    float ballp1[TAMALLP1];
    float ballp2[TAMALLP2];
    // buffers iterators
    int icomb1;
    int icomb2;
    int icomb3;
    int icomb4;
    int iallp1;
    int iallp2;
    // DC filter
    float in1;
    float ou0;
    // low-pass filter
    float in1l;
    float ou0l;
    float a0;
    float a1;
    float b1;
    float prev_REVDA;
    // other
    char state;
    // calculates low-pass coefs
    void CalcCoefLowPass(float frequency);
public:
    void         Init();
    char         GetState(void);
    void         Process(float *b, int size);
    inline float Key2Frequency(float value);
    void         SetPar(char param, float value);
};
