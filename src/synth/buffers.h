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

class CBuffers
{
private:
	void FillWaveforms();
	void Filtrar(int indorigem, int inddestino);
	void Normalizar(int indice);
public:
	int  bOPA          [SAMPLES_PER_PROCESS];
	int  bOPB          [SAMPLES_PER_PROCESS];
	int  bOPC          [SAMPLES_PER_PROCESS];
	int  bOPD          [SAMPLES_PER_PROCESS];
	int  bOPE          [SAMPLES_PER_PROCESS];
	int  bOPF          [SAMPLES_PER_PROCESS];
	int  bOPX          [SAMPLES_PER_PROCESS];
	int  bOPZ          [SAMPLES_PER_PROCESS];
	int  bREV          [SAMPLES_PER_PROCESS];
	int  bDLY          [SAMPLES_PER_PROCESS];
	// output
	int  bNoteOut      [SAMPLES_PER_PROCESS<<1];
	int  bSynthOut     [SAMPLES_PER_PROCESS<<1];
	// waveforms
	short bWaves       [WAVEFORMS][WAVEFORM_BSIZE];
	// construtor
	CBuffers();
};
