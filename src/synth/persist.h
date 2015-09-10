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

#ifdef __OXEDMO__
typedef void CPersist;
#else

#define MAX_FILENAME   64
#define MAX_BANKS      8
#define SOUNDBANK_SIZE 90688

void GetDllPath(char *caminho, int tam);

class CPersist
{
private:
	unsigned char banks[MAX_BANKS][SOUNDBANK_SIZE];
	char dll_path[MAX_PATH];
	char SoundBankNames[MAX_BANKS][MAX_FILENAME];
	int  banks_count;
	int  bank_from_host;
	void GetSoundBankPath(void);
	void LoadExternalBank(void *bank, char *filename);
	void EnumerateBanks();
	void SortFilenames(int inLow, int inHi);
public:
	CPersist();
	int   GetNumberBanks(void);
	void  GetBankName(char *str, int index);
	void* GetSoundBank(int index);
	int   AddBank(void *bank, unsigned int size, char *name, bool from_host);
};
#endif
