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

#import "cocoawrapper.h"
#import <Cocoa/Cocoa.h>

@interface CocoaToolkit : NSObject
{
    int test;
}
- (int) test:(int) par;
@end

@implementation CocoaToolkit

void* CocoaToolkitInit()
{
    return [[CocoaToolkit alloc] init];
}
void CocoaToolkitDeinit(void *self)
{
    [(id)self dealloc];
}

int CocoaToolkitTest(void *self, int par)
{
    return [(id)self test:par];
}

- (int) test:(int) par
{
    return par + 1;
}

@end