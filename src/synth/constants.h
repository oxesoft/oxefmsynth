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

// ------------------------------------------------------------------
// configuration constants
// ------------------------------------------------------------------

#define OXFM_MAJOR_VERSION  1
#define OXFM_MINOR_VERSION  3
#define OXFM_PATCH_VERSION  5
#define VERSION_STR         "1.3.6"
#define VERSION_INT         ((OXFM_MAJOR_VERSION * 100) + (OXFM_MINOR_VERSION * 10) + OXFM_PATCH_VERSION)
#define TITLE_SMALL         "Oxe FM Synth "VERSION_STR
#define TITLE_FULL          TITLE_SMALL" :: http://www.oxesoft.com"
#define SAMPLES_PER_PROCESS 128      // the synth process buffer size
#define WAVEFORM_BIT_DEPTH  11
#define WAVEFORM_BSIZE      (1<<WAVEFORM_BIT_DEPTH) // waveform size
#define VPITCH              2.0f     // pitch weel semitones
#define POLIPHONY           64       // maximum simultaneous notes
#define MAX_PROGRAMS        128      // programs of a soundbank
#define PG_NAME_SIZE        16       // program name size (there is no \0)
#define WAVEFORMS           6        // number of waveforms
#define C0                  16.3516f // C0 frequency
#define MAXOPER             8        // number of operators
#define MAXOOSC             6        // number of operators excluding X and Z
#define MAXFREQOSC          124.0f   // MIDI note that represents the maximum value of oscillators frequency, when they works independently of the key played
#define MAXFREQFLT          124.0f   // MIDI note that represents the maximum value of filter cutoff frequency
#define MINFREQLFO          0.2f     // minimum LFO frequency, in Hz
#define MAXFREQLFO          8.0f     // maximum LFO frequency, in octaves of the previous one
#define MAXTIMEENV          10.0f    // maximum time of the envelop parameters
#define MAXPARVALUE         100.0f   // maximum time of the GUI controls
#define OCTAVES_PITCH_CURVE 3.f      // number of octaves up and down
#define MIDICHANNELS        16
#define STEREOPHONIC        2
#define TEXT_SIZE           32

// ------------------------------------------------------------------
// GUI constants
// ------------------------------------------------------------------
#define TIMER_RESOLUTION_MS 20
#define BMP_PATH            "skin"
#define GUI_CONTROLS        189
#define GUI_WIDTH           633
#define GUI_HEIGHT          437
enum
{
    BMP_CHARS  ,
    BMP_KNOB   ,
    BMP_KNOB2  ,
    BMP_KNOB3  ,
    BMP_KEY    ,
    BMP_BG     ,
    BMP_BUTTONS,
    BMP_OPS    ,
    BMP_COUNT
};

// ------------------------------------------------------------------
// math constants
// ------------------------------------------------------------------
#define PI                 3.14159265358979f
#define D_PI               3.1415926535897932384626433832795

// ------------------------------------------------------------------
// float to integer fast conversion
// ------------------------------------------------------------------
#ifndef __GNUC__
__inline long int lrint (double flt)
{
    int intgr;

    _asm
    {
        fld flt
        fistp intgr
    };

    return intgr;
}

__inline long int lrintf (float flt)
{
    int intgr;

    _asm
    {
        fld flt
        fistp intgr
    };

    return intgr;
}
#endif

// ------------------------------------------------------------------
// general purpose constants
// ------------------------------------------------------------------
enum
{
    INACTIVE,
    ACTIVE,
    ENDED,
    STATE,
    FREQUENCY,
    TYPE,
    RESONANCE,
    ATTACK,
    DECAY,
    SUSTAIN,
    RELEASE,
    NOTEON,
    NOTEOFF,
    PITCH,
    AFTERTOUCH,
    MODULATION,
    VOLUME,
    PAN,
    ENVELOP,
    SELFOSC,
    TUNING,
    BWAVE,
    DELAY,
    SUSTAINTIME,
    BYPASS,
    INTERPOLATION,
    SAMPLERATE,
    TIME,
    FEEDBACK,
    DAMP,
    RATE,
    AMOUNT,
    SINGLESAMPLEMODE
};

#ifndef min
#define min(X, Y) (((X) < (Y)) ? (X) : (Y))
#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef MAX_PATH
#define MAX_PATH 256
#endif

#ifdef _WIN32
    #define snprintf _snprintf
#endif
