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
    int bcomb1[TAMCOMB1];
    int bcomb2[TAMCOMB2];
    int bcomb3[TAMCOMB3];
    int bcomb4[TAMCOMB4];
    // allpasses buffers
    int ballp1[TAMALLP1];
    int ballp2[TAMALLP2];
    // buffers iterators
    int icomb1;
    int icomb2;
    int icomb3;
    int icomb4;
    int iallp1;
    int iallp2;
    // DC filter
    int in1;
    int ou0;
    // low-pass filter
    int in1l;
    int ou0l;
    int a0;
    int a1;
    int b1;
    float REVDAant;
    // other
    char state;
    // calculates low-pass coefs
    void CalcCoefLowPass(float frequencia);
public:
    void         Init();
    char         GetState(void);
    void         Process(int *b, int size);
    inline float Key2Frequency(float valor);
    void         SetPar(char param, float value);
};
