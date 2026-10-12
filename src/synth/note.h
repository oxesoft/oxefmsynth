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

class CNote
{
private:
    float samplerate;
    SProgram *program;  // the note's program
    CBuffers *buffers;  // the buffers
    float ptc;          // pitch weel (1.f == inert)
    float aft;          // aftertouch (0.0f to 1.0f)
    float lfodph;       // from MODULATION
    char  state;        // (on/off) the note state is defined by the operators's envelops except the Z one
    // portamento
    float freqnote;     // base frequency
    float current_freq; // current frequency of portamento
    float porta_factor; // multiplication factor
    int   porta_count;  // samples count
    // pitch curve
    float current_curv; // current pitch curve value
    float curv_factor;  // multiplication factor
    int   curv_count;   // samples count
    // pan and volume
    float lpan;         // (Range: -1.0f to 1.0f) Position between Left and Right channels
    float lvol;         // (Range:  0.0f to 1.0f) Volume
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
    
    void  SumMonoMono    (float *bInput , float *bNoteOut  , float volume  , int   size  , int offset            );
    void  SumMonoStereo  (float *bInput , float *bNoteOut  , float volume  , float pan   , int size  , int offset);
    void  PanVolStereo   (float *b      , float volume    , float pan     , int   size  , int offset            );
    
    float Scaling             (unsigned char key, float value);
    inline float VelSen       (float value, float vel);
    inline float Val2Mul      (float value);
    inline float Key2Frequency(char  value);
public:
    void  Init (SProgram *program, CBuffers *buf, unsigned char key, unsigned char previousKey, float velocity, float samplerate);
    void  SendEvent(char param, float value, int position);
    void  Process(float *b, int size, int position);
    char  GetState(void);
    void  UpdateProgram (void);
};
