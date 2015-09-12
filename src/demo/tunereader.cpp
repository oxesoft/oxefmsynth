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

#include "tune.h"
#include "tunereader.h"

CTuneReader::CTuneReader()
{
    unsigned short division;
    unsigned short nchunks;
    unsigned int   evcursor = 0;
    unsigned int   currcoef;
    unsigned int   i;
    unsigned int   endpos;
    unsigned int   len;
    EVENT auxev;
    tune_cursor = 0;
    read_bytes(&division, sizeof(short));
    read_bytes(&nchunks,  sizeof(short));
    while (nchunks--)
    {
        /* Read chunk data */
        read_bytes(&len, sizeof(int));
        endpos = evcursor + len;
        for (i=evcursor;i<endpos;i++)
            read_bytes(&events[i].timestamp, sizeof(int));
        for (i=evcursor;i<endpos;i++)
            read_bytes(&events[i].status   , sizeof(char));
        for (i=evcursor;i<endpos;i++)
            read_bytes(&events[i].data1    , sizeof(char));
        for (i=evcursor;i<endpos;i++)
            read_bytes(&events[i].data2    , sizeof(char));
        /* Delta decode data */
        auxev.timestamp = 0;
        auxev.status    = 0;
        auxev.data1     = 0;
        auxev.data2     = 0;
        for (i=evcursor;i<endpos;i++)
        {
            events[i].timestamp += auxev.timestamp;
            events[i].status    += auxev.status;
            events[i].data1     += auxev.data1;
            events[i].data2     += auxev.data2;
            auxev.timestamp = events[i].timestamp;
            auxev.status    = events[i].status;
            auxev.data1     = events[i].data1;
            auxev.data2     = events[i].data2;
        }
        /* iterate the chunk cursor */
        evcursor = endpos;
    }
    /* rearranges the events in accordance with the timestamps. */
    quick_sort(events, 0, evcursor - 1);
    /* apply tempo meta events in all midi timestamps and make then "sample based" timestamps */
    /* 
    beat             = quarter_note
    beats_per_minute = 120
    beats_per_second = beats_per_minute / 60
    ticks_per_beats  = division
    ticks_per_second = ticks_per_beat * beats_per_second
    samples_per_tick = SAMPLE_RATE / ticks_per_second
    */
    currcoef = (unsigned int)(SAMPLE_RATE / ((float)division * (120.f / 60.f))); /* initializes the coefficient with a default value (120BPM) */
    for (i=0;i<evcursor;i++)
    {
        if (0x51 == events[i].status)
            currcoef = (unsigned int)(SAMPLE_RATE / ((float)division * ((float)events[i].data1 / 60.f)));
        events[i].timestamp *= currcoef;
    }
}

void CTuneReader::read_bytes(void *p, char size)
{
    char i;
    for (i=0;i<size;i++)
        *((char*)p+i) = tune[tune_cursor++];
}

void CTuneReader::quick_sort(EVENT* events, int left, int right)
{
    EVENT tmp;
    EVENT pivot = events[(left + right) / 2];
    int i = left;
    int j = right;

    while (i <= j)
    {
        while (events[i].timestamp < pivot.timestamp && i < right)
        {
            i++;
        }
        while (events[j].timestamp > pivot.timestamp && j > left)
        {
            j--;
        }
        if (i <= j)
        {
            tmp       = events[i];
            events[i] = events[j];
            events[j] = tmp;
            i++;
            j--;
        }
    }
    if (j > left)
    {
        quick_sort(events, left, j);
    }
    if (i < right)
    {
        quick_sort(events, i, right);
    }
}