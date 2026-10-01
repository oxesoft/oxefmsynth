/*
Oxe FM Synth: a software synthesizer
Copyright (C) 2004-2015  Daniel Moura <oxesoft@gmail.com>

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
    float bOPA          [SAMPLES_PER_PROCESS];
    float bOPB          [SAMPLES_PER_PROCESS];
    float bOPC          [SAMPLES_PER_PROCESS];
    float bOPD          [SAMPLES_PER_PROCESS];
    float bOPE          [SAMPLES_PER_PROCESS];
    float bOPF          [SAMPLES_PER_PROCESS];
    float bOPX          [SAMPLES_PER_PROCESS];
    float bOPZ          [SAMPLES_PER_PROCESS];
    float bREV          [SAMPLES_PER_PROCESS];
    float bDLY          [SAMPLES_PER_PROCESS];
    // output
    float bNoteOut      [SAMPLES_PER_PROCESS<<1];
    float bSynthOut     [SAMPLES_PER_PROCESS<<1];
    // waveforms
    float bWaves       [WAVEFORMS][WAVEFORM_BSIZE + 1];
    // construtor
    CBuffers();
};
