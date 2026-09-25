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
#include <stdlib.h>
#include <math.h>
#include "synthesizer.h"
#include "control.h"
#include "knob.h"
#include "mapper.h"

static const char* GetShortLabel(const char* fullName)
{
    if (strstr(fullName, "Attack Time")) return "ATT";
    if (strstr(fullName, "Decay Time")) return "DECAY";
    if (strstr(fullName, "Sustain Lvl")) return "SUST";
    if (strstr(fullName, "SustainTime")) return "TIME";
    if (strstr(fullName, "ReleaseTime") || strstr(fullName, "Release Time")) return "REL";
    if (strstr(fullName, "Cutoff")) return "CUTOFF";
    if (strstr(fullName, "Resonance")) return "RESO";
    if (strstr(fullName, "Amount")) return "AMNT";
    if (strstr(fullName, "VlSensivity")) return "VEL";
    if (strstr(fullName, "KeybScaling")) return "SCA";
    if (strstr(fullName, "Coarse Tune")) return "COAR";
    if (strstr(fullName, "Fine Tune")) return "FINE";
    if (strstr(fullName, "Delay Feedback")) return "FEED";
    if (strstr(fullName, "Delay LFO Rate")) return "RATE";
    if (strstr(fullName, "Delay LFO Amt")) return "AMNT";
    if (strstr(fullName, "Reverb Damp")) return "DAMP";
    if (strstr(fullName, "Reverb Time")) return "TIME";
    if (strstr(fullName, "Delay Level")) return "DELAY";
    if (strstr(fullName, "Reverb Level")) return "REVERB";
    if (strstr(fullName, "Portamento")) return "PORTA";
    if (strstr(fullName, "Pitch Curve")) return "CURVE";
    if (strstr(fullName, "PitchCurveTime")) return "TIME";
    if (strstr(fullName, "LFO Rate")) return "RATE";
    if (strstr(fullName, "LFO Depth")) return "DEPHT";
    if (strstr(fullName, "LFO Delay")) return "DELAY";
    if (strstr(fullName, "LFO Destination")) return "DEST";
    if (strstr(fullName, "Mod Destination")) return "DEST";
    if (strstr(fullName, "Waveform")) return "WAVE";
    if (strstr(fullName, "Delay Time")) return "DELAY";
    return "";
}

CKnob::CKnob(int bmp, int knobSize, const char *name, CSynthesizer *synthesizer, char &channel, int type, int par, int x, int y)
{
    strncpy(this->name, name, TEXT_SIZE);
    this->toolkit     = NULL;
    this->bmp         = bmp;
    this->knobSize    = knobSize;
    this->channel     = &channel;
    this->synthesizer = synthesizer;
    this->par         = par;
    this->type        = type;
    this->left        = x;
    this->top         = y;
    this->right       = x + knobSize;
    this->bottom      = y + knobSize;
    this->value       = 0;
    this->fvalue      = 999.f;
    this->isMatrix    = (par >= MAA && par <= MZP);
    this->isOutput    = (par == MAO || par == MBO || par == MCO || par == MDO ||
                         par == MEO || par == MFO || par == MXO || par == MZO);
    this->isPan       = (par == MAP || par == MBP || par == MCP || par == MDP ||
                         par == MEP || par == MFP || par == MXP || par == MZP || type == VL_PAN);
    this->isSelfMod   = (par == MAA || par == MBB || par == MCC || par == MDD ||
                         par == MEE || par == MFF);
    this->shortLabel  = GetShortLabel(this->name);
}

int CKnob::GetCoordinates (oxeCoords *coords)
{
    char valtemp = value;
    switch (type)
    {
        case VL_WAVEFORM:
            valtemp = (char)lrintf(fvalue*(MAXPARVALUE/(WAVEFORMS-1)));
            break;
        case VL_FILTER:
            valtemp = (char)lrintf(fvalue*(MAXPARVALUE/2));
            break;
        case VL_LFO_DEST:
            valtemp = (char)lrintf(fvalue*(MAXPARVALUE/2));
            break;
        case VL_MOD_DEST:
            valtemp = (char)lrintf(fvalue*(MAXPARVALUE/6));
            break;
    }
    if (valtemp > 99)
        valtemp = 99;
    coords->destX   = this->left;
    coords->destY   = this->top;
    coords->width   = this->right - this->left;
    coords->height  = this->bottom - this->top;
    coords->origBmp = this->bmp;
    coords->origX   = (valtemp - (abs(valtemp/10) * 10)) * this->knobSize;
    coords->origY   = abs(valtemp/10) * this->knobSize;
    return 1;
}

void CKnob::GetRepaintBounds(int &rx, int &ry, int &rw, int &rh) const
{
    if (isMatrix)
    {
        rx = this->left - 2;
        ry = this->top - 2;
        rw = this->knobSize + 4;
        rh = this->knobSize + 4;
    }
    else
    {
        rx = this->left - 6;
        ry = this->top - 2;
        rw = this->knobSize + 12;
        rh = this->knobSize + 16;
    }
}

void CKnob::Repaint()
{
    if (toolkit)
    {
        int rx, ry, rw, rh;
        GetRepaintBounds(rx, ry, rw, rh);
        toolkit->InvalidateRect(rx, ry, rw, rh);
    }
}



void CKnob::Paint(BLContext &ctx, const BLFont &fontSmall, const BLFont &fontNormal)
{
    float cx = left + knobSize * 0.5f;
    float cy = top + knobSize * 0.5f;
    float r  = knobSize * 0.42f;

    if (isMatrix)
    {
        // Matrix rotary node
        BLRgba32 baseRingColor(0x28, 0x33, 0x43);
        BLRgba32 activeColor(0x00, 0xf0, 0xff); // default cyan

        if (isOutput)
        {
            activeColor = BLRgba32(0xff, 0x90, 0x00); // orange output
        }
        else if (isPan)
        {
            activeColor = BLRgba32(0x00, 0xe5, 0xff); // cyan pan
        }
        else if (isSelfMod)
        {
            activeColor = BLRgba32(0xff, 0xc8, 0x20); // gold self-mod
        }

        // Draw background node circle
        ctx.fill_circle(cx, cy, r, BLRgba32(0x16, 0x1d, 0x26));
        ctx.set_stroke_width(1.5);
        ctx.stroke_circle(cx, cy, r, baseRingColor);

        if (value > 0)
        {
            ctx.set_stroke_width(2.2);
            ctx.set_stroke_caps(BL_STROKE_CAP_ROUND);

            if (isPan)
            {
                // Bipolar pan arc from top (1.5 * M_PI)
                float offset = ((float)value - 50.0f) / 50.0f; // -1..1
                float sweep = offset * 0.75f * (float)M_PI;
                ctx.stroke_arc(BLArc(cx, cy, r, r, 1.5 * M_PI, sweep), activeColor);
            }
            else
            {
                // Unipolar arc from 0.75 * M_PI
                float norm = (float)value / (float)MAXPARVALUE;
                float sweep = norm * 1.5f * (float)M_PI;
                ctx.stroke_arc(BLArc(cx, cy, r, r, 0.75 * M_PI, sweep), activeColor);
            }

            // Glowing center pip
            ctx.fill_circle(cx, cy, 2.5, activeColor);
        }
        else
        {
            // Dim center dot
            ctx.fill_circle(cx, cy, 1.8, BLRgba32(0x2b, 0x35, 0x44));
        }
    }
    else
    {
        // Standard high-resolution synth knob
        ctx.set_stroke_width(2.6);
        ctx.set_stroke_caps(BL_STROKE_CAP_ROUND);

        // Background track (270 degrees)
        ctx.stroke_arc(BLArc(cx, cy, r, r, 0.75 * M_PI, 1.5 * M_PI), BLRgba32(0x22, 0x2a, 0x36));

        bool isBipolar = (type == VL_PAN || type == VL_MINUS1_2_PLUS1);
        BLRgba32 activeColor(0x00, 0xf0, 0xff); // cyan

        if (type == VL_COARSE_TUNE || type == VL_FINE_TUNE)
        {
            activeColor = BLRgba32(0xff, 0xb7, 0x20); // amber
        }

        float angle = 0.75f * (float)M_PI;
        if (isBipolar)
        {
            float norm = ((float)value - 50.0f) / 50.0f;
            float sweep = norm * 0.75f * (float)M_PI;
            ctx.stroke_arc(BLArc(cx, cy, r, r, 1.5 * M_PI, sweep), activeColor);
            angle = 1.5f * (float)M_PI + sweep;
        }
        else
        {
            float norm = (float)value / (float)MAXPARVALUE;
            float sweep = norm * 1.5f * (float)M_PI;
            ctx.stroke_arc(BLArc(cx, cy, r, r, 0.75 * M_PI, sweep), activeColor);
            angle = 0.75f * (float)M_PI + sweep;
        }

        // Inner dial body with subtle gradient
        float capRadius = r * 0.74f;
        BLGradient radial(BLRadialGradientValues(cx, cy, cx, cy, capRadius));
        radial.add_stop(0.0, BLRgba32(0x28, 0x31, 0x40));
        radial.add_stop(1.0, BLRgba32(0x14, 0x19, 0x22));
        ctx.fill_circle(cx, cy, capRadius, radial);

        ctx.set_stroke_width(1.0);
        ctx.stroke_circle(cx, cy, capRadius, BLRgba32(0x35, 0x42, 0x54));

        // White indicator needle
        float p1x = cx + cosf(angle) * (capRadius * 0.35f);
        float p1y = cy + sinf(angle) * (capRadius * 0.35f);
        float p2x = cx + cosf(angle) * (capRadius * 0.88f);
        float p2y = cy + sinf(angle) * (capRadius * 0.88f);
        ctx.set_stroke_width(1.8);
        ctx.set_stroke_caps(BL_STROKE_CAP_ROUND);
        BLPath needle;
        needle.move_to(p1x, p1y);
        needle.line_to(p2x, p2y);
        ctx.stroke_path(needle, BLRgba32(0xff, 0xff, 0xff));

        // Short label below dial
        if (shortLabel[0] && fontSmall.is_valid())
        {
            // Estimate text width: ~5.5px per char at size 9
            float tw = strlen(shortLabel) * 5.5f;
            float tx = cx - tw * 0.5f;
            float ty = top + knobSize + 9.0f;
            ctx.fill_utf8_text(BLPoint(tx, ty), fontSmall, shortLabel, SIZE_MAX, BLRgba32(0x7a, 0x8a, 0x9e));
        }
    }
}

bool CKnob::Update(void)
{
    float newValue = synthesizer->GetPar(*channel, par);
    if (newValue != this->fvalue)
    {
        this->fvalue = newValue;
        this->value = CMapper::FloatValueToIntValue(this->synthesizer, *channel, this->par, this->type, this->fvalue);
        Repaint();
    }
    return true;
}

bool CKnob::IncreaseValue(int delta)
{
    this->value += delta;
    if (this->value > (char)MAXPARVALUE)
    {
        this->value = (char)MAXPARVALUE;
    }
    if (this->value < 0)
    {
        this->value = 0;
    }
    this->fvalue = CMapper::IntValueToFloatValue(this->synthesizer, *channel, this->par, this->type, this->value);
    synthesizer->SetPar(*channel, this->par, this->fvalue);
    Repaint();
    return true;
}

bool CKnob::GetName(char* str)
{
    strncpy(str, name, TEXT_SIZE);
    return true;
}

int CKnob::GetIndex()
{
    return this->par;
}

int CKnob::GetType()
{
    return this->type;
}
