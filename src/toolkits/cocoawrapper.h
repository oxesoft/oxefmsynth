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

#ifdef __cplusplus
extern "C"
{
#endif

void* CocoaToolkitCreate(void *toolkit);
void  CocoaToolkitDestroy(void *self);
void  CocoaToolkitCreateWindow(void *self, void *parent);
void  CocoaToolkitShowWindow(void *self);
void  CocoaToolkitWaitWindowClosed(void *self);
void  CocoaToolkitCopyRect(void *self, int destX, int destY, int width, int height, int origBmp, int origX, int origY);

void  CppOnLButtonDown(void *toolkit, int x, int y);
void  CppOnLButtonUp(void *toolkit);
void  CppOnMouseMove(void *toolkit, int x, int y);
void  CppUpdate(void *toolkit);

#ifdef __cplusplus
}
#endif
