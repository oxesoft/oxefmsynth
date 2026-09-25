# Oxe FM Synth: a software synthesizer
# Copyright (C) 2015  Daniel Moura <oxesoft@gmail.com>
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

.PHONY: all clean standalone vst3 clap install install-vst3 install-clap

all:
	@echo "Building converter"
	@$(MAKE) -s -f Makefile.converter
	@echo "Building demo"
	@$(MAKE) -s -f Makefile.demo
	@echo "Building standalone"
	@$(MAKE) -s -f Makefile.standalone
	@$(MAKE) -s clap

standalone:
	@echo "Building standalone"
	@$(MAKE) -s -f Makefile.standalone

vst3:
	@echo "Building VST3 plugin"
	@$(MAKE) -s -f Makefile.vst3

clap:
	@echo "Building CLAP plugin"
	@$(MAKE) -s -f Makefile.clap

install-vst3:
	@$(MAKE) -s -f Makefile.vst3 install

install-clap:
	@$(MAKE) -s -f Makefile.clap install

install: install-clap

clean:
	@rm -rf bin oxeconverter oxefmsynthdemo oxefmsynthdemo.exe oxefmsynthdemo.wav oxefmsynth oxefmsynth.exe *.o *.d oxefmsynth.app oxefmsynth.vst3 oxefmsynth.clap
