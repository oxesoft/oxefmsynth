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

all:
	@echo "Building converter"
	@$(MAKE) -s -f Makefile.converter
	@echo "Building demo"
	@$(MAKE) -s -f Makefile.demo
	@echo "Building standalone"
	@$(MAKE) -s -f Makefile.standalone
	@echo "Building plugin"
	@$(MAKE) -s -f Makefile.vstwindows

clean:
	@rm -f converter oxefmsynthdemo oxefmsynthdemo.exe oxefmsynthdemo.wav oxefmsynth.exe oxevst64.dll oxevst.dll
