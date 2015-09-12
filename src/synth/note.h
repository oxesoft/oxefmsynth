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

class CNote
{
private:
    float samplerate;
    SProgram *program;  // the note's program
    CBuffers *buffers;  // the buffers
    float ptc;          // pitch weel (1.f == inert)
    float aft;          // aftertouch (0.f a 1.f)
    float lfodph;       // from MODULATION
    char  state;        // (on/off) the note state is defined by the operators's envelops except the Z one
    // portamento
    float freqnote;     // base frequency
    float freqAtual;    // current frequency of portamento
    float portaFator;   // multiplication factor
    int   portaCont;    // samples count
    // pitch curve
    float curvAtual;    // current pitch curve value
    float curvFator;    // multiplication factor
    int   curvCont;     // samples count
    // pan and volume
    int   lpan;         // (Faixa: -1.f a 1.f) Posicionamento entre os canais Esquerdo e Direito
    int   lvol;         // (Faixa:  0.f a 1.f) Volume
    // note position
    int   startPosition;// the sample number of the note begining
    int   lastpos;      // keeps the last position for accuracy of note off
    
    COscillator osc[6];
    CNoise      noise;
    CFilter     filter;
    COscillator lfoosc;
    CEnvelop    env    [MAXOPER + 1 /* LFO envelop */];
    char        opstate[MAXOPER + 1 /* LFO envelop */];
    // operators volumes
    float opAvol;
    float opBvol;
    float opCvol;
    float opDvol;
    float opEvol;
    float opFvol;
    float opXvol;
    float opZvol;
    
    void  SumMonoMono    (int *bInput , int *bNoteOut  , float  volume  , int   size  , int offset            );
    void  SumMonoStereo  (int *bInput , int *bNoteOut  , float  volume  , float pan   , int size  , int offset);
    void  PanVolStereo   (int *b      , int  volume    , int    pan     , int   size  , int offset            );
    int   enZant;
    
    float Scaling             (unsigned char tecla, float valor);
    inline float VelSen       (float valor, float vel);
    inline float Val2Mul      (float valor);
    inline float Key2Frequency(char  valor);
public:
    void  Init (SProgram *program, CBuffers *buf, unsigned char key, unsigned char previousKey, float velocity, float samplerate);
    void  SendEvent(char param, float value, int position);
    void  Process(int *b, int size, int position);
    char  GetState(void);
    void  UpdateProgram (void);
};
