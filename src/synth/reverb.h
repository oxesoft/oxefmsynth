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
    double sr;
    double ti;  // time
    double da;  // damp
    // combs buffers
    long bcomb1[TAMCOMB1];
    long bcomb2[TAMCOMB2];
    long bcomb3[TAMCOMB3];
    long bcomb4[TAMCOMB4];
    // allpasses buffers
    long ballp1[TAMALLP1];
    long ballp2[TAMALLP2];
    // buffers iterators
    long icomb1;
    long icomb2;
    long icomb3;
    long icomb4;
    long iallp1;
    long iallp2;
    // DC filter
    long in1;
    long ou0;
    // low-pass filter
    long in1l;
    long ou0l;
    long a0;
    long a1;
    long b1;
    double REVDAant;
    // other
    char state;
    // calculates low-pass coefs
    void CalcCoefLowPass(double frequencia);
public:
    void          Init();
    char          GetState(void);
    void          Process(int *b, int size);
    inline double Key2Frequency(double valor);
    void          SetPar(char param, double value);
};
