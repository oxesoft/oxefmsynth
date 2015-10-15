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

#if defined(__linux)
#include <unistd.h>
#include <time.h>
#elif defined(__APPLE__)
#include <mach/mach_time.h>
#else
#include <windows.h>
#endif
unsigned int GetTick()
{
#if defined(__linux)
    struct timespec ts;
    unsigned int theTick = 0U;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    theTick  = ts.tv_nsec / 1000000;
    theTick += ts.tv_sec * 1000;
    return theTick;
#elif defined(__APPLE__)
    mach_timebase_info_data_t info;
    mach_timebase_info(&info);
    uint64_t value = mach_absolute_time();
    value *= info.numer;
    value /= info.denom;
    value /= 1000000;
    return (unsigned int)value;
#else
    return (unsigned int)GetTickCount();
#endif
}
