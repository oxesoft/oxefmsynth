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

#include <string.h>
#include <stdio.h>
#include <math.h>
#include "editor.h"
#include "mapper.h"

CEditor::CEditor(CSynthesizer *synthesizer)
{
    int ctlcount        = 0;
    this->synthesizer   = synthesizer;
    this->toolkit       = NULL;
    this->hostinterface = NULL;
    channel             = 0;

    // auxiliar vars
    int aX   = 0;
    int aY   = 0;

    // separation between the operators controls
    int sX   = 32*GUI_SCALE;
    int sX2  = sX/2;
    int sY   = 35*GUI_SCALE;

    // separation between the operators
    int oX   = 202*GUI_SCALE;
    int oY   = 94*GUI_SCALE;

    // separation between the half matrix controls
    int fX   = 20*GUI_SCALE;
    int fY   = 20*GUI_SCALE;

    // separation between the buttons
    int tX   = 44*GUI_SCALE;
    int tY   = 16*GUI_SCALE;

    // OPA coordinates
    int cX   = 17*GUI_SCALE;
    int cY   = 80*GUI_SCALE;

    // half-matrix coordinates
    int mX   = 419*GUI_SCALE;
    int mY   = 77*GUI_SCALE;

    // LFO section coordinates
    int lX   = 421*GUI_SCALE;
    int lY   = 338*GUI_SCALE;

    // pitch section coordinates
    int pX   = 421*GUI_SCALE;
    int pY   = 397*GUI_SCALE;

    // effects section coordinates
    int zX   = 527*GUI_SCALE;
    int zY   = 397*GUI_SCALE;

    // modulation control coordinates
    int dX   = 591*GUI_SCALE;
    int dY   = 338*GUI_SCALE;

    // channels control coordinates
    int gX   = 327*GUI_SCALE;
    int gY   = 25*GUI_SCALE;

    // reverb section corrdinates
    int rX   = 559*GUI_SCALE;
    int rY   = 23*GUI_SCALE;

    // delay section coordinates
    int eX   = 421*GUI_SCALE;
    int eY   = 23*GUI_SCALE;

    // buttons section coordinates
    int bX   = 147*GUI_SCALE;
    int bY   = 15*GUI_SCALE;

    // hq control coordinates
    int hX   = 603*GUI_SCALE;
    int hY   = 401*GUI_SCALE;

    // display
    lcd             = new CLcd     (BMP_CHARS, 25*GUI_SCALE, 23*GUI_SCALE);

    // OPA
    aX = cX;
    aY = cY;
    ctl[ctlcount++] = new CKey     (BMP_KEY,  0,10*GUI_SCALE,10*GUI_SCALE,"OPA On/Off",      synthesizer, channel,                   OPAON, aX         , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPA Waveform",    synthesizer, channel, VL_WAVEFORM,      OPAWF, aX+sX2     , aY      );
    ctl[ctlcount++] = new CKey     (BMP_KEY,  0,10*GUI_SCALE,10*GUI_SCALE,"OPA Keyb Track",  synthesizer, channel,                   OPAKT, aX+sX+sX2  , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPA Coarse Tune", synthesizer, channel, VL_COARSE_TUNE,   OPACT, aX+sX*2    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPA Fine Tune",   synthesizer, channel, VL_FINE_TUNE,     OPAFT, aX+sX*3    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPA VlSensivity", synthesizer, channel, VL_ZERO_TO_ONE,   OPAVS, aX+sX*4    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPA KeybScaling", synthesizer, channel, VL_MINUS1_2_PLUS1,OPAKS, aX+sX*5    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPA Delay Time",  synthesizer, channel, VL_PORTAMENTO,    OPADL, aX         , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPA Attack Time", synthesizer, channel, VL_TEMPO,         OPAAT, aX+sX      , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPA Decay Time",  synthesizer, channel, VL_TEMPO,         OPADE, aX+sX*2    , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPA Sustain Lvl", synthesizer, channel, VL_ZERO_TO_ONE,   OPASU, aX+sX*3    , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPA SustainTime", synthesizer, channel, VL_PORTAMENTO,    OPAST, aX+sX*4    , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPA ReleaseTime", synthesizer, channel, VL_TEMPO,         OPARE, aX+sX*5    , aY+sY   );

    // OPB
    aX = cX + oX;
    aY = cY;
    ctl[ctlcount++] = new CKey     (BMP_KEY,  0,10*GUI_SCALE,10*GUI_SCALE,"OPB On/Off",      synthesizer, channel,                   OPBON, aX         , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPB Waveform",    synthesizer, channel, VL_WAVEFORM,      OPBWF, aX+sX2     , aY      );
    ctl[ctlcount++] = new CKey     (BMP_KEY,  0,10*GUI_SCALE,10*GUI_SCALE,"OPB Keyb Track",  synthesizer, channel,                   OPBKT, aX+sX+sX2  , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPB Coarse Tune", synthesizer, channel, VL_COARSE_TUNE,   OPBCT, aX+sX*2    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPB Fine Tune",   synthesizer, channel, VL_FINE_TUNE,     OPBFT, aX+sX*3    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPB VlSensivity", synthesizer, channel, VL_ZERO_TO_ONE,   OPBVS, aX+sX*4    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPB KeybScaling", synthesizer, channel, VL_MINUS1_2_PLUS1,OPBKS, aX+sX*5    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPB Delay Time",  synthesizer, channel, VL_PORTAMENTO,    OPBDL, aX         , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPB Attack Time", synthesizer, channel, VL_TEMPO,         OPBAT, aX+sX      , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPB Decay Time",  synthesizer, channel, VL_TEMPO,         OPBDE, aX+sX*2    , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPB Sustain Lvl", synthesizer, channel, VL_ZERO_TO_ONE,   OPBSU, aX+sX*3    , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPB SustainTime", synthesizer, channel, VL_PORTAMENTO,    OPBST, aX+sX*4    , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPB ReleaseTime", synthesizer, channel, VL_TEMPO,         OPBRE, aX+sX*5    , aY+sY   );

    // OPC
    aX = cX;
    aY = cY + oY;
    ctl[ctlcount++] = new CKey     (BMP_KEY,  0,10*GUI_SCALE,10*GUI_SCALE,"OPC On/Off",      synthesizer, channel,                   OPCON, aX         , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPC Waveform",    synthesizer, channel, VL_WAVEFORM,      OPCWF, aX+sX2     , aY      );
    ctl[ctlcount++] = new CKey     (BMP_KEY,  0,10*GUI_SCALE,10*GUI_SCALE,"OPC Keyb Track",  synthesizer, channel,                   OPCKT, aX+sX+sX2  , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPC Coarse Tune", synthesizer, channel, VL_COARSE_TUNE,   OPCCT, aX+sX*2    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPC Fine Tune",   synthesizer, channel, VL_FINE_TUNE,     OPCFT, aX+sX*3    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPC VlSensivity", synthesizer, channel, VL_ZERO_TO_ONE,   OPCVS, aX+sX*4    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPC KeybScaling", synthesizer, channel, VL_MINUS1_2_PLUS1,OPCKS, aX+sX*5    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPC Delay Time",  synthesizer, channel, VL_PORTAMENTO,    OPCDL, aX         , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPC Attack Time", synthesizer, channel, VL_TEMPO,         OPCAT, aX+sX      , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPC Decay Time",  synthesizer, channel, VL_TEMPO,         OPCDE, aX+sX*2    , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPC Sustain Lvl", synthesizer, channel, VL_ZERO_TO_ONE,   OPCSU, aX+sX*3    , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPC SustainTime", synthesizer, channel, VL_PORTAMENTO,    OPCST, aX+sX*4    , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPC ReleaseTime", synthesizer, channel, VL_TEMPO,         OPCRE, aX+sX*5    , aY+sY   );

    // OPD
    aX = cX + oX;
    aY = cY + oY;
    ctl[ctlcount++] = new CKey     (BMP_KEY,  0,10*GUI_SCALE,10*GUI_SCALE,"OPD On/Off",      synthesizer, channel,                   OPDON, aX         , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPD Waveform",    synthesizer, channel, VL_WAVEFORM,      OPDWF, aX+sX2     , aY      );
    ctl[ctlcount++] = new CKey     (BMP_KEY,  0,10*GUI_SCALE,10*GUI_SCALE,"OPD Keyb Track",  synthesizer, channel,                   OPDKT, aX+sX+sX2  , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPD Coarse Tune", synthesizer, channel, VL_COARSE_TUNE,   OPDCT, aX+sX*2    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPD Fine Tune",   synthesizer, channel, VL_FINE_TUNE,     OPDFT, aX+sX*3    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPD VlSensivity", synthesizer, channel, VL_ZERO_TO_ONE,   OPDVS, aX+sX*4    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPD KeybScaling", synthesizer, channel, VL_MINUS1_2_PLUS1,OPDKS, aX+sX*5    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPD Delay Time",  synthesizer, channel, VL_PORTAMENTO,    OPDDL, aX         , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPD Attack Time", synthesizer, channel, VL_TEMPO,         OPDAT, aX+sX      , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPD Decay Time",  synthesizer, channel, VL_TEMPO,         OPDDE, aX+sX*2    , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPD Sustain Lvl", synthesizer, channel, VL_ZERO_TO_ONE,   OPDSU, aX+sX*3    , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPD SustainTime", synthesizer, channel, VL_PORTAMENTO,    OPDST, aX+sX*4    , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPD ReleaseTime", synthesizer, channel, VL_TEMPO,         OPDRE, aX+sX*5    , aY+sY   );

    // OPE
    aX = cX;
    aY = cY + oY * 2;
    ctl[ctlcount++] = new CKey     (BMP_KEY,  0,10*GUI_SCALE,10*GUI_SCALE,"OPE On/Off",      synthesizer, channel,                   OPEON, aX         , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPE Waveform",    synthesizer, channel, VL_WAVEFORM,      OPEWF, aX+sX2     , aY      );
    ctl[ctlcount++] = new CKey     (BMP_KEY,  0,10*GUI_SCALE,10*GUI_SCALE,"OPE Keyb Track",  synthesizer, channel,                   OPEKT, aX+sX+sX2  , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPE Coarse Tune", synthesizer, channel, VL_COARSE_TUNE,   OPECT, aX+sX*2    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPE Fine Tune",   synthesizer, channel, VL_FINE_TUNE,     OPEFT, aX+sX*3    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPE VlSensivity", synthesizer, channel, VL_ZERO_TO_ONE,   OPEVS, aX+sX*4    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPE KeybScaling", synthesizer, channel, VL_MINUS1_2_PLUS1,OPEKS, aX+sX*5    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPE Delay Time",  synthesizer, channel, VL_PORTAMENTO,    OPEDL, aX         , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPE Attack Time", synthesizer, channel, VL_TEMPO,         OPEAT, aX+sX      , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPE Decay Time",  synthesizer, channel, VL_TEMPO,         OPEDE, aX+sX*2    , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPE Sustain Lvl", synthesizer, channel, VL_ZERO_TO_ONE,   OPESU, aX+sX*3    , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPE SustainTime", synthesizer, channel, VL_PORTAMENTO,    OPEST, aX+sX*4    , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPE ReleaseTime", synthesizer, channel, VL_TEMPO,         OPERE, aX+sX*5    , aY+sY   );

    // OPF
    aX = cX + oX;
    aY = cY + oY * 2;
    ctl[ctlcount++] = new CKey     (BMP_KEY,  0,10*GUI_SCALE,10*GUI_SCALE,"OPF On/Off",      synthesizer, channel,                   OPFON, aX         , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPF Waveform",    synthesizer, channel, VL_WAVEFORM,      OPFWF, aX+sX2     , aY      );
    ctl[ctlcount++] = new CKey     (BMP_KEY,  0,10*GUI_SCALE,10*GUI_SCALE,"OPF Keyb Track",  synthesizer, channel,                   OPFKT, aX+sX+sX2  , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPF Coarse Tune", synthesizer, channel, VL_COARSE_TUNE,   OPFCT, aX+sX*2    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPF Fine Tune",   synthesizer, channel, VL_FINE_TUNE,     OPFFT, aX+sX*3    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPF VlSensivity", synthesizer, channel, VL_ZERO_TO_ONE,   OPFVS, aX+sX*4    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPF KeybScaling", synthesizer, channel, VL_MINUS1_2_PLUS1,OPFKS, aX+sX*5    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPF Delay Time",  synthesizer, channel, VL_PORTAMENTO,    OPFDL, aX         , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPF Attack Time", synthesizer, channel, VL_TEMPO,         OPFAT, aX+sX      , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPF Decay Time",  synthesizer, channel, VL_TEMPO,         OPFDE, aX+sX*2    , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPF Sustain Lvl", synthesizer, channel, VL_ZERO_TO_ONE,   OPFSU, aX+sX*3    , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPF SustainTime", synthesizer, channel, VL_PORTAMENTO,    OPFST, aX+sX*4    , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPF ReleaseTime", synthesizer, channel, VL_TEMPO,         OPFRE, aX+sX*5    , aY+sY   );

    // OPX
    aX = cX;
    aY = cY + oY * 3;
    ctl[ctlcount++] = new CKey     (BMP_KEY,  0,10*GUI_SCALE,10*GUI_SCALE,"OPX On/Off",      synthesizer, channel,                   OPXON, aX         , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPX Cuttof",      synthesizer, channel, VL_FILTER_CUTOFF, OPXCU, aX+sX2     , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPX Resonance",   synthesizer, channel, VL_ZERO_TO_ONE,   OPXRS, aX+sX+sX2  , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPX Amount",      synthesizer, channel, VL_ZERO_TO_ONE,   OPXAM, aX+sX*2+sX2, aY      );
    ctl[ctlcount++] = new CKey     (BMP_KEY,  0,10*GUI_SCALE,10*GUI_SCALE,"OPX Bypass",      synthesizer, channel,                   OPXBP, aX+sX*3+sX2, aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPX VlSensivity", synthesizer, channel, VL_ZERO_TO_ONE,   OPXVS, aX+sX*4    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPX KeybScaling", synthesizer, channel, VL_MINUS1_2_PLUS1,OPXKS, aX+sX*5    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPX Delay Time",  synthesizer, channel, VL_PORTAMENTO,    OPXDL, aX         , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPX Attack Time", synthesizer, channel, VL_TEMPO,         OPXAT, aX+sX      , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPX Decay Time",  synthesizer, channel, VL_TEMPO,         OPXDE, aX+sX*2    , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPX Sustain Lvl", synthesizer, channel, VL_ZERO_TO_ONE,   OPXSU, aX+sX*3    , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPX SustainTime", synthesizer, channel, VL_PORTAMENTO,    OPXST, aX+sX*4    , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPX ReleaseTime", synthesizer, channel, VL_TEMPO,         OPXRE, aX+sX*5    , aY+sY   );

    // OPZ
    aX = cX + oX;
    aY = cY + oY * 3;
    ctl[ctlcount++] = new CKey     (BMP_KEY,  0,10*GUI_SCALE,10*GUI_SCALE,"OPZ On/Off",      synthesizer, channel,                   OPZON, aX         , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPZ Cuttof",      synthesizer, channel, VL_FILTER_CUTOFF, OPZCU, aX+sX2     , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPZ Resonance",   synthesizer, channel, VL_ZERO_TO_ONE,   OPZRS, aX+sX+sX2  , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPZ Type",        synthesizer, channel, VL_FILTER,        OPZTY, aX+sX*2+sX2, aY      );
    ctl[ctlcount++] = new CKey     (BMP_KEY,  0,10*GUI_SCALE,10*GUI_SCALE,"OPZ Keyb Track",  synthesizer, channel,                   OPZKT, aX+sX*3+sX2, aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPZ VlSensivity", synthesizer, channel, VL_ZERO_TO_ONE,   OPZVS, aX+sX*4    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPZ KeybScaling", synthesizer, channel, VL_MINUS1_2_PLUS1,OPZKS, aX+sX*5    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPZ Delay Time",  synthesizer, channel, VL_PORTAMENTO,    OPZDL, aX         , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPZ Attack Time", synthesizer, channel, VL_TEMPO,         OPZAT, aX+sX      , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPZ Decay Time",  synthesizer, channel, VL_TEMPO,         OPZDE, aX+sX*2    , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPZ Sustain Lvl", synthesizer, channel, VL_ZERO_TO_ONE,   OPZSU, aX+sX*3    , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPZ SustainTime", synthesizer, channel, VL_PORTAMENTO,    OPZST, aX+sX*4    , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "OPZ ReleaseTime", synthesizer, channel, VL_TEMPO,         OPZRE, aX+sX*5    , aY+sY   );

    // Matriz
    aX = mX;
    aY = mY;
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  20*GUI_SCALE,   "OPA Self Mod",    synthesizer, channel, VL_MOD,           MAA,     aX       , aY      );
    ctl[ctlcount++] = new CKey     (BMP_OPS  ,0,20*GUI_SCALE,20*GUI_SCALE,"OPA On/Off",      synthesizer, channel,                   OPAON,   aX       , aY+fY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  20*GUI_SCALE,   "FM OPA->OPB",     synthesizer, channel, VL_MOD,           MAB,     aX       , aY+fY*2 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  20*GUI_SCALE,   "FM OPA->OPC",     synthesizer, channel, VL_MOD,           MAC,     aX       , aY+fY*3 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  20*GUI_SCALE,   "FM OPA->OPD",     synthesizer, channel, VL_MOD,           MAD,     aX       , aY+fY*4 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  20*GUI_SCALE,   "FM OPA->OPE",     synthesizer, channel, VL_MOD,           MAE,     aX       , aY+fY*5 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  20*GUI_SCALE,   "FM OPA->OPF",     synthesizer, channel, VL_MOD,           MAF,     aX       , aY+fY*6 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  20*GUI_SCALE,   "FM OPA->OPX",     synthesizer, channel, VL_MOD,           MAX,     aX       , aY+fY*7 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  20*GUI_SCALE,   "FM OPA->OPZ",     synthesizer, channel, VL_MOD,           MAZ,     aX       , aY+fY*8 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  20*GUI_SCALE,   "OPA Output",      synthesizer, channel, VL_MOD,           MAO,     aX       , aY+fY*9 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB3,  20*GUI_SCALE,   "OPA Pan",         synthesizer, channel, VL_PAN,           MAP,     aX       , aY+fY*10);

    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  20*GUI_SCALE,   "OPB Self Mod",    synthesizer, channel, VL_MOD,           MBB,     aX+fX    , aY+fY   );
    ctl[ctlcount++] = new CKey     (BMP_OPS  ,1,20*GUI_SCALE,20*GUI_SCALE,"OPB On/Off",      synthesizer, channel,                   OPBON,   aX+fX    , aY+fY*2 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  20*GUI_SCALE,   "FM OPB->OPC",     synthesizer, channel, VL_MOD,           MBC,     aX+fX    , aY+fY*3 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  20*GUI_SCALE,   "FM OPB->OPD",     synthesizer, channel, VL_MOD,           MBD,     aX+fX    , aY+fY*4 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  20*GUI_SCALE,   "FM OPB->OPE",     synthesizer, channel, VL_MOD,           MBE,     aX+fX    , aY+fY*5 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  20*GUI_SCALE,   "FM OPB->OPF",     synthesizer, channel, VL_MOD,           MBF,     aX+fX    , aY+fY*6 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  20*GUI_SCALE,   "FM OPB->OPX",     synthesizer, channel, VL_MOD,           MBX,     aX+fX    , aY+fY*7 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  20*GUI_SCALE,   "FM OPB->OPZ",     synthesizer, channel, VL_MOD,           MBZ,     aX+fX    , aY+fY*8 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  20*GUI_SCALE,   "OPB Output",      synthesizer, channel, VL_MOD,           MBO,     aX+fX    , aY+fY*9 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB3,  20*GUI_SCALE,   "OPB Pan",         synthesizer, channel, VL_PAN,           MBP,     aX+fX    , aY+fY*10);

    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  20*GUI_SCALE,   "OPC Self Mod",    synthesizer, channel, VL_MOD,           MCC,     aX+fX*2  , aY+fY*2 );
    ctl[ctlcount++] = new CKey     (BMP_OPS  ,2,20*GUI_SCALE,20*GUI_SCALE,"OPC On/Off",      synthesizer, channel,                   OPCON,   aX+fX*2  , aY+fY*3 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  20*GUI_SCALE,   "FM OPC->OPD",     synthesizer, channel, VL_MOD,           MCD,     aX+fX*2  , aY+fY*4 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  20*GUI_SCALE,   "FM OPC->OPE",     synthesizer, channel, VL_MOD,           MCE,     aX+fX*2  , aY+fY*5 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  20*GUI_SCALE,   "FM OPC->OPF",     synthesizer, channel, VL_MOD,           MCF,     aX+fX*2  , aY+fY*6 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  20*GUI_SCALE,   "FM OPC->OPX",     synthesizer, channel, VL_MOD,           MCX,     aX+fX*2  , aY+fY*7 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  20*GUI_SCALE,   "FM OPC->OPZ",     synthesizer, channel, VL_MOD,           MCZ,     aX+fX*2  , aY+fY*8 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  20*GUI_SCALE,   "OPC Output",      synthesizer, channel, VL_MOD,           MCO,     aX+fX*2  , aY+fY*9 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB3,  20*GUI_SCALE,   "OPC Pan",         synthesizer, channel, VL_PAN,           MCP,     aX+fX*2  , aY+fY*10);

    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  20*GUI_SCALE,   "OPD Self Mod",    synthesizer, channel, VL_MOD,           MDD,     aX+fX*3  , aY+fY*3 );
    ctl[ctlcount++] = new CKey     (BMP_OPS  ,3,20*GUI_SCALE,20*GUI_SCALE,"OPD On/Off",      synthesizer, channel,                   OPDON,   aX+fX*3  , aY+fY*4 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  20*GUI_SCALE,   "FM OPD->OPE",     synthesizer, channel, VL_MOD,           MDE,     aX+fX*3  , aY+fY*5 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  20*GUI_SCALE,   "FM OPD->OPF",     synthesizer, channel, VL_MOD,           MDF,     aX+fX*3  , aY+fY*6 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  20*GUI_SCALE,   "FM OPD->OPX",     synthesizer, channel, VL_MOD,           MDX,     aX+fX*3  , aY+fY*7 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  20*GUI_SCALE,   "FM OPD->OPZ",     synthesizer, channel, VL_MOD,           MDZ,     aX+fX*3  , aY+fY*8 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  20*GUI_SCALE,   "OPD Output",      synthesizer, channel, VL_MOD,           MDO,     aX+fX*3  , aY+fY*9 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB3,  20*GUI_SCALE,   "OPD Pan",         synthesizer, channel, VL_PAN,           MDP,     aX+fX*3  , aY+fY*10);

    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  20*GUI_SCALE,   "OPE Self Mod",    synthesizer, channel, VL_MOD,           MEE,     aX+fX*4  , aY+fY*4 );
    ctl[ctlcount++] = new CKey     (BMP_OPS  ,4,20*GUI_SCALE,20*GUI_SCALE,"OPE On/Off",      synthesizer, channel,                   OPEON,   aX+fX*4  , aY+fY*5 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  20*GUI_SCALE,   "FM OPE->OPF",     synthesizer, channel, VL_MOD,           MEF,     aX+fX*4  , aY+fY*6 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  20*GUI_SCALE,   "FM OPE->OPX",     synthesizer, channel, VL_MOD,           MEX,     aX+fX*4  , aY+fY*7 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  20*GUI_SCALE,   "FM OPE->OPZ",     synthesizer, channel, VL_MOD,           MEZ,     aX+fX*4  , aY+fY*8 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  20*GUI_SCALE,   "OPE Output",      synthesizer, channel, VL_MOD,           MEO,     aX+fX*4  , aY+fY*9 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB3,  20*GUI_SCALE,   "OPE Pan",         synthesizer, channel, VL_PAN,           MEP,     aX+fX*4  , aY+fY*10);

    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  20*GUI_SCALE,   "OPF Self Mod",    synthesizer, channel, VL_MOD,           MFF,     aX+fX*5  , aY+fY*5 );
    ctl[ctlcount++] = new CKey     (BMP_OPS  ,5,20*GUI_SCALE,20*GUI_SCALE,"OPF On/Off",      synthesizer, channel,                   OPFON,   aX+fX*5  , aY+fY*6 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  20*GUI_SCALE,   "FM OPF->OPX",     synthesizer, channel, VL_MOD,           MFX,     aX+fX*5  , aY+fY*7 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  20*GUI_SCALE,   "FM OPF->OPZ",     synthesizer, channel, VL_MOD,           MFZ,     aX+fX*5  , aY+fY*8 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  20*GUI_SCALE,   "OPF Output",      synthesizer, channel, VL_MOD,           MFO,     aX+fX*5  , aY+fY*9 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB3,  20*GUI_SCALE,   "OPF Pan",         synthesizer, channel, VL_PAN,           MFP,     aX+fX*5  , aY+fY*10);

    ctl[ctlcount++] = new CKey     (BMP_OPS  ,6,20*GUI_SCALE,20*GUI_SCALE,"OPX On/Off",      synthesizer, channel,                   OPXON,   aX+fX*6  , aY+fY*7 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  20*GUI_SCALE,   "FM OPX->OPZ",     synthesizer, channel, VL_MOD,           MXZ,     aX+fX*6  , aY+fY*8 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  20*GUI_SCALE,   "OPX Output",      synthesizer, channel, VL_MOD,           MXO,     aX+fX*6  , aY+fY*9 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB3,  20*GUI_SCALE,   "OPX Pan",         synthesizer, channel, VL_PAN,           MXP,     aX+fX*6  , aY+fY*10);

    ctl[ctlcount++] = new CKey     (BMP_OPS  ,7,20*GUI_SCALE,20*GUI_SCALE,"OPZ On/Off",      synthesizer, channel,                   OPZON,   aX+fX*7  , aY+fY*8 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  20*GUI_SCALE,   "OPZ Output",      synthesizer, channel, VL_MOD,           MZO,     aX+fX*7  , aY+fY*9 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB3,  20*GUI_SCALE,   "OPZ Pan",         synthesizer, channel, VL_PAN,           MZP,     aX+fX*7  , aY+fY*10);

    // Pitch
    aX = pX;
    aY = pY;
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "Portamento",      synthesizer, channel, VL_PORTAMENTO,    PORTA, aX         , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "Pitch Curve",     synthesizer, channel, VL_PITCH_CURVE,   PTCCU, aX+sX      , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "PitchCurveTime",  synthesizer, channel, VL_PORTAMENTO,    PTCTI, aX+sX*2    , aY      );

    // LFO
    aX = lX;
    aY = lY;
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "LFO Waveform",    synthesizer, channel, VL_WAVEFORM,      LFOWF, aX         , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "LFO Rate",        synthesizer, channel, VL_LFO_RATE,      LFORA, aX+sX      , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "LFO Depth",       synthesizer, channel, VL_MOD,           LFODE, aX+sX*2    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "LFO Delay",       synthesizer, channel, VL_PORTAMENTO,    LFODL, aX+sX*3    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "LFO Destination", synthesizer, channel, VL_LFO_DEST,      LFODS, aX+sX*4    , aY      );

    // Modulation
    aX = dX;
    aY = dY;
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "Mod Destination", synthesizer, channel, VL_MOD_DEST,      MDLDS, aX         , aY      );

    // Channels
    aX = gX;
    aY = gY;
    ctl[ctlcount++] = new CChannels(BMP_KEY,                             synthesizer, channel,                          aX         , aY      );

    // Reverb
    aX = rX;
    aY = rY;
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "Reverb Time",     synthesizer, channel, VL_ZERO_TO_ONE,   REVTI, aX         , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "Reverb Damp",     synthesizer, channel, VL_ZERO_TO_ONE,   REVDA, aX+sX      , aY      );

    // Delay
    aX = eX;
    aY = eY;
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "Delay Time",      synthesizer, channel, VL_ZERO_TO_ONE,   DLYTI, aX         , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "Delay Feedback",  synthesizer, channel, VL_ZERO_TO_ONE,   DLYFE, aX+sX      , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "Delay LFO Rate",  synthesizer, channel, VL_LFO_RATE,      DLYLF, aX+sX*2    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "Delay LFO Amt",   synthesizer, channel, VL_ZERO_TO_ONE,   DLYLA, aX+sX*3    , aY      );

    // Effects
    aX = zX;
    aY = zY;
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "Delay Level",     synthesizer, channel, VL_ZERO_TO_ONE,   DLYLV, aX         , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   25*GUI_SCALE,   "Reverb Level",    synthesizer, channel, VL_ZERO_TO_ONE,   RVBLV, aX+sX      , aY      );

    // Buttons
    aX = bX;
    aY = bY;
    ctl[ctlcount++] = new CButton  (BMP_BUTTONS, 0,                      synthesizer, channel, BT_BANK,                 aX         , aY      );
    ctl[ctlcount++] = new CButton  (BMP_BUTTONS, 1,                      synthesizer, channel, BT_PROGRAM,              aX         , aY+tY   );
    ctl[ctlcount++] = new CButton  (BMP_BUTTONS, 2,                      synthesizer, channel, BT_MINUS_10,             aX+tX      , aY      );
    ctl[ctlcount++] = new CButton  (BMP_BUTTONS, 3,                      synthesizer, channel, BT_MINUS_1,              aX+tX      , aY+tY   );
    ctl[ctlcount++] = new CButton  (BMP_BUTTONS, 4,                      synthesizer, channel, BT_PLUS_1,               aX+tX*2    , aY+tY   );
    ctl[ctlcount++] = new CButton  (BMP_BUTTONS, 5,                      synthesizer, channel, BT_PLUS_10,              aX+tX*2    , aY      );
    ctl[ctlcount++] = new CButton  (BMP_BUTTONS, 6,                      synthesizer, channel, BT_NAME,                 aX+tX*3    , aY      );
    ctl[ctlcount++] = new CButton  (BMP_BUTTONS, 7,                      synthesizer, channel, BT_STORE,                aX+tX*3    , aY+tY   );

    // HQ
    ctl[ctlcount++] = new CKey     (BMP_KEY,0,10*GUI_SCALE,10*GUI_SCALE,"High Quality",      synthesizer, channel,                   HQ   , hX         , hY      );

    changingControl = false;
    this->currentX  = -1;
    this->currentY  = -1;
}

CEditor::~CEditor()
{
    for (int i=0;i<GUI_CONTROLS;i++)
    {
        delete ctl[i];
    }
}

void CEditor::SetToolkit(CToolkit *toolkit)
{
    this->toolkit = toolkit;
    if (toolkit)
    {
        toolkit->CopyRect(0, 0, GUI_WIDTH, GUI_HEIGHT, BMP_BG, 0, 0);
    }
    lcd->SetToolkit(toolkit);
    for (int i = 0; i < GUI_CONTROLS; i++)
    {
        ctl[i]->SetToolkit(toolkit);
    }
    if (toolkit)
    {
        // show version
        char version_info[LCD_COLS + 1];
        int centeredPos = (LCD_COLS / 2) - (strlen(VERSION_STR) / 2);
        strncpy(version_info, "                ", LCD_COLS);
        strncpy(version_info + centeredPos, VERSION_STR, LCD_COLS - centeredPos);
        lcd->SetText(0, "  Oxe FM Synth  ");
        lcd->SetText(1, version_info);
    }
    changingControl = false;
}

void CEditor::SetHostInterface(CHostInterface *hostinterface)
{
    this->hostinterface = hostinterface;
    lcd->SetHostInterface(hostinterface);
    for (int i = 0; i < GUI_CONTROLS; i++)
    {
        ctl[i]->SetHostInterface(hostinterface);
        synthesizer->SetHostInterface(hostinterface);
    }
}

void CEditor::ProgramChanged()
{
    char str[TEXT_SIZE];
    snprintf(str, TEXT_SIZE, "Program %03i", synthesizer->GetNumProgr(channel));
    lcd->SetText(0,str);
    synthesizer->GetProgName(str, channel);
    lcd->SetText(1,str);
}

void CEditor::ProgramChangedWaiting()
{
    char str[TEXT_SIZE];
    snprintf(str, TEXT_SIZE, "Store current");
    lcd->SetText(0,str);
    snprintf(str, TEXT_SIZE, "conf in Prg%03i?",synthesizer->GetNumProgr(channel));
    lcd->SetText(1,str);
}

bool CEditor::OnChar(int cod)
{
    char str0[PG_NAME_SIZE + 1];
    char str1[PG_NAME_SIZE + 1];
    int  pos;
    if (!synthesizer->IsEditingName())
    {
        return false;
    }
    // edits the program name
    memset(str1, 0, sizeof(str1));
    synthesizer->GetProgName(str1, channel);
    for (pos = PG_NAME_SIZE - 1; pos>=0; pos--)
        if (str1[pos]>0)
            break;
    if (cod >= 32 && cod <= 126 && pos < PG_NAME_SIZE - 1)
    {
        str1[pos+1] = (char)cod;
        lcd->SetText(1,str1);
        synthesizer->SetProgName(str1, channel);
    }
    else if (cod == 8 || cod == 127)
    {
        if (pos >= 0)
        {
            str1[pos] = 0;
            lcd->SetText(1, str1);
            synthesizer->SetProgName(str1, channel);
        }
    }
    snprintf(str0, sizeof(str0), "Program %03i", synthesizer->GetNumProgr(channel));
    lcd->SetText(0, str0);
    return true;
}

void CEditor::OnLButtonDblClick(int x, int y)
{
    for (int i=0;i<GUI_CONTROLS;i++)
    {
        if (ctl[i]->IsMouseOver(x, y))
        {
            int index = ctl[i]->GetIndex();
            if (index >= 0)
            {
                synthesizer->SetDefault(channel, index);
                if (channel == 0)
                {
                    if (hostinterface)
                    {
                        hostinterface->ReceiveMessageFromPlugin(SET_PARAMETER, index, GetPar(index) * MAXPARVALUE);
                    }
                }
            }
            break;
        }
    }
}

void CEditor::OnLButtonDown(int x, int y)
{
    char str[TEXT_SIZE];
    char prevchannel = channel; // saves the current channel in case of the clicked control be CChannels
    for (int i=0;i<GUI_CONTROLS;i++)
    {
        if (ctl[i]->IsMouseOver(x, y))
        {
            ctl[i]->OnClick(x, y);
            synthesizer->SetEditingName(ctl[i]->GetType() == BT_NAME);
            if (synthesizer->GetStandBy(prevchannel))
            {
                synthesizer->SetStandBy(prevchannel, ctl[i]->GetType() != BT_NAME);
            }
            ctl[i]->GetName(str);
            lcd->SetText(0,str);
            CMapper::GetDisplayValue(this->synthesizer, this->channel, ctl[i]->GetIndex(), ctl[i]->GetType(), str);
            lcd->SetText(1,str);
            cID = i;
            if (ctl[i]->IsKnob() == true)
            {
                changingControl = true;
                if (toolkit)
                {
                    toolkit->StartMouseCapture();
                }
            }
            else if (channel == 0)
            {
                int index = ctl[i]->GetIndex();
                if (index >= 0)
                {
                    if (hostinterface)
                    {
                        hostinterface->ReceiveMessageFromPlugin(SET_PARAMETER, index, GetPar(index) * MAXPARVALUE);
                    }
                }
            }
            break;
        }
    }
    this->prevX = x;
    this->prevY = y;
}

void CEditor::OnLButtonUp()
{
    if (changingControl && channel == 0)
    {
        int index = ctl[cID]->GetIndex();
        if (index >= 0)
        {
            if (hostinterface)
            {
                hostinterface->ReceiveMessageFromPlugin(SET_PARAMETER, index, GetPar(index) * MAXPARVALUE);
            }
        }
    }
    changingControl = false;
    if (toolkit)
    {
        toolkit->StopMouseCapture();
    }
}

void CEditor::OnMouseMove(int x, int y)
{
    this->currentX = x;
    this->currentY = y;
    if (changingControl)
    {
        char str[TEXT_SIZE];
        int value = -(y - this->prevY) + (x - this->prevX);
        this->prevX = x;
        this->prevY = y;
        if (ctl[cID]->IncreaseValue(value) == true)
        {
            CMapper::GetDisplayValue(this->synthesizer, this->channel, ctl[cID]->GetIndex(), ctl[cID]->GetType(), str);
            lcd->SetText(1, str);
        }
    }
}

void CEditor::OnMouseWheel(int x, int y, int delta)
{
    char str[TEXT_SIZE];
    for (int i=0;i<GUI_CONTROLS;i++)
    {
        if (ctl[i]->IsMouseOver(x, y))
        {
            if (ctl[i]->IsKnob() == true)
            {
                ctl[i]->GetName(str);
                lcd->SetText(0,str);
                if (ctl[i]->IncreaseValue(delta) == true)
                {
                    CMapper::GetDisplayValue(this->synthesizer, this->channel, ctl[i]->GetIndex(), ctl[i]->GetType(), str);
                    lcd->SetText(1,str);
                    if (channel == 0)
                    {
                        int index = ctl[i]->GetIndex();
                        if (index >= 0)
                        {
                            if (hostinterface)
                            {
                                hostinterface->ReceiveMessageFromPlugin(SET_PARAMETER, index, lrintf(GetPar(index) * MAXPARVALUE));
                            }
                        }
                    }
                }
            }
            break;
        }
    }
}

void CEditor::Update()
{
    for (int i=0;i<GUI_CONTROLS;i++)
    {
        ctl[i]->Update();
    }
    if (synthesizer->HasChanges())
    {
        if (!synthesizer->GetBankMode())
        {
            if (!synthesizer->GetStandBy(channel))
            {
                ProgramChanged();
            }
            else
            {
                ProgramChangedWaiting();
            }
        }
        changingControl = false;
    }
    // hover feature
    if (currentX < 0 || currentY < 0 || changingControl)
    {
        return;
    }
    for (int i = 0; i < GUI_CONTROLS; i++)
    {
        if (ctl[i]->IsMouseOver(currentX, currentY) && cID != i)
        {
            cID = i;
            char str[TEXT_SIZE];
            ctl[i]->GetName(str);
            lcd->SetText(0,str);
            CMapper::GetDisplayValue(this->synthesizer, this->channel, ctl[i]->GetIndex(), ctl[i]->GetType(), str);
            lcd->SetText(1,str);
            break;
        }
    }
}

void CEditor::GetCoordinates(oxeCoords *coords)
{
    Update();
    coords += lcd->GetCoordinates(coords);
    for (int i = 0; i < GUI_CONTROLS; i++)
    {
        coords += ctl[i]->GetCoordinates(coords);
    }
}

void CEditor::SetPar(int index, float value)
{
    const char CHANNEL = 0;
    for (int i = 0; i < GUI_CONTROLS; i++)
    {
        if (ctl[i]->GetIndex() == index)
        {
            int type = ctl[i]->GetType();
            float fvalue = CMapper::IntValueToFloatValue(this->synthesizer, CHANNEL, index, type, lrintf(value * MAXPARVALUE));
            synthesizer->SetPar(CHANNEL, index, fvalue);
            break;
        }
    }
}

float CEditor::GetPar(int index)
{
    const char CHANNEL = 0;
    for (int i = 0; i < GUI_CONTROLS; i++)
    {
        if (ctl[i]->GetIndex() == index)
        {
            int type = ctl[i]->GetType();
            float fvalue = synthesizer->GetPar(CHANNEL, index);
            float vstvalue = (float)CMapper::FloatValueToIntValue(this->synthesizer, CHANNEL, index, type, fvalue);
            return vstvalue / MAXPARVALUE;
        }
    }
    return 0.f;
}

void CEditor::GetParLabel(int index, char* label)
{
    strncpy(label, "", TEXT_SIZE);
}

void CEditor::GetParDisplay(int index, char* text)
{
    const char CHANNEL = 0;
    for (int i = 0; i < GUI_CONTROLS; i++)
    {
        if (ctl[i]->GetIndex() == index)
        {
            int type = ctl[i]->GetType();
            CMapper::GetDisplayValue(this->synthesizer, CHANNEL, index, type, text);
            return;
        }
    }
    strncpy(text, "not found", TEXT_SIZE);
}

void CEditor::GetParName(int index, char* text)
{
    for (int i = 0; i < GUI_CONTROLS; i++)
    {
        if (ctl[i]->GetIndex() == index)
        {
            ctl[i]->GetName(text);
            return;
        }
    }
    strncpy(text, "not found", TEXT_SIZE);
}
