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
#include <algorithm>
#include "editor.h"
#include "mapper.h"
#include "fonts.h"

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
    int sX   = 48;
    int sX2  = sX/2;
    int sY   = 52;

    // separation between the operators
    int oX   = 298;
    int oY   = 141;

    // separation between the half matrix controls
    int fX   = 30;
    int fY   = 30;

    // separation between the buttons
    int tX   = 66;
    int tY   = 24;

    // OPA coordinates
    int cX   = 20;
    int cY   = 115;

    // half-matrix coordinates
    int mX   = 628;
    int mY   = 115;

    // LFO section coordinates
    int lX   = 632;
    int lY   = 497;

    // pitch section coordinates
    int pX   = 632;
    int pY   = 595;

    // effects section coordinates
    int zX   = 790;
    int zY   = 595;

    // modulation control coordinates
    int dX   = 886;
    int dY   = 497;

    // channels control coordinates
    int gX   = 490;
    int gY   = 38;

    // reverb section corrdinates
    int rX   = 838;
    int rY   = 28;

    // delay section coordinates
    int eX   = 632;
    int eY   = 28;

    // buttons section coordinates
    int bX   = 220;
    int bY   = 22;

    // hq control coordinates
    int hX   = 905;
    int hY   = 602;

    // display
    lcd      = new CLcd (BMP_CHARS, 38, 22);

    // OPA
    aX = cX;
    aY = cY;
    ctl[ctlcount++] = new CKey     (BMP_KEY,  0,15,15,"OPA On/Off",      synthesizer, channel,                   OPAON, aX         , aY+10   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPA Waveform",    synthesizer, channel, VL_WAVEFORM,      OPAWF, aX+sX2     , aY      );
    ctl[ctlcount++] = new CKey     (BMP_KEY,  0,15,15,"OPA Keyb Track",  synthesizer, channel,                   OPAKT, aX+sX+sX2  , aY+10   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPA Coarse Tune", synthesizer, channel, VL_COARSE_TUNE,   OPACT, aX+sX*2    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPA Fine Tune",   synthesizer, channel, VL_FINE_TUNE,     OPAFT, aX+sX*3    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPA VlSensivity", synthesizer, channel, VL_ZERO_TO_ONE,   OPAVS, aX+sX*4    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPA KeybScaling", synthesizer, channel, VL_MINUS1_2_PLUS1,OPAKS, aX+sX*5    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPA Delay Time",  synthesizer, channel, VL_PORTAMENTO,    OPADL, aX         , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPA Attack Time", synthesizer, channel, VL_TEMPO,         OPAAT, aX+sX      , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPA Decay Time",  synthesizer, channel, VL_TEMPO,         OPADE, aX+sX*2    , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPA Sustain Lvl", synthesizer, channel, VL_ZERO_TO_ONE,   OPASU, aX+sX*3    , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPA SustainTime", synthesizer, channel, VL_PORTAMENTO,    OPAST, aX+sX*4    , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPA ReleaseTime", synthesizer, channel, VL_TEMPO,         OPARE, aX+sX*5    , aY+sY   );

    // OPB
    aX = cX + oX;
    aY = cY;
    ctl[ctlcount++] = new CKey     (BMP_KEY,  0,15,15,"OPB On/Off",      synthesizer, channel,                   OPBON, aX         , aY+10   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPB Waveform",    synthesizer, channel, VL_WAVEFORM,      OPBWF, aX+sX2     , aY      );
    ctl[ctlcount++] = new CKey     (BMP_KEY,  0,15,15,"OPB Keyb Track",  synthesizer, channel,                   OPBKT, aX+sX+sX2  , aY+10   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPB Coarse Tune", synthesizer, channel, VL_COARSE_TUNE,   OPBCT, aX+sX*2    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPB Fine Tune",   synthesizer, channel, VL_FINE_TUNE,     OPBFT, aX+sX*3    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPB VlSensivity", synthesizer, channel, VL_ZERO_TO_ONE,   OPBVS, aX+sX*4    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPB KeybScaling", synthesizer, channel, VL_MINUS1_2_PLUS1,OPBKS, aX+sX*5    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPB Delay Time",  synthesizer, channel, VL_PORTAMENTO,    OPBDL, aX         , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPB Attack Time", synthesizer, channel, VL_TEMPO,         OPBAT, aX+sX      , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPB Decay Time",  synthesizer, channel, VL_TEMPO,         OPBDE, aX+sX*2    , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPB Sustain Lvl", synthesizer, channel, VL_ZERO_TO_ONE,   OPBSU, aX+sX*3    , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPB SustainTime", synthesizer, channel, VL_PORTAMENTO,    OPBST, aX+sX*4    , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPB ReleaseTime", synthesizer, channel, VL_TEMPO,         OPBRE, aX+sX*5    , aY+sY   );

    // OPC
    aX = cX;
    aY = cY + oY;
    ctl[ctlcount++] = new CKey     (BMP_KEY,  0,15,15,"OPC On/Off",      synthesizer, channel,                   OPCON, aX         , aY+10   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPC Waveform",    synthesizer, channel, VL_WAVEFORM,      OPCWF, aX+sX2     , aY      );
    ctl[ctlcount++] = new CKey     (BMP_KEY,  0,15,15,"OPC Keyb Track",  synthesizer, channel,                   OPCKT, aX+sX+sX2  , aY+10   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPC Coarse Tune", synthesizer, channel, VL_COARSE_TUNE,   OPCCT, aX+sX*2    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPC Fine Tune",   synthesizer, channel, VL_FINE_TUNE,     OPCFT, aX+sX*3    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPC VlSensivity", synthesizer, channel, VL_ZERO_TO_ONE,   OPCVS, aX+sX*4    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPC KeybScaling", synthesizer, channel, VL_MINUS1_2_PLUS1,OPCKS, aX+sX*5    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPC Delay Time",  synthesizer, channel, VL_PORTAMENTO,    OPCDL, aX         , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPC Attack Time", synthesizer, channel, VL_TEMPO,         OPCAT, aX+sX      , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPC Decay Time",  synthesizer, channel, VL_TEMPO,         OPCDE, aX+sX*2    , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPC Sustain Lvl", synthesizer, channel, VL_ZERO_TO_ONE,   OPCSU, aX+sX*3    , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPC SustainTime", synthesizer, channel, VL_PORTAMENTO,    OPCST, aX+sX*4    , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPC ReleaseTime", synthesizer, channel, VL_TEMPO,         OPCRE, aX+sX*5    , aY+sY   );

    // OPD
    aX = cX + oX;
    aY = cY + oY;
    ctl[ctlcount++] = new CKey     (BMP_KEY,  0,15,15,"OPD On/Off",      synthesizer, channel,                   OPDON, aX         , aY+10   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPD Waveform",    synthesizer, channel, VL_WAVEFORM,      OPDWF, aX+sX2     , aY      );
    ctl[ctlcount++] = new CKey     (BMP_KEY,  0,15,15,"OPD Keyb Track",  synthesizer, channel,                   OPDKT, aX+sX+sX2  , aY+10   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPD Coarse Tune", synthesizer, channel, VL_COARSE_TUNE,   OPDCT, aX+sX*2    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPD Fine Tune",   synthesizer, channel, VL_FINE_TUNE,     OPDFT, aX+sX*3    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPD VlSensivity", synthesizer, channel, VL_ZERO_TO_ONE,   OPDVS, aX+sX*4    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPD KeybScaling", synthesizer, channel, VL_MINUS1_2_PLUS1,OPDKS, aX+sX*5    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPD Delay Time",  synthesizer, channel, VL_PORTAMENTO,    OPDDL, aX         , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPD Attack Time", synthesizer, channel, VL_TEMPO,         OPDAT, aX+sX      , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPD Decay Time",  synthesizer, channel, VL_TEMPO,         OPDDE, aX+sX*2    , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPD Sustain Lvl", synthesizer, channel, VL_ZERO_TO_ONE,   OPDSU, aX+sX*3    , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPD SustainTime", synthesizer, channel, VL_PORTAMENTO,    OPDST, aX+sX*4    , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPD ReleaseTime", synthesizer, channel, VL_TEMPO,         OPDRE, aX+sX*5    , aY+sY   );

    // OPE
    aX = cX;
    aY = cY + oY * 2;
    ctl[ctlcount++] = new CKey     (BMP_KEY,  0,15,15,"OPE On/Off",      synthesizer, channel,                   OPEON, aX         , aY+10   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPE Waveform",    synthesizer, channel, VL_WAVEFORM,      OPEWF, aX+sX2     , aY      );
    ctl[ctlcount++] = new CKey     (BMP_KEY,  0,15,15,"OPE Keyb Track",  synthesizer, channel,                   OPEKT, aX+sX+sX2  , aY+10   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPE Coarse Tune", synthesizer, channel, VL_COARSE_TUNE,   OPECT, aX+sX*2    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPE Fine Tune",   synthesizer, channel, VL_FINE_TUNE,     OPEFT, aX+sX*3    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPE VlSensivity", synthesizer, channel, VL_ZERO_TO_ONE,   OPEVS, aX+sX*4    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPE KeybScaling", synthesizer, channel, VL_MINUS1_2_PLUS1,OPEKS, aX+sX*5    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPE Delay Time",  synthesizer, channel, VL_PORTAMENTO,    OPEDL, aX         , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPE Attack Time", synthesizer, channel, VL_TEMPO,         OPEAT, aX+sX      , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPE Decay Time",  synthesizer, channel, VL_TEMPO,         OPEDE, aX+sX*2    , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPE Sustain Lvl", synthesizer, channel, VL_ZERO_TO_ONE,   OPESU, aX+sX*3    , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPE SustainTime", synthesizer, channel, VL_PORTAMENTO,    OPEST, aX+sX*4    , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPE ReleaseTime", synthesizer, channel, VL_TEMPO,         OPERE, aX+sX*5    , aY+sY   );

    // OPF
    aX = cX + oX;
    aY = cY + oY * 2;
    ctl[ctlcount++] = new CKey     (BMP_KEY,  0,15,15,"OPF On/Off",      synthesizer, channel,                   OPFON, aX         , aY+10   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPF Waveform",    synthesizer, channel, VL_WAVEFORM,      OPFWF, aX+sX2     , aY      );
    ctl[ctlcount++] = new CKey     (BMP_KEY,  0,15,15,"OPF Keyb Track",  synthesizer, channel,                   OPFKT, aX+sX+sX2  , aY+10   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPF Coarse Tune", synthesizer, channel, VL_COARSE_TUNE,   OPFCT, aX+sX*2    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPF Fine Tune",   synthesizer, channel, VL_FINE_TUNE,     OPFFT, aX+sX*3    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPF VlSensivity", synthesizer, channel, VL_ZERO_TO_ONE,   OPFVS, aX+sX*4    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPF KeybScaling", synthesizer, channel, VL_MINUS1_2_PLUS1,OPFKS, aX+sX*5    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPF Delay Time",  synthesizer, channel, VL_PORTAMENTO,    OPFDL, aX         , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPF Attack Time", synthesizer, channel, VL_TEMPO,         OPFAT, aX+sX      , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPF Decay Time",  synthesizer, channel, VL_TEMPO,         OPFDE, aX+sX*2    , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPF Sustain Lvl", synthesizer, channel, VL_ZERO_TO_ONE,   OPFSU, aX+sX*3    , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPF SustainTime", synthesizer, channel, VL_PORTAMENTO,    OPFST, aX+sX*4    , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPF ReleaseTime", synthesizer, channel, VL_TEMPO,         OPFRE, aX+sX*5    , aY+sY   );

    // OPX
    aX = cX;
    aY = cY + oY * 3;
    ctl[ctlcount++] = new CKey     (BMP_KEY,  0,15,15,"OPX On/Off",      synthesizer, channel,                   OPXON, aX         , aY+10   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPX Cuttof",      synthesizer, channel, VL_FILTER_CUTOFF, OPXCU, aX+sX2     , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPX Resonance",   synthesizer, channel, VL_ZERO_TO_ONE,   OPXRS, aX+sX+sX2  , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPX Amount",      synthesizer, channel, VL_ZERO_TO_ONE,   OPXAM, aX+sX*2+sX2, aY      );
    ctl[ctlcount++] = new CKey     (BMP_KEY,  0,15,15,"OPX Bypass",      synthesizer, channel,                   OPXBP, aX+sX*3+sX2, aY+10   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPX VlSensivity", synthesizer, channel, VL_ZERO_TO_ONE,   OPXVS, aX+sX*4    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPX KeybScaling", synthesizer, channel, VL_MINUS1_2_PLUS1,OPXKS, aX+sX*5    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPX Delay Time",  synthesizer, channel, VL_PORTAMENTO,    OPXDL, aX         , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPX Attack Time", synthesizer, channel, VL_TEMPO,         OPXAT, aX+sX      , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPX Decay Time",  synthesizer, channel, VL_TEMPO,         OPXDE, aX+sX*2    , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPX Sustain Lvl", synthesizer, channel, VL_ZERO_TO_ONE,   OPXSU, aX+sX*3    , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPX SustainTime", synthesizer, channel, VL_PORTAMENTO,    OPXST, aX+sX*4    , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPX ReleaseTime", synthesizer, channel, VL_TEMPO,         OPXRE, aX+sX*5    , aY+sY   );

    // OPZ
    aX = cX + oX;
    aY = cY + oY * 3;
    ctl[ctlcount++] = new CKey     (BMP_KEY,  0,15,15,"OPZ On/Off",      synthesizer, channel,                   OPZON, aX         , aY+10   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPZ Cuttof",      synthesizer, channel, VL_FILTER_CUTOFF, OPZCU, aX+sX2     , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPZ Resonance",   synthesizer, channel, VL_ZERO_TO_ONE,   OPZRS, aX+sX+sX2  , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPZ Type",        synthesizer, channel, VL_FILTER,        OPZTY, aX+sX*2+sX2, aY      );
    ctl[ctlcount++] = new CKey     (BMP_KEY,  0,15,15,"OPZ Keyb Track",  synthesizer, channel,                   OPZKT, aX+sX*3+sX2, aY+10   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPZ VlSensivity", synthesizer, channel, VL_ZERO_TO_ONE,   OPZVS, aX+sX*4    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPZ KeybScaling", synthesizer, channel, VL_MINUS1_2_PLUS1,OPZKS, aX+sX*5    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPZ Delay Time",  synthesizer, channel, VL_PORTAMENTO,    OPZDL, aX         , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPZ Attack Time", synthesizer, channel, VL_TEMPO,         OPZAT, aX+sX      , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPZ Decay Time",  synthesizer, channel, VL_TEMPO,         OPZDE, aX+sX*2    , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPZ Sustain Lvl", synthesizer, channel, VL_ZERO_TO_ONE,   OPZSU, aX+sX*3    , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPZ SustainTime", synthesizer, channel, VL_PORTAMENTO,    OPZST, aX+sX*4    , aY+sY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "OPZ ReleaseTime", synthesizer, channel, VL_TEMPO,         OPZRE, aX+sX*5    , aY+sY   );

    // Matriz
    aX = mX;
    aY = mY;
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  28,   "OPA Self Mod",    synthesizer, channel, VL_MOD,           MAA,     aX       , aY      );
    ctl[ctlcount++] = new CKey     (BMP_OPS  ,0,28,28,"OPA On/Off",      synthesizer, channel,                   OPAON,   aX       , aY+fY   );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  28,   "FM OPA->OPB",     synthesizer, channel, VL_MOD,           MAB,     aX       , aY+fY*2 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  28,   "FM OPA->OPC",     synthesizer, channel, VL_MOD,           MAC,     aX       , aY+fY*3 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  28,   "FM OPA->OPD",     synthesizer, channel, VL_MOD,           MAD,     aX       , aY+fY*4 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  28,   "FM OPA->OPE",     synthesizer, channel, VL_MOD,           MAE,     aX       , aY+fY*5 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  28,   "FM OPA->OPF",     synthesizer, channel, VL_MOD,           MAF,     aX       , aY+fY*6 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  28,   "FM OPA->OPX",     synthesizer, channel, VL_MOD,           MAX,     aX       , aY+fY*7 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  28,   "FM OPA->OPZ",     synthesizer, channel, VL_MOD,           MAZ,     aX       , aY+fY*8 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  28,   "OPA Output",      synthesizer, channel, VL_MOD,           MAO,     aX       , aY+fY*9 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB3,  28,   "OPA Pan",         synthesizer, channel, VL_PAN,           MAP,     aX       , aY+fY*10);

    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  28,   "OPB Self Mod",    synthesizer, channel, VL_MOD,           MBB,     aX+fX    , aY+fY   );
    ctl[ctlcount++] = new CKey     (BMP_OPS  ,1,28,28,"OPB On/Off",      synthesizer, channel,                   OPBON,   aX+fX    , aY+fY*2 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  28,   "FM OPB->OPC",     synthesizer, channel, VL_MOD,           MBC,     aX+fX    , aY+fY*3 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  28,   "FM OPB->OPD",     synthesizer, channel, VL_MOD,           MBD,     aX+fX    , aY+fY*4 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  28,   "FM OPB->OPE",     synthesizer, channel, VL_MOD,           MBE,     aX+fX    , aY+fY*5 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  28,   "FM OPB->OPF",     synthesizer, channel, VL_MOD,           MBF,     aX+fX    , aY+fY*6 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  28,   "FM OPB->OPX",     synthesizer, channel, VL_MOD,           MBX,     aX+fX    , aY+fY*7 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  28,   "FM OPB->OPZ",     synthesizer, channel, VL_MOD,           MBZ,     aX+fX    , aY+fY*8 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  28,   "OPB Output",      synthesizer, channel, VL_MOD,           MBO,     aX+fX    , aY+fY*9 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB3,  28,   "OPB Pan",         synthesizer, channel, VL_PAN,           MBP,     aX+fX    , aY+fY*10);

    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  28,   "OPC Self Mod",    synthesizer, channel, VL_MOD,           MCC,     aX+fX*2  , aY+fY*2 );
    ctl[ctlcount++] = new CKey     (BMP_OPS  ,2,28,28,"OPC On/Off",      synthesizer, channel,                   OPCON,   aX+fX*2  , aY+fY*3 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  28,   "FM OPC->OPD",     synthesizer, channel, VL_MOD,           MCD,     aX+fX*2  , aY+fY*4 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  28,   "FM OPC->OPE",     synthesizer, channel, VL_MOD,           MCE,     aX+fX*2  , aY+fY*5 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  28,   "FM OPC->OPF",     synthesizer, channel, VL_MOD,           MCF,     aX+fX*2  , aY+fY*6 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  28,   "FM OPC->OPX",     synthesizer, channel, VL_MOD,           MCX,     aX+fX*2  , aY+fY*7 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  28,   "FM OPC->OPZ",     synthesizer, channel, VL_MOD,           MCZ,     aX+fX*2  , aY+fY*8 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  28,   "OPC Output",      synthesizer, channel, VL_MOD,           MCO,     aX+fX*2  , aY+fY*9 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB3,  28,   "OPC Pan",         synthesizer, channel, VL_PAN,           MCP,     aX+fX*2  , aY+fY*10);

    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  28,   "OPD Self Mod",    synthesizer, channel, VL_MOD,           MDD,     aX+fX*3  , aY+fY*3 );
    ctl[ctlcount++] = new CKey     (BMP_OPS  ,3,28,28,"OPD On/Off",      synthesizer, channel,                   OPDON,   aX+fX*3  , aY+fY*4 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  28,   "FM OPD->OPE",     synthesizer, channel, VL_MOD,           MDE,     aX+fX*3  , aY+fY*5 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  28,   "FM OPD->OPF",     synthesizer, channel, VL_MOD,           MDF,     aX+fX*3  , aY+fY*6 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  28,   "FM OPD->OPX",     synthesizer, channel, VL_MOD,           MDX,     aX+fX*3  , aY+fY*7 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  28,   "FM OPD->OPZ",     synthesizer, channel, VL_MOD,           MDZ,     aX+fX*3  , aY+fY*8 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  28,   "OPD Output",      synthesizer, channel, VL_MOD,           MDO,     aX+fX*3  , aY+fY*9 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB3,  28,   "OPD Pan",         synthesizer, channel, VL_PAN,           MDP,     aX+fX*3  , aY+fY*10);

    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  28,   "OPE Self Mod",    synthesizer, channel, VL_MOD,           MEE,     aX+fX*4  , aY+fY*4 );
    ctl[ctlcount++] = new CKey     (BMP_OPS  ,4,28,28,"OPE On/Off",      synthesizer, channel,                   OPEON,   aX+fX*4  , aY+fY*5 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  28,   "FM OPE->OPF",     synthesizer, channel, VL_MOD,           MEF,     aX+fX*4  , aY+fY*6 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  28,   "FM OPE->OPX",     synthesizer, channel, VL_MOD,           MEX,     aX+fX*4  , aY+fY*7 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  28,   "FM OPE->OPZ",     synthesizer, channel, VL_MOD,           MEZ,     aX+fX*4  , aY+fY*8 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  28,   "OPE Output",      synthesizer, channel, VL_MOD,           MEO,     aX+fX*4  , aY+fY*9 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB3,  28,   "OPE Pan",         synthesizer, channel, VL_PAN,           MEP,     aX+fX*4  , aY+fY*10);

    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  28,   "OPF Self Mod",    synthesizer, channel, VL_MOD,           MFF,     aX+fX*5  , aY+fY*5 );
    ctl[ctlcount++] = new CKey     (BMP_OPS  ,5,28,28,"OPF On/Off",      synthesizer, channel,                   OPFON,   aX+fX*5  , aY+fY*6 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  28,   "FM OPF->OPX",     synthesizer, channel, VL_MOD,           MFX,     aX+fX*5  , aY+fY*7 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  28,   "FM OPF->OPZ",     synthesizer, channel, VL_MOD,           MFZ,     aX+fX*5  , aY+fY*8 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  28,   "OPF Output",      synthesizer, channel, VL_MOD,           MFO,     aX+fX*5  , aY+fY*9 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB3,  28,   "OPF Pan",         synthesizer, channel, VL_PAN,           MFP,     aX+fX*5  , aY+fY*10);

    ctl[ctlcount++] = new CKey     (BMP_OPS  ,6,28,28,"OPX On/Off",      synthesizer, channel,                   OPXON,   aX+fX*6  , aY+fY*7 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  28,   "FM OPX->OPZ",     synthesizer, channel, VL_MOD,           MXZ,     aX+fX*6  , aY+fY*8 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  28,   "OPX Output",      synthesizer, channel, VL_MOD,           MXO,     aX+fX*6  , aY+fY*9 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB3,  28,   "OPX Pan",         synthesizer, channel, VL_PAN,           MXP,     aX+fX*6  , aY+fY*10);

    ctl[ctlcount++] = new CKey     (BMP_OPS  ,7,28,28,"OPZ On/Off",      synthesizer, channel,                   OPZON,   aX+fX*7  , aY+fY*8 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB2,  28,   "OPZ Output",      synthesizer, channel, VL_MOD,           MZO,     aX+fX*7  , aY+fY*9 );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB3,  28,   "OPZ Pan",         synthesizer, channel, VL_PAN,           MZP,     aX+fX*7  , aY+fY*10);

    // Pitch
    aX = pX;
    aY = pY;
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "Portamento",      synthesizer, channel, VL_PORTAMENTO,    PORTA, aX         , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "Pitch Curve",     synthesizer, channel, VL_PITCH_CURVE,   PTCCU, aX+sX      , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "PitchCurveTime",  synthesizer, channel, VL_PORTAMENTO,    PTCTI, aX+sX*2    , aY      );

    // LFO
    aX = lX;
    aY = lY;
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "LFO Waveform",    synthesizer, channel, VL_WAVEFORM,      LFOWF, aX         , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "LFO Rate",        synthesizer, channel, VL_LFO_RATE,      LFORA, aX+sX      , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "LFO Depth",       synthesizer, channel, VL_MOD,           LFODE, aX+sX*2    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "LFO Delay",       synthesizer, channel, VL_PORTAMENTO,    LFODL, aX+sX*3    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "LFO Destination", synthesizer, channel, VL_LFO_DEST,      LFODS, aX+sX*4    , aY      );

    // Modulation
    aX = dX;
    aY = dY;
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "Mod Destination", synthesizer, channel, VL_MOD_DEST,      MDLDS, aX         , aY      );

    // Channels
    aX = gX;
    aY = gY;
    ctl[ctlcount++] = new CChannels(BMP_KEY,                             synthesizer, channel,                          aX         , aY      );

    // Reverb
    aX = rX;
    aY = rY;
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "Reverb Time",     synthesizer, channel, VL_ZERO_TO_ONE,   REVTI, aX         , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "Reverb Damp",     synthesizer, channel, VL_ZERO_TO_ONE,   REVDA, aX+sX      , aY      );

    // Delay
    aX = eX;
    aY = eY;
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "Delay Time",      synthesizer, channel, VL_ZERO_TO_ONE,   DLYTI, aX         , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "Delay Feedback",  synthesizer, channel, VL_ZERO_TO_ONE,   DLYFE, aX+sX      , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "Delay LFO Rate",  synthesizer, channel, VL_LFO_RATE,      DLYLF, aX+sX*2    , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "Delay LFO Amt",   synthesizer, channel, VL_ZERO_TO_ONE,   DLYLA, aX+sX*3    , aY      );

    // Effects
    aX = zX;
    aY = zY;
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "Delay Level",     synthesizer, channel, VL_ZERO_TO_ONE,   DLYLV, aX         , aY      );
    ctl[ctlcount++] = new CKnob    (BMP_KNOB,   36,   "Reverb Level",    synthesizer, channel, VL_ZERO_TO_ONE,   RVBLV, aX+sX      , aY      );

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
    ctl[ctlcount++] = new CKey     (BMP_KEY,0,15,15,"High Quality",      synthesizer, channel,                   HQ   , hX         , hY      );

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
        toolkit->Invalidate();
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
    if (toolkit)
    {
        toolkit->Invalidate();
    }
}

void CEditor::ProgramChangedWaiting()
{
    char str[TEXT_SIZE];
    snprintf(str, TEXT_SIZE, "Store current");
    lcd->SetText(0,str);
    snprintf(str, TEXT_SIZE, "conf in Prg%03i?",synthesizer->GetNumProgr(channel));
    lcd->SetText(1,str);
    if (toolkit)
    {
        toolkit->Invalidate();
    }
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

struct OpCard {
    float x, y, w, h;
    const char* tag;
    const char* title;
    BLRgba32 accent;
};

static const OpCard opCards[8] = {
    {  12.0f,  86.0f, 292.0f, 132.0f, "A", "OPERATOR A",                BLRgba32(0x00, 0xf0, 0xff) },
    { 310.0f,  86.0f, 292.0f, 132.0f, "B", "OPERATOR B",                BLRgba32(0x00, 0xe6, 0x76) },
    {  12.0f, 227.0f, 292.0f, 132.0f, "C", "OPERATOR C",                BLRgba32(0xff, 0xd6, 0x00) },
    { 310.0f, 227.0f, 292.0f, 132.0f, "D", "OPERATOR D",                BLRgba32(0xff, 0x91, 0x00) },
    {  12.0f, 368.0f, 292.0f, 132.0f, "E", "OPERATOR E",                BLRgba32(0xb3, 0x88, 0xff) },
    { 310.0f, 368.0f, 292.0f, 132.0f, "F", "OPERATOR F",                BLRgba32(0xff, 0x40, 0x81) },
    {  12.0f, 509.0f, 292.0f, 136.0f, "X", "OPERATOR X (FILTER/NOISE)", BLRgba32(0x40, 0xc4, 0xff) },
    { 310.0f, 509.0f, 292.0f, 136.0f, "Z", "OPERATOR Z (FILTER)",       BLRgba32(0xee, 0xf2, 0xf6) }
};

static int GetEnvelopeOpIndexForParam(int par)
{
    if ((par >= OPADL && par <= OPARE) || par == OPAON) return 0;
    if ((par >= OPBDL && par <= OPBRE) || par == OPBON) return 1;
    if ((par >= OPCDL && par <= OPCRE) || par == OPCON) return 2;
    if ((par >= OPDDL && par <= OPDRE) || par == OPDON) return 3;
    if ((par >= OPEDL && par <= OPERE) || par == OPEON) return 4;
    if ((par >= OPFDL && par <= OPFRE) || par == OPFON) return 5;
    if ((par >= OPXDL && par <= OPXRE) || par == OPXON) return 6;
    if ((par >= OPZDL && par <= OPZRE) || par == OPZON) return 7;
    return -1;
}

static inline void InvalidateEnvelopeIfParam(CToolkit *toolkit, int par)
{
    if (!toolkit) return;
    int op = GetEnvelopeOpIndexForParam(par);
    if (op >= 0)
    {
        toolkit->InvalidateRect((int)opCards[op].x + 170, (int)opCards[op].y + 4, 116, 23);
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
            InvalidateEnvelopeIfParam(toolkit, ctl[i]->GetIndex());
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
    if (prevchannel != channel)
    {
        Update();
        if (toolkit)
        {
            toolkit->Invalidate();
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
            InvalidateEnvelopeIfParam(toolkit, ctl[cID]->GetIndex());
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
                    InvalidateEnvelopeIfParam(toolkit, ctl[i]->GetIndex());
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
        if (toolkit)
        {
            toolkit->Invalidate();
        }
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
            ctl[i]->Update();
            InvalidateEnvelopeIfParam(toolkit, index);
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
            float normValue = (float)CMapper::FloatValueToIntValue(this->synthesizer, CHANNEL, index, type, fvalue);
            return normValue / MAXPARVALUE;
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

void CEditor::DrawPanelCards(BLContext &ctx, const BLFont &fontSmall, const BLFont &fontNormal, const BLFont &fontHeader)
{
    // 1. Top Header Card
    ctx.fill_round_rect(BLRoundRect(12.0, 10.0, 926.0, 68.0, 6.0), BLRgba32(0x13, 0x18, 0x22));
    ctx.set_stroke_width(1.0);
    ctx.stroke_round_rect(BLRoundRect(12.0, 10.0, 926.0, 68.0, 6.0), BLRgba32(0x24, 0x2d, 0x3d));

    // Dividers in top card
    ctx.stroke_line(478.0, 16.0, 478.0, 72.0, BLRgba32(0x1e, 0x27, 0x35));
    ctx.stroke_line(618.0, 16.0, 618.0, 72.0, BLRgba32(0x1e, 0x27, 0x35));
    ctx.stroke_line(826.0, 16.0, 826.0, 72.0, BLRgba32(0x1e, 0x27, 0x35));

    // Labels in top card
    if (fontSmall.is_valid())
    {
        ctx.fill_utf8_text(BLPoint(490.0, 22.0), fontSmall, "MIDI CHANNELS", SIZE_MAX, BLRgba32(0x60, 0x72, 0x88));
        ctx.fill_utf8_text(BLPoint(632.0, 22.0), fontSmall, "DELAY", SIZE_MAX, BLRgba32(0x60, 0x72, 0x88));
        ctx.fill_utf8_text(BLPoint(838.0, 22.0), fontSmall, "REVERB", SIZE_MAX, BLRgba32(0x60, 0x72, 0x88));
    }

    // 2. Operator Cards (A through F, X, Z)
    for (int i = 0; i < 8; i++)
    {
        float cx = opCards[i].x;
        float cy = opCards[i].y;
        float cw = opCards[i].w;
        float ch = opCards[i].h;

        // Card base
        ctx.fill_round_rect(BLRoundRect(cx, cy, cw, ch, 6.0), BLRgba32(0x13, 0x18, 0x22));
        ctx.set_stroke_width(1.0);
        ctx.stroke_round_rect(BLRoundRect(cx, cy, cw, ch, 6.0), BLRgba32(0x22, 0x2a, 0x39));

        // Tag pill
        ctx.fill_round_rect(BLRoundRect(cx + 8.0f, cy + 7.0f, 16.0f, 16.0f, 3.0f), BLRgba32(opCards[i].accent.r(), opCards[i].accent.g(), opCards[i].accent.b(), 0x30));
        ctx.stroke_round_rect(BLRoundRect(cx + 8.0f, cy + 7.0f, 16.0f, 16.0f, 3.0f), opCards[i].accent);
        if (fontSmall.is_valid())
        {
            ctx.fill_utf8_text(BLPoint(cx + 12.5f, cy + 19.0f), fontSmall, opCards[i].tag, 1, opCards[i].accent);
            ctx.fill_utf8_text(BLPoint(cx + 30.0f, cy + 19.0f), fontSmall, opCards[i].title, SIZE_MAX, BLRgba32(0x8a, 0x9b, 0xaf));
        }
    }

    // 3. Matrix Card
    ctx.fill_round_rect(BLRoundRect(612.0, 86.0, 326.0, 370.0, 6.0), BLRgba32(0x13, 0x18, 0x22));
    ctx.set_stroke_width(1.0);
    ctx.stroke_round_rect(BLRoundRect(612.0, 86.0, 326.0, 370.0, 6.0), BLRgba32(0x22, 0x2a, 0x39));

    // 4. LFO Card
    ctx.fill_round_rect(BLRoundRect(612.0, 464.0, 326.0, 84.0, 6.0), BLRgba32(0x13, 0x18, 0x22));
    ctx.set_stroke_width(1.0);
    ctx.stroke_round_rect(BLRoundRect(612.0, 464.0, 326.0, 84.0, 6.0), BLRgba32(0x22, 0x2a, 0x39));
    ctx.stroke_line(872.0, 470.0, 872.0, 542.0, BLRgba32(0x1e, 0x27, 0x35));
    if (fontSmall.is_valid())
    {
        ctx.fill_utf8_text(BLPoint(626.0, 480.0), fontSmall, "LFO", SIZE_MAX, BLRgba32(0x60, 0x72, 0x88));
        ctx.fill_utf8_text(BLPoint(882.0, 480.0), fontSmall, "MOD", SIZE_MAX, BLRgba32(0x60, 0x72, 0x88));
    }

    // 5. Pitch & Effects Card
    ctx.fill_round_rect(BLRoundRect(612.0, 556.0, 326.0, 89.0, 6.0), BLRgba32(0x13, 0x18, 0x22));
    ctx.set_stroke_width(1.0);
    ctx.stroke_round_rect(BLRoundRect(612.0, 556.0, 326.0, 89.0, 6.0), BLRgba32(0x22, 0x2a, 0x39));
    ctx.stroke_line(776.0, 562.0, 776.0, 638.0, BLRgba32(0x1e, 0x27, 0x35));
    ctx.stroke_line(886.0, 562.0, 886.0, 638.0, BLRgba32(0x1e, 0x27, 0x35));
    if (fontSmall.is_valid())
    {
        ctx.fill_utf8_text(BLPoint(626.0, 572.0), fontSmall, "PITCH", SIZE_MAX, BLRgba32(0x60, 0x72, 0x88));
        ctx.fill_utf8_text(BLPoint(788.0, 572.0), fontSmall, "EFFECTS", SIZE_MAX, BLRgba32(0x60, 0x72, 0x88));
        ctx.fill_utf8_text(BLPoint(898.0, 572.0), fontSmall, "HQ", SIZE_MAX, BLRgba32(0x60, 0x72, 0x88));
    }
}

void CEditor::DrawOperatorEnvelope(BLContext &ctx, int opIndex, float x, float y, float w, float h, const BLFont &fontSmall, const BLRgba32 &accentColor)
{
    ctx.fill_round_rect(BLRoundRect(x, y, w, h, 3.0), BLRgba32(0x0e, 0x12, 0x1a));
    ctx.set_stroke_width(0.8);
    ctx.stroke_round_rect(BLRoundRect(x, y, w, h, 3.0), BLRgba32(0x20, 0x28, 0x36));

    if (!synthesizer) return;

    int base = opIndex * 13;
    float dl = synthesizer->GetPar(channel, base + 7);
    float at = synthesizer->GetPar(channel, base + 8);
    float de = synthesizer->GetPar(channel, base + 9);
    float su = synthesizer->GetPar(channel, base + 10);
    float st = synthesizer->GetPar(channel, base + 11);
    float re = synthesizer->GetPar(channel, base + 12);
    float onOff = synthesizer->GetPar(channel, base);

    // Normalize envelope parameter values matching CMapper
    float dl_norm = fmaxf(0.0f, fminf(1.0f, powf(fmaxf(0.0f, dl) / MAXTIMEENV, 1.0f / 3.0f)));

    float at_raw = powf(fmaxf(0.0f, at) / MAXTIMEENV, 1.0f / 3.0f);
    float at_norm = fmaxf(0.0f, fminf(1.0f, (at_raw - (2.0f / 102.0f)) / (1.0f - (2.0f / 102.0f))));

    float de_raw = powf(fmaxf(0.0f, de) / MAXTIMEENV, 1.0f / 3.0f);
    float de_norm = fmaxf(0.0f, fminf(1.0f, (de_raw - (2.0f / 102.0f)) / (1.0f - (2.0f / 102.0f))));

    float su_norm = fmaxf(0.0f, fminf(1.0f, su));

    float st_norm = fmaxf(0.0f, fminf(1.0f, powf(fmaxf(0.0f, st) / MAXTIMEENV, 1.0f / 3.0f)));

    float re_raw = powf(fmaxf(0.0f, re) / MAXTIMEENV, 1.0f / 3.0f);
    float re_norm = fmaxf(0.0f, fminf(1.0f, (re_raw - (2.0f / 102.0f)) / (1.0f - (2.0f / 102.0f))));

    float availW = w - 4.0f;
    float bottomY = y + h - 2.0f;
    float topY    = y + 2.0f;
    float height  = bottomY - topY;

    // 1. Delay: exactly 0 width when dl_norm == 0, up to 22% of availW at max
    float dl_w = dl_norm * (availW * 0.22f);

    // 2. Attack: exactly 0.6px (almost vertical) when at_norm == 0, up to 35% of availW at max
    float min_at_w = 0.6f;
    float max_at_w = availW * 0.35f;
    float at_w = min_at_w + at_norm * (max_at_w - min_at_w);

    // 3. Remaining width distributed across Decay, Sustain Time, and Release
    float remainingW = availW - dl_w - at_w;

    float de_weight = 0.06f + de_norm * 0.44f;
    float st_weight = 0.05f + st_norm * 0.30f;
    float re_weight = 0.06f + re_norm * 0.44f;
    float total_rem_weight = de_weight + st_weight + re_weight;

    float de_w = (de_weight / total_rem_weight) * remainingW;
    float st_w = (st_weight / total_rem_weight) * remainingW;

    float p0x = x + 2.0f;
    float p0y = bottomY;

    // Start of attack is p1x. When dl == 0, dl_w == 0, so p1x == p0x!
    float p1x = p0x + dl_w;
    float p1y = bottomY;

    float p2x = p1x + at_w;
    float p2y = topY;

    float p3x = p2x + de_w;
    float p3y = bottomY - su_norm * height;

    float p4x = p3x + st_w;
    float p4y = p3y;

    float p5x = p0x + availW;
    float p5y = bottomY;

    BLPath path;
    path.move_to(p0x, p0y);
    if (dl_w > 0.001f)
    {
        path.line_to(p1x, p1y);
    }
    path.line_to(p2x, p2y);
    path.line_to(p3x, p3y);
    path.line_to(p4x, p4y);
    path.line_to(p5x, p5y);

    BLPath fillPath = path;
    fillPath.line_to(p5x, bottomY);
    fillPath.line_to(p0x, bottomY);
    fillPath.close();

    bool isOpActive = (onOff > 0.5f);
    BLRgba32 strokeColor = isOpActive ? accentColor : BLRgba32(0x45, 0x54, 0x68);
    BLRgba32 fillColor = isOpActive ? BLRgba32(accentColor.r(), accentColor.g(), accentColor.b(), 0x24)
                                    : BLRgba32(0x30, 0x3d, 0x50, 0x18);

    ctx.fill_path(fillPath, fillColor);
    ctx.set_stroke_width(1.3);
    ctx.stroke_path(path, strokeColor);
}

void CEditor::DrawFilterCurve(BLContext &ctx, float x, float y, float w, float h, const BLFont &fontSmall, const BLRgba32 &accentColor)
{
    ctx.fill_round_rect(BLRoundRect(x, y, w, h, 3.0), BLRgba32(0x0e, 0x12, 0x1a));
    ctx.set_stroke_width(0.8);
    ctx.stroke_round_rect(BLRoundRect(x, y, w, h, 3.0), BLRgba32(0x20, 0x28, 0x36));

    if (!synthesizer) return;

    float cutoff = synthesizer->GetPar(channel, OPZCU);
    float reso   = synthesizer->GetPar(channel, OPZRS);
    float onOff  = synthesizer->GetPar(channel, OPZON);

    float normCutoff = fmaxf(0.05f, fminf(0.95f, cutoff / MAXFREQFLT));
    float normReso   = fmaxf(0.0f, fminf(1.0f, reso));

    float bottomY = y + h - 2.0f;
    float topY    = y + 2.0f;
    float height  = bottomY - topY;

    float cx = x + 2.0f + normCutoff * (w - 4.0f);
    float cy = topY + (1.0f - normReso * 0.7f) * (height * 0.5f);

    BLPath path;
    path.move_to(x + 2.0f, topY + height * 0.35f);
    path.line_to(cx - 8.0f, topY + height * 0.35f);
    path.quad_to(cx, cy, cx + 10.0f, bottomY);
    path.line_to(x + w - 2.0f, bottomY);

    BLPath fillPath = path;
    fillPath.line_to(x + w - 2.0f, bottomY);
    fillPath.line_to(x + 2.0f, bottomY);
    fillPath.close();

    bool isOpActive = (onOff > 0.5f);
    BLRgba32 strokeColor = isOpActive ? accentColor : BLRgba32(0x45, 0x54, 0x68);
    BLRgba32 fillColor = isOpActive ? BLRgba32(accentColor.r(), accentColor.g(), accentColor.b(), 0x24)
                                    : BLRgba32(0x30, 0x3d, 0x50, 0x18);

    ctx.fill_path(fillPath, fillColor);
    ctx.set_stroke_width(1.3);
    ctx.stroke_path(path, strokeColor);
}

void CEditor::DrawMatrixDecorations(BLContext &ctx, const BLFont &fontSmall, const BLFont &fontNormal)
{
    float mX = 628.0f;
    float mY = 115.0f;
    float fX = 30.0f;
    float fY = 30.0f;

    // Diagonal "Modulation half-Matrix" aligned with the angle of self-modulation knobs
    if (fontSmall.is_valid())
    {
        ctx.save();
        double angle = std::atan2(fY, fX); // 45.0 degrees
        double uX = std::cos(angle);
        double uY = std::sin(angle);
        double nX =  std::sin(angle);
        double nY = -std::cos(angle);

        double cX = mX + 80.0;
        double cY = mY + 80.0;
        double t = 10.0;
        double d = 24.0;
        double posX = cX + t * uX + d * nX;
        double posY = cY + t * uY + d * nY;

        ctx.translate(posX, posY);
        ctx.rotate(angle);
        BLFont fontDiag = CFontManager::GetFont(16.0f);
        if (!fontDiag.is_valid()) fontDiag = fontSmall;
        ctx.fill_utf8_text(BLPoint(0, 0), fontDiag, "Modulation half-Matrix", SIZE_MAX, BLRgba32(0x8a, 0x9b, 0xaf));
        ctx.restore();
    }

    float outY = mY + fY * 9.0f + 18.0f;
    float panY = mY + fY * 10.0f + 18.0f;

    if (fontSmall.is_valid())
    {
        ctx.fill_utf8_text(BLPoint(mX + fX * 8.0f + 6.0f, outY), fontSmall, "OUTPUT", SIZE_MAX, BLRgba32(0xff, 0x90, 0x00));
        ctx.fill_utf8_text(BLPoint(mX + fX * 8.0f + 6.0f, panY), fontSmall, "PAN", SIZE_MAX, BLRgba32(0x00, 0xe5, 0xff));
    }

    ctx.set_stroke_width(0.7);
    BLRgba32 gridColor(0x1e, 0x29, 0x38, 0x55);
    for (int col = 0; col < 8; col++)
    {
        float x = mX + col * fX + 14.0f;
        ctx.stroke_line(x, mY + col * fY + 14.0f, x, mY + 10.0f * fY + 14.0f, gridColor);
    }
}

// ============================================================================
// Procedural 2004 "Ô X e" Logo for Blend2D (Calibrated to bg.bmp 148x110)
// Rendered as discrete polygons with mathematical circular arc segments
// Tuned with Oxe FM Synth Vector Studio
// ============================================================================
void CEditor::DrawOxeLogo(BLContext &ctx, float x, float y, float scale)
{
    ctx.save();
    ctx.translate(x, y);
    ctx.scale(scale, scale);

    // ------------------------------------------------------------------------
    // Shape 1: O uppper
    // ------------------------------------------------------------------------
    BLPath poly1;
    // Outer Contour
    poly1.move_to(9.0f, 47.0f);
    poly1.line_to(16.5f, 47.0f);
    poly1.arc_to(35.0f, 42.5f, 19.0f, 19.0f, 2.9046f, 3.6156f, false); // Arc to (53.5, 47.0)
    poly1.line_to(60.5f, 47.0f);
    poly1.arc_to(34.8f, 43.1f, 26.1f, 26.1f, 0.1522f, -3.4459f, false); // Arc to (9.0, 47.0)
    poly1.close();
    ctx.fill_path(poly1, BLRgba32(0x16, 0x92, 0x2c));
    ctx.set_stroke_width(0.5f);
    ctx.stroke_path(poly1, BLRgba32(0x00, 0x00, 0x00));

    // ------------------------------------------------------------------------
    // Shape 2: O lower
    // ------------------------------------------------------------------------
    BLPath poly2;
    // Outer Contour
    poly2.move_to(9.5f, 50.0f);
    poly2.line_to(17.0f, 50.0f);
    poly2.arc_to(35.0f, 44.8f, 18.8f, 18.8f, 2.8578f, -2.5740f, false); // Arc to (53.0, 50.0)
    poly2.line_to(60.0f, 50.0f);
    poly2.arc_to(34.8f, 44.1f, 25.9f, 25.9f, 0.2310f, 2.6796f, false); // Arc to (9.5, 50.0)
    poly2.close();
    ctx.fill_path(poly2, BLRgba32(0x16, 0x92, 0x2c));
    ctx.set_stroke_width(0.5f);
    ctx.stroke_path(poly2, BLRgba32(0x00, 0x00, 0x00));

    // ------------------------------------------------------------------------
    // Shape 3: Hat
    // ------------------------------------------------------------------------
    BLPath poly3;
    // Outer Contour
    poly3.move_to(21.0f, 15.5f);
    poly3.line_to(34.0f, 8.5f);
    poly3.line_to(46.5f, 15.5f);
    poly3.close();
    ctx.fill_path(poly3, BLRgba32(0x58, 0x87, 0x12, 0xe6));
    ctx.set_stroke_width(0.5f);
    ctx.stroke_path(poly3, BLRgba32(0x00, 0x00, 0x00));

    // ------------------------------------------------------------------------
    // Shape 4: X
    // ------------------------------------------------------------------------
    BLPath poly4;
    // Outer Contour
    poly4.move_to(47.5f, 9.5f);
    poly4.line_to(56.0f, 7.0f);
    poly4.line_to(70.5f, 37.5f);
    poly4.line_to(87.0f, 9.0f);
    poly4.line_to(77.0f, 45.0f);
    poly4.line_to(100.0f, 71.5f);
    poly4.line_to(89.5f, 79.5f);
    poly4.line_to(72.0f, 54.0f);
    poly4.line_to(47.0f, 91.5f);
    poly4.line_to(67.0f, 42.5f);
    poly4.close();
    ctx.fill_path(poly4, BLRgba32(0x87, 0xc6, 0x27, 0xe6));
    ctx.set_stroke_width(0.5f);
    ctx.stroke_path(poly4, BLRgba32(0x00, 0x00, 0x00));

    // ------------------------------------------------------------------------
    // Shape 5: E upper
    // ------------------------------------------------------------------------
    BLPath poly5;
    // Outer Contour
    poly5.move_to(87.5f, 46.5f);
    poly5.line_to(138.5f, 46.5f);
    poly5.arc_to(113.0f, 42.8f, 25.8f, 25.8f, 0.1452f, -3.4320f, false); // Arc to (87.5, 46.5)
    poly5.close();
    // Hole 1 Contour
    poly5.move_to(94.0f, 40.5f);
    poly5.line_to(131.5f, 40.5f);
    poly5.arc_to(112.8f, 41.8f, 18.8f, 18.8f, -0.0689f, -3.0037f, false); // Arc to (94.0, 40.5)
    poly5.close();
    ctx.set_fill_rule(BL_FILL_RULE_EVEN_ODD);
    ctx.fill_path(poly5, BLRgba32(0x76, 0x70, 0x06));
    ctx.set_stroke_width(0.5f);
    ctx.stroke_path(poly5, BLRgba32(0x00, 0x00, 0x00));

    // ------------------------------------------------------------------------
    // Shape 6: E lower
    // ------------------------------------------------------------------------
    BLPath poly6;
    // Outer Contour
    poly6.move_to(87.5f, 50.0f);
    poly6.line_to(95.0f, 50.0f);
    poly6.arc_to(113.3f, 44.4f, 19.1f, 19.1f, 2.8446f, -2.5476f, false); // Arc to (131.5, 50.0)
    poly6.line_to(138.0f, 50.0f);
    poly6.arc_to(112.8f, 44.1f, 25.9f, 25.9f, 0.2310f, 2.6796f, false); // Arc to (87.5, 50.0)
    poly6.close();
    ctx.fill_path(poly6, BLRgba32(0x76, 0x70, 0x06, 0xe6));
    ctx.set_stroke_width(0.5f);
    ctx.stroke_path(poly6, BLRgba32(0x00, 0x00, 0x00));

    // ------------------------------------------------------------------------
    // FM Synth
    // ------------------------------------------------------------------------
    BLFont fontLogo = CFontManager::GetFontBold(20.0f);
    if (!fontLogo.is_valid())
    {
        fontLogo = CFontManager::GetFont(20.0f);
    }
    if (fontLogo.is_valid())
    {
        ctx.fill_utf8_text(BLPoint(58.0f, 95.0f), fontLogo, "FM Synth", SIZE_MAX, BLRgba32(0xbe, 0xe4, 0x00));
    }

    ctx.restore();
}

void CEditor::RenderBackgroundCache(int targetW, int targetH)
{
    bgCache.create(targetW, targetH, BL_FORMAT_PRGB32);
    BLContext bgCtx(bgCache);
    bgCtx.clear_all();

    double scaleX = (double)targetW / (double)GUI_WIDTH;
    double scaleY = (double)targetH / (double)GUI_HEIGHT;
    bgCtx.scale(scaleX, scaleY);

    BLFont fontSmall  = CFontManager::GetFont(9.0f);
    BLFont fontNormal = CFontManager::GetFont(11.0f);
    BLFont fontHeader = CFontManager::GetFont(13.0f);

    // Global background
    bgCtx.fill_box(0.0, 0.0, GUI_WIDTH, GUI_HEIGHT, BLRgba32(0x0b, 0x0e, 0x14));

    // Panel cards (static base, borders, headers)
    DrawPanelCards(bgCtx, fontSmall, fontNormal, fontHeader);

    // Matrix decorations & branding logo
    DrawMatrixDecorations(bgCtx, fontSmall, fontNormal);
    DrawOxeLogo(bgCtx, 746.0f, 94.0f, 1.2f);

    bgCtx.end();
}

static inline bool RectsIntersect(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
    return !(x1 + w1 <= x2 || x2 + w2 <= x1 || y1 + h1 <= y2 || y2 + h2 <= y1);
}

void CEditor::Paint(BLContext &ctx, int dirtyX, int dirtyY, int dirtyW, int dirtyH)
{
    int targetW = (int)ctx.target_width();
    int targetH = (int)ctx.target_height();
    if (bgCache.width() != targetW || bgCache.height() != targetH)
    {
        RenderBackgroundCache(targetW, targetH);
        dirtyW = 0; // force full repaint
    }

    double scaleX = (double)targetW / (double)GUI_WIDTH;
    double scaleY = (double)targetH / (double)GUI_HEIGHT;

    bool isFullRepaint = (dirtyW <= 0 || dirtyH <= 0 || (dirtyX <= 0 && dirtyY <= 0 && dirtyW >= GUI_WIDTH && dirtyH >= GUI_HEIGHT));

    if (isFullRepaint)
    {
        // Fast 1:1 background blit
        ctx.save();
        ctx.reset_transform();
        ctx.blit_image(BLPointI(0, 0), bgCache);
        ctx.restore();
    }
    else
    {
        // Restore background from cache for dirty region only
        int px = (int)floor(dirtyX * scaleX);
        int py = (int)floor(dirtyY * scaleY);
        int pw = (int)ceil(dirtyW * scaleX) + 1;
        int ph = (int)ceil(dirtyH * scaleY) + 1;

        if (px < 0) { pw += px; px = 0; }
        if (py < 0) { ph += py; py = 0; }
        if (px + pw > targetW) pw = targetW - px;
        if (py + ph > targetH) ph = targetH - py;

        if (pw > 0 && ph > 0)
        {
            ctx.save();
            ctx.reset_transform();
            ctx.blit_image(BLPointI(px, py), bgCache, BLRectI(px, py, pw, ph));
            ctx.restore();
        }

        ctx.save();
        ctx.clip_to_rect(BLRect((double)dirtyX, (double)dirtyY, (double)dirtyW, (double)dirtyH));
    }

    BLFont fontSmall  = CFontManager::GetFont(9.0f);
    BLFont fontNormal = CFontManager::GetFont(11.0f);

    // Dynamic envelope curves in card headers
    for (int i = 0; i < 8; i++)
    {
        int ex = (int)opCards[i].x + 170;
        int ey = (int)opCards[i].y + 4;
        int ew = 116;
        int eh = 23;
        if (isFullRepaint || RectsIntersect(ex, ey, ew, eh, dirtyX, dirtyY, dirtyW, dirtyH))
        {
            DrawOperatorEnvelope(ctx, i, opCards[i].x + 172.0f, opCards[i].y + 6.0f, 112.0f, 19.0f, fontSmall, opCards[i].accent);
        }
    }

    // Paint LCD
    if (lcd)
    {
        int lx, ly, lw, lh;
        lcd->GetRepaintBounds(lx, ly, lw, lh);
        if (isFullRepaint || RectsIntersect(lx, ly, lw, lh, dirtyX, dirtyY, dirtyW, dirtyH))
        {
            lcd->Paint(ctx, fontSmall, fontNormal);
        }
    }

    // Paint controls
    for (int i = 0; i < GUI_CONTROLS; i++)
    {
        if (ctl[i])
        {
            int cx, cy, cw, ch;
            ctl[i]->GetRepaintBounds(cx, cy, cw, ch);
            if (isFullRepaint || RectsIntersect(cx, cy, cw, ch, dirtyX, dirtyY, dirtyW, dirtyH))
            {
                ctl[i]->Paint(ctx, fontSmall, fontNormal);
            }
        }
    }

    if (!isFullRepaint)
    {
        ctx.restore();
    }
}

