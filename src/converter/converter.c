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

/*
Group MIDI events in chunks of:

Tempo
MIDI channel
    Note
    Polyphonic aftertouch
    Control Change
        Number of Controller
    Program Change
    Channel aftertouch
    Pitch Bend

Where each chunk is grouped in:
    length of events
    timestamp  , timestamp  , timestamp  , ...
    status     , status     , status     , ...
    data byte 1, data byte 1, data byte 1, ...
    data byte 2, data byte 2, data byte 2, ...

TODO:
- clear all patches names
- clear all unused patches
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned int MThd = 'M'<<24 | 'T'<<16 | 'h'<<8 | 'd';
static unsigned int MTrk = 'M'<<24 | 'T'<<16 | 'r'<<8 | 'k';

struct {
    unsigned int   ChunkType;
    unsigned int   Length;
    unsigned short Format;
    unsigned short NTracks;
    unsigned short Division;
} headerchunk;

#define sizeof_headerchunk 14

struct {
    unsigned int ChunkType;
    unsigned int Length;
} trackchunk;

#define sizeof_trackchunk 8

typedef struct {
    unsigned int   timestamp;
    unsigned char  status;
    unsigned char  data1;
    unsigned char  data2;
} EVENT;

typedef struct
{
    unsigned int   length;
    EVENT        **events;
} TYPE;

#define CONTROLS 128

typedef struct
{
    TYPE notes;           /* note events */
    TYPE poaft;           /* poly aftertouch events */
    TYPE contr[CONTROLS]; /* control change events for each control number */
    TYPE progr;           /* program change events */
    TYPE chaft;           /* channel aftertouch events */
    TYPE pitch;           /* pitch wheel events */
} CHANNEL;

#define CHANNELS 16

typedef struct
{
    unsigned short division;    /* ticks per quarter */
    unsigned short nchunks;     /* number of filled chunks */
             int   nevents;     /* total number of events */
    TYPE    tempo;              /* tempo events */
    CHANNEL channels[CHANNELS]; /* channel events for each channel */
} OUTPUT;

static unsigned int IntToLittleEndian(unsigned int number)
{
    unsigned int result;
    *(((char*)&result)+0)  = *(((char*)&number)+3);
    *(((char*)&result)+1)  = *(((char*)&number)+2);
    *(((char*)&result)+2)  = *(((char*)&number)+1);
    *(((char*)&result)+3)  = *(((char*)&number)+0);
    return result;
}
static unsigned short ShortToLittleEndian(unsigned short number)
{
    unsigned short result;
    *(((char*)&result)+0) = *(((char*)&number)+1);
    *(((char*)&result)+1) = *(((char*)&number)+0);
    return result;
}

static unsigned int ReadVarLen(FILE *file)
{
    unsigned int value;
    unsigned char c;

    if ((value = getc(file)) & 0x80)
    {
        value  &= 0x7f;
        do
            value = (value << 7) + ((c = (unsigned char)getc(file)) & 0x7f);
        while (c & 0x80);
    }
    return(value);
}

void initialize_output(OUTPUT *output)
{
    int channel = CHANNELS;
    int number;
    output->tempo.length = 0;
    output->tempo.events = NULL;
    while (channel--)
    {
        output->channels[channel].notes.length = 0;
        output->channels[channel].notes.events = NULL;
        output->channels[channel].poaft.length = 0;
        output->channels[channel].poaft.events = NULL;
        number = CONTROLS;
        while (number--)
        {
            output->channels[channel].contr[number].length = 0;
            output->channels[channel].contr[number].events = NULL;
        }
        output->channels[channel].progr.length = 0;
        output->channels[channel].progr.events = NULL;
        output->channels[channel].chaft.length = 0;
        output->channels[channel].chaft.events = NULL;
        output->channels[channel].pitch.length = 0;
        output->channels[channel].pitch.events = NULL;
    }
}

int compare_events(const void *a, const void *b)
{
    EVENT *arg1 = *(EVENT**)a;
    EVENT *arg2 = *(EVENT**)b;
    if (arg1->timestamp < arg2->timestamp)
        return -1;
    else if (arg1->timestamp == arg2->timestamp )
        return 0;
    else
        return 1;
}

void sort_output(OUTPUT *output)
{
    int channel = CHANNELS;
    int number;
    qsort(output->tempo.events, output->tempo.length, sizeof(EVENT*), compare_events);
    while (channel--)
    {
        qsort(output->channels[channel].notes.events, output->channels[channel].notes.length, sizeof(EVENT*), compare_events);
        qsort(output->channels[channel].poaft.events, output->channels[channel].poaft.length, sizeof(EVENT*), compare_events);
        number = CONTROLS;
        while (number--)
            qsort(output->channels[channel].contr[number].events, output->channels[channel].contr[number].length, sizeof(EVENT*), compare_events);
        qsort(output->channels[channel].progr.events, output->channels[channel].progr.length, sizeof(EVENT*), compare_events);
        qsort(output->channels[channel].chaft.events, output->channels[channel].chaft.length, sizeof(EVENT*), compare_events);
        qsort(output->channels[channel].pitch.events, output->channels[channel].pitch.length, sizeof(EVENT*), compare_events);
    }
}

void delta_encode_events(TYPE* t)
{
    EVENT e;
    EVENT o;
    int   l;
    e.timestamp = 0;
    e.status    = 0;
    e.data1     = 0;
    e.data2     = 0;
    for (l=0;l<t->length;l++)
    {
        o.timestamp = t->events[l]->timestamp;
        o.status    = t->events[l]->status;
        o.data1     = t->events[l]->data1;
        o.data2     = t->events[l]->data2;
        t->events[l]->timestamp = (unsigned int )(t->events[l]->timestamp - e.timestamp);
        t->events[l]->status    = (unsigned char)(t->events[l]->status    - e.status   );
        t->events[l]->data1     = (unsigned char)(t->events[l]->data1     - e.data1    );
        t->events[l]->data2     = (unsigned char)(t->events[l]->data2     - e.data2    );
        e.timestamp = o.timestamp;
        e.status    = o.status;
        e.data1     = o.data1;
        e.data2     = o.data2;
    }
}

void encode_output(OUTPUT *output)
{
    int channel = CHANNELS;
    int number;
    delta_encode_events(&output->tempo);
    while (channel--)
    {
        delta_encode_events(&output->channels[channel].notes);
        delta_encode_events(&output->channels[channel].poaft);
        number = CONTROLS;
        while (number--)
            delta_encode_events(&output->channels[channel].contr[number]);
        delta_encode_events(&output->channels[channel].progr);
        delta_encode_events(&output->channels[channel].chaft);
        delta_encode_events(&output->channels[channel].pitch);
    }
}

void count_chunks(OUTPUT *output)
{
    int channel = CHANNELS;
    int number;
    output->nchunks = 0;
    if (output->tempo.length)
        output->nchunks++;
    while (channel--)
    {
        if (output->channels[channel].notes.length)
            output->nchunks++;
        if (output->channels[channel].poaft.length)
            output->nchunks++;
        number = CONTROLS;
        while (number--)
        {
            if (output->channels[channel].contr[number].length)
                output->nchunks++;
        }
        if (output->channels[channel].progr.length)
            output->nchunks++;
        if (output->channels[channel].chaft.length)
            output->nchunks++;
        if (output->channels[channel].pitch.length)
            output->nchunks++;
    }
}

void count_events(OUTPUT *output)
{
    int channel = CHANNELS;
    int number;
    output->nevents = output->tempo.length;
    while (channel--)
    {
        output->nevents += output->channels[channel].notes.length;
        output->nevents += output->channels[channel].poaft.length;
        number = CONTROLS;
        while (number--)
            output->nevents += output->channels[channel].contr[number].length;
        output->nevents += output->channels[channel].progr.length;
        output->nevents += output->channels[channel].chaft.length;
        output->nevents += output->channels[channel].pitch.length;
    }
}

void write_bytes(void* p, char size, FILE* file, int *count)
{
    char i;
    for (i=0;i<size;i++)
        fprintf(file, "0x%02X,%s", *(((unsigned char*)p)+i), ++(*count) % 16?"":"\n");
}

void write_byte(unsigned char c, FILE* file, int *count)
{
    write_bytes(&c, sizeof(char), file, count);
}

void write_short(unsigned short s, FILE* file, int *count)
{
    write_bytes(&s, sizeof(short), file, count);
}

void write_int(unsigned int l, FILE* file, int *count)
{
    write_bytes(&l, sizeof(int), file, count);
}

void write_events(TYPE* t, FILE* file, int *count)
{
    int l;
    if (!t->length)
        return;
    write_int(t->length, file, count);
    for (l=0;l<t->length;l++)
        write_int(t->events[l]->timestamp, file, count);
    for (l=0;l<t->length;l++)
        write_byte(t->events[l]->status, file, count);
    for (l=0;l<t->length;l++)
        write_byte(t->events[l]->data1,  file, count);
    for (l=0;l<t->length;l++)
        write_byte(t->events[l]->data2,  file, count);
}

void write_tune_file(OUTPUT *output, const char* filename)
{
    FILE* file;
    int  channel = CHANNELS;
    int  count   = 0;
    int  number;
    file = fopen(filename, "wb");
    fprintf(file, "#define NEVENTS %d\n\n", output->nevents);
    fprintf(file, "static const unsigned char tune[] = {\n");
    write_short(output->division, file, &count);
    write_short(output->nchunks , file, &count);
    write_events(&output->tempo , file, &count);
    while (channel--)
    {
        write_events(&output->channels[channel].notes, file, &count);
        write_events(&output->channels[channel].poaft, file, &count);
        number = CONTROLS;
        while (number--)
            write_events(&output->channels[channel].contr[number], file, &count);
        write_events(&output->channels[channel].progr, file, &count);
        write_events(&output->channels[channel].chaft, file, &count);
        write_events(&output->channels[channel].pitch, file, &count);
    }
    fseek(file, -1, SEEK_CUR);
    fprintf(file, "};\n");
    fclose(file);
    printf("Bytes:  %d\n", count);
    printf("Chunks: %d\n", output->nchunks);
    printf("Events: %d\n", output->nevents);
}

void free_output(OUTPUT *output)
{
    int channel = CHANNELS;
    int number;
    if (output->tempo.length)
    {
        while (output->tempo.length--)
            free(output->tempo.events[output->tempo.length]);
        free(output->tempo.events);
    }
    while (channel--)
    {
        if (output->channels[channel].notes.length)
        {
            while (output->channels[channel].notes.length--)
                free(output->channels[channel].notes.events[output->channels[channel].notes.length]);
            free(output->channels[channel].notes.events);
        }
        if (output->channels[channel].poaft.length)
        {
            while (output->channels[channel].poaft.length--)
                free(output->channels[channel].poaft.events[output->channels[channel].poaft.length]);
            free(output->channels[channel].poaft.events);
        }
        number = CONTROLS;
        while (number--)
        {
            if (output->channels[channel].contr[number].length)
            {
                while (output->channels[channel].contr[number].length--)
                    free(output->channels[channel].contr[number].events[output->channels[channel].contr[number].length]);
                free(output->channels[channel].contr[number].events);
            }
        }
        if (output->channels[channel].progr.length)
        {
            while (output->channels[channel].progr.length--)
                free(output->channels[channel].progr.events[output->channels[channel].progr.length]);
            free(output->channels[channel].progr.events);
        }
        if (output->channels[channel].chaft.length)
        {
            while (output->channels[channel].chaft.length--)
                free(output->channels[channel].chaft.events[output->channels[channel].chaft.length]);
            free(output->channels[channel].chaft.events);
        }
        if (output->channels[channel].pitch.length)
        {
            while (output->channels[channel].pitch.length--)
                free(output->channels[channel].pitch.events[output->channels[channel].pitch.length]);
            free(output->channels[channel].pitch.events);
        }
    }
}

void insert(TYPE *type, unsigned int timestamp, unsigned char status, unsigned char data1, unsigned char data2)
{
    int len = type->length++;
    if (NULL == type->events)
        type->events = malloc(sizeof(EVENT*) * type->length);
    else
        type->events = realloc(type->events, sizeof(EVENT*) * type->length);
    type->events[len] = malloc(sizeof(EVENT));
    type->events[len]->timestamp = timestamp;
    type->events[len]->status    = status;
    type->events[len]->data1     = data1;
    type->events[len]->data2     = data2;
}

char load_tune_file(OUTPUT* output, const char* filename)
{
    FILE*          file = NULL;
    unsigned int   mark;
    unsigned int   timestamp;
    unsigned int   delta;
    unsigned char  status = 0;
    unsigned char  data1;
    unsigned char  data2;
    unsigned int   temp;
    unsigned int   size;
    unsigned short track      = 0;
    unsigned char  endoftrack = 0;
    unsigned int   trackpos   = 0;
    unsigned char  bt;
             char  b1;
             char  b2;
    size_t         result;
    file = fopen(filename, "rb");
    if (!file)
    {
        printf("File not found\n");
        return 0;
    }
    result = fread(&headerchunk,sizeof_headerchunk,1,file);
    if (!result)
    {
        printf("Error reading file\n");
        return 0;
    }
    headerchunk.ChunkType  = IntToLittleEndian (headerchunk.ChunkType);
    if (MThd != headerchunk.ChunkType)
    {
        fclose(file);
        printf("Invalid file format. Use only .mid files\n");
        return 0;
    }
    headerchunk.Length  = IntToLittleEndian (headerchunk.Length);
    headerchunk.Format  = ShortToLittleEndian(headerchunk.Format);
    printf("Format: %d\n", headerchunk.Format);
    headerchunk.NTracks = ShortToLittleEndian(headerchunk.NTracks);
    b1 = *(((char*)&headerchunk.Division)+0);
    b2 = *(((char*)&headerchunk.Division)+1);
    if (b1 < 0)
        headerchunk.Division = (unsigned short)(-b1 * b2);
    else
        headerchunk.Division = ShortToLittleEndian(headerchunk.Division);
    output->division = headerchunk.Division;
    while (track < headerchunk.NTracks)
    {
        endoftrack = 0;
        fread(&trackchunk,sizeof_trackchunk,1,file);
        trackchunk.ChunkType = IntToLittleEndian(trackchunk.ChunkType);
        if (MTrk != trackchunk.ChunkType)
        {
            fclose(file);
            printf("Invalid file format. Use only .mid files\n");
            return 0;
        }
        trackchunk.Length = IntToLittleEndian(trackchunk.Length);
        trackpos = ftell(file);
        mark = trackpos + trackchunk.Length;
        timestamp = 0;
        while (trackpos < mark)
        {
            delta = ReadVarLen(file);
            timestamp += delta;
            bt = (unsigned char)fgetc(file);
            if (bt & 0x80)
                status = bt;
            else
                fseek(file, -1, SEEK_CUR);
            switch(status & 0xF0)
            {
            case 0x80: /* Note Off */
                data1 = (unsigned char)fgetc(file);
                data2 = (unsigned char)fgetc(file);
                insert(&output->channels[status & 0xF].notes,timestamp,(unsigned char)(status + 0x10),data1,0);
                break;
            case 0x90: /* Note On */
                data1 = (unsigned char)fgetc(file);
                data2 = (unsigned char)fgetc(file);
                insert(&output->channels[status & 0xF].notes,timestamp,status,data1,data2);
                break;
            case 0xA0: /* Polyphonic aftertouch */
                data1 = (unsigned char)fgetc(file);
                data2 = (unsigned char)fgetc(file);
                insert(&output->channels[status & 0xF].poaft,timestamp,status,data1,data2);
                break;
            case 0xB0: /* Control / Mode change */
                data1 = (unsigned char)fgetc(file);
                data2 = (unsigned char)fgetc(file);
                switch (data1)
                {
                case 0:   /* Bank Select (coarse) */
                    break;
                case 1:   /* Modulation Wheel (coarse) */
                    insert(&output->channels[status & 0xF].contr[data1],timestamp,status,data1,data2);
                    break;
                case 2:   /* Breath controller (coarse) */
                    break;
                case 4:   /* Foot Pedal (coarse) */
                    break;
                case 5:   /* Portamento Time (coarse) */
                    break;
                case 6:   /* Data Entry (coarse) */
                    break;
                case 7:   /* Volume (coarse) */
                    insert(&output->channels[status & 0xF].contr[data1],timestamp,status,data1,data2);
                    break;
                case 8:   /* Balance (coarse) */
                    break;
                case 10:  /* Pan position (coarse) */
                    insert(&output->channels[status & 0xF].contr[data1],timestamp,status,data1,data2);
                    break;
                case 11:  /* Expression (coarse) */
                    break;
                case 12:  /* Effect Control 1 (coarse) */
                    break;
                case 13:  /* Effect Control 2 (coarse) */
                    break;
                case 16:  /* General Purpose Slider 1 */
                    break;
                case 17:  /* General Purpose Slider 2 */
                    break;
                case 18:  /* General Purpose Slider 3 */
                    break;
                case 19:  /* General Purpose Slider 4 */
                    break;
                case 32:  /* Bank Select (fine) */
                    break;
                case 33:  /* Modulation Wheel (fine) */
                    break;
                case 34:  /* Breath controller (fine) */
                    break;
                case 36:  /* Foot Pedal (fine) */
                    break;
                case 37:  /* Portamento Time (fine) */
                    break;
                case 38:  /* Data Entry (fine) */
                    break;
                case 39:  /* Volume (fine) */
                    break;
                case 40:  /* Balance (fine) */
                    break;
                case 42:  /* Pan position (fine) */
                    break;
                case 43:  /* Expression (fine) */
                    break;
                case 44:  /* Effect Control 1 (fine) */
                    break;
                case 45:  /* Effect Control 2 (fine) */
                    break;
                case 64:  /* Hold Pedal (on/off) */
                    break;
                case 65:  /* Portamento (on/off) */
                    break;
                case 66:  /* Sustenuto Pedal (on/off) */
                    break;
                case 67:  /* Soft Pedal (on/off) */
                    break;
                case 68:  /* Legato Pedal (on/off) */
                    break;
                case 69:  /* Hold 2 Pedal (on/off) */
                    break;
                case 70:  /* Sound Variation */
                    break;
                case 71:  /* Sound Timbre */
                    break;
                case 72:  /* Sound Release Time */
                    break;
                case 73:  /* Sound Attack Time */
                    break;
                case 74:  /* Sound Brightness */
                    break;
                case 75:  /* Sound Control 6 */
                    break;
                case 76:  /* Sound Control 7 */
                    break;
                case 77:  /* Sound Control 8 */
                    break;
                case 78:  /* Sound Control 9 */
                    break;
                case 79:  /* Sound Control 10 */
                    break;
                case 80:  /* General Purpose Button 1 (on/off) */
                    break;
                case 81:  /* General Purpose Button 2 (on/off) */
                    break;
                case 82:  /* General Purpose Button 3 (on/off) */
                    break;
                case 83:  /* General Purpose Button 4 (on/off) */
                    break;
                case 91:  /* Effects Level */
                    break;
                case 92:  /* Tremulo Level */
                    break;
                case 93:  /* Chorus Level */
                    break;
                case 94:  /* Celeste Level */
                    break;
                case 95:  /* Phaser Level */
                    break;
                case 96:  /* Data Button increment */
                    break;
                case 97:  /* Data Button decrement */
                    break;
                case 98:  /* Non-registered Parameter (fine) */
                    break;
                case 99:  /* Non-registered Parameter (coarse) */
                    break;
                case 100: /* Registered Parameter (fine) */
                    break;
                case 101: /* Registered Parameter (coarse) */
                    break;
                case 120: /* All Sound Off */
                    break;
                case 121: /* All Controllers Off */
                    break;
                case 122: /* Local Keyboard (on/off) */
                    break;
                case 123: /* All Notes Off */
                    break;
                case 124: /* Omni Mode Off */
                    break;
                case 125: /* Omni Mode On */
                    break;
                case 126: /* Mono Operation */
                    break;
                case 127: /* Poly Operation */
                    break;
                }
                break;
            case 0xC0: /* Program change */
                data1 = (unsigned char)fgetc(file);
                data2 = 0;
                insert(&output->channels[status & 0xF].progr,timestamp,status,data1,data2);
                break;
            case 0xD0: /* Channel aftertouch */
                data1 = (unsigned char)fgetc(file);
                data2 = 0;
                insert(&output->channels[status & 0xF].chaft,timestamp,status,data1,data2);
                break;
            case 0xE0: /* Pitch wheel range */
                data1 = (unsigned char)fgetc(file);
                data2 = (unsigned char)fgetc(file);
                insert(&output->channels[status & 0xF].pitch,timestamp,status,data1,data2);
                break;
            case 0xF0: /* Other */
                if (status!=0xff) /* SysEx */
                {
                    size = ReadVarLen(file);
                    fseek(file, size, SEEK_CUR);
                }
                else /* Meta-events */
                {
                    status = (unsigned char)fgetc(file);
                    size = ReadVarLen(file);
                    switch(status)
                    {
                    case 0x0: /* Sequence Number */
                        fseek(file, size, SEEK_CUR);
                        break;
                    case 0x1: /* Text */
                        fseek(file, size, SEEK_CUR);
                        break;
                    case 0x2: /* Copyright Notice */
                        fseek(file, size, SEEK_CUR);
                        break;
                    case 0x3: /* Sequence/Track Name */
                        fseek(file, size, SEEK_CUR);
                        break;
                    case 0x4: /* Instrument Name */
                        fseek(file, size, SEEK_CUR);
                        break;
                    case 0x5: /* Lyric */
                        fseek(file, size, SEEK_CUR);
                        break;
                    case 0x6: /* Marker */
                        fseek(file, size, SEEK_CUR);
                        break;
                    case 0x7: /* Cue Point */
                        fseek(file, size, SEEK_CUR);
                        break;
                    case 0x20: /* MIDI Channel Prefix */
                        fseek(file, size, SEEK_CUR);
                        break;
                    case 0x21: /* MIDI Output Port */
                        fseek(file, size, SEEK_CUR);
                        break;
                    case 0x2F: /* End of Track */
                        endoftrack = 1;
                        break;
                    case 0x51: /* Set Tempo */
                        temp  = 0;
                        temp |= (unsigned char)fgetc(file) << 16;
                        temp |= (unsigned char)fgetc(file) << 8;
                        temp |= (unsigned char)fgetc(file) << 0;
                        data1 = (unsigned char)(60000000 / temp);
                        data2 = 0;
                        insert(&output->tempo,timestamp,status,data1,data2);
                        break;
                    case 0x54: // SMPTE Offset
                        fseek(file, size, SEEK_CUR);
                        break;
                    case 0x58: // Time Signature
                        fseek(file, size, SEEK_CUR);
                        break;
                    case 0x59: // Key Signature
                        fseek(file, size, SEEK_CUR);
                        break;
                    case 0x7F: // Sequencer-Specific
                        fseek(file, size, SEEK_CUR);
                        break;
                    default: // Unknow event
                        fseek(file, size, SEEK_CUR);
                        break;
                    }
                }
                break;
            default: //
                fclose(file);
                printf("Invalid status byte 0x%X\n", status);
                return 0;
                break;
            }
            trackpos = ftell(file);
        }
        if (!endoftrack)
            continue; // End of track is missing
        track++;
    }
    fclose(file);
    printf("Tracks: %d\n", track);
    return 1;
}

char convert_soudbank_file(const char *input_file, const char *output_file)
{
    FILE *ifile = NULL;
    FILE *ofile = NULL;
    int   count = 0;
    unsigned char b;
    ifile = fopen(input_file, "rb");
    if (!ifile)
    {
        printf("Unable to open input_file");
        return 0;
    }
    ofile = fopen(output_file, "w");
    if (!ofile)
    {
        printf("Unable to open output_file");
        return 0;
    }
    fprintf(ofile, "static const unsigned char soundbank[] = {\n");
    while (fread(&b, 1, 1, ifile) == 1)
    {
        write_byte(b, ofile, &count);
    }
    fseek(ofile, -1, SEEK_CUR);
    fprintf(ofile, "};\n\n");
    fclose(ofile);
    fclose(ifile);
    printf("Bytes read: %d\n", count);
    return 1;
}

void usage()
{
    printf("Oxe FM Synth Converter Developer Tool\n");
    printf("Description: Converts a binary file of specified type to C header file\n");
    printf("Usage:       converter <option> input_file output_file\n");
    printf("Options:     -m input_file is a .mid file\n");
    printf("             -b input_file is a soundbank file\n");
}

int main(int argc, char* argv[])
{
    OUTPUT         output;
    if (argc<4)
    {
        usage();
        exit(1);
    }
    if (!strcmp(argv[1], "-m"))
    {
        initialize_output(&output);
        if (!load_tune_file(&output, argv[2]))
        {
            free_output(&output);
            exit(1);
        }
        sort_output(&output);
        encode_output(&output);
        count_chunks(&output);
        count_events(&output);
        write_tune_file(&output, argv[3]);
        free_output(&output);
    }
    else if (!strcmp(argv[1], "-b"))
    {
        if (!convert_soudbank_file(argv[2], argv[3]))
        {
            exit(1);
        }
    }
    else
    {
        usage();
        exit(1);
    }
    printf("Done.\n");
    exit(0);
}
