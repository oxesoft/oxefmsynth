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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <strsafe.h>

#include "constants.h"
#include "persist.h"
#include "programs.h"
#include "bank0.h"
#include "bank1.h"

void GetDllPath(char *caminho, int tam)
{
	ZeroMemory(caminho, tam);
	HMODULE hm = NULL;
	if (!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCSTR) &GetDllPath, &hm))
	{
		OXEDEBUG("GetModuleHandleExA returned %d\n", GetLastError());
	}
	DWORD x = GetModuleFileNameA(hm, caminho, tam);
	if (!x)
		return;
	int p = lstrlenA(caminho);
	if (!p)
		return;
	while (p--)
	{
		if (caminho[p] == '\\')
		{
			caminho[p+1] = 0;
			break;
		}
	}
}

CPersist::CPersist()
{
	bank_from_host = -1;
	banks_count = 0;
	ZeroMemory(banks, sizeof(banks));
	AddBank((void*)bank0, sizeof(bank0), (char*)"bank0 (internal)", false);
	AddBank((void*)bank1, sizeof(bank1), (char*)"bank1 (internal)", false);
	GetDllPath(dll_path,sizeof(dll_path));
	EnumerateBanks();
}

int CPersist::AddBank(void *bank, unsigned int size, char *name, bool from_host)
{
	int index = banks_count;
	if (from_host == true)
	{
		if (bank_from_host > -1)
		{
			index = bank_from_host;
		}
		else
		{
			bank_from_host = banks_count++;
		}
	}
	else if (banks_count >= MAX_BANKS - 1)
	{
		return -1;
	}
	else
	{
		banks_count++;
	}
	if (bank != NULL && size == SOUNDBANK_SIZE)
	{
		CopyMemory(banks[index], bank, size);
		StringCchCopyA(SoundBankNames[index], MAX_PATH, name);
	}
	return index;
}

void CPersist::LoadExternalBank(void *bank, char *filename)
{
	char path[MAX_PATH];
	BYTE tmp[SOUNDBANK_SIZE];
	DWORD bytes_read = 0;
	StringCchCopyA(path, MAX_PATH, dll_path);
	StringCchCatA(path, MAX_PATH, filename);
	HANDLE file = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file == INVALID_HANDLE_VALUE)
	{
		return;
	}
	ReadFile(file, tmp, SOUNDBANK_SIZE, &bytes_read, NULL);
	CloseHandle(file);
	if (bytes_read != SOUNDBANK_SIZE)
	{
		return;
	}
	AddBank(tmp, SOUNDBANK_SIZE, filename, false);
}

void CPersist::EnumerateBanks()
{
	char cambanloc[MAX_PATH];
	WIN32_FIND_DATAA FindFileData;
	HANDLE hFind;
	StringCchCopyA(cambanloc, MAX_PATH, dll_path);
	StringCchCatA(cambanloc, MAX_PATH, "*.bin");
	hFind = FindFirstFileA(cambanloc, &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE)
		return;
	while (1)
	{
		if (FindFileData.nFileSizeLow == SOUNDBANK_SIZE)
		{
			LoadExternalBank(banks[banks_count], FindFileData.cFileName);
		}
		if (!FindNextFileA(hFind,&FindFileData))
			break;
	}
	FindClose(hFind);
	//SortFilenames(2,banks_count-1);
}

/*
void CPersist::SortFilenames(int inLow, int inHi)
{
	char vPivot[MAX_FILENAME];
	char vSwap [MAX_FILENAME];
	int  lLow = inLow;
	int  lHi  = inHi;
	StringCchCopyA(vPivot, MAX_FILENAME, SoundBankNames[(inLow+inHi)/2]);
	while (lLow <= lHi)
	{
		while (lstrcmpiA(SoundBankNames[lLow], vPivot) < 0 && lLow < inHi )
			lLow++;
		while (lstrcmpiA(SoundBankNames[lHi],  vPivot) > 0 && lHi  > inLow)
			lHi--;
		if (lLow <= lHi)
		{
			StringCchCopyA(vSwap               , MAX_FILENAME, SoundBankNames[lLow]);
			StringCchCopyA(SoundBankNames[lLow], MAX_FILENAME, SoundBankNames[lHi]);
			StringCchCopyA(SoundBankNames[lHi] , MAX_FILENAME, vSwap);
			lLow++;
			lHi--;
		}
	}
	if (inLow < lHi)
		SortFilenames(inLow, lHi);
	if (lLow < inHi)
		SortFilenames(lLow, inHi);
}
*/

int CPersist::GetNumberBanks()
{
	return banks_count;
}

void CPersist::GetBankName(char *str, int index)
{
	StringCchCopyA(str, MAX_FILENAME, SoundBankNames[index]);
}

void* CPersist::GetSoundBank(int index)
{
	return banks[index];
}
