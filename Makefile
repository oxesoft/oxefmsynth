# Oxe FM Synth: a software synthesizer
# Copyright (C) 2015  Daniel Moura <oxe@oxesoft.com>
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

UNAME_S:=$(shell uname -s)

all:
	@echo "Building converter"
	@$(MAKE) -s -f Makefile.converter
	@echo "Building demo"
	@$(MAKE) -s -f Makefile.demo
	@echo "Building standalone"
	@$(MAKE) -s -f Makefile.standalone
ifeq ($(OS),Windows_NT)
	@echo "Building Windows VST plugin (32 and 64 bit)"
	@$(MAKE) -s -f Makefile.vstwindows
endif
ifeq ($(UNAME_S),Linux)
	@echo "Building Linux VST plugin"
	@$(MAKE) -s -f Makefile.vstlinux
endif

clean:
	@rm -f oxeconverter oxefmsynthdemo oxefmsynthdemo.exe oxefmsynthdemo.wav oxefmsynth oxefmsynth.exe oxevst*.dll oxevst*.so embedresources bitmaps.h *.o *.d
