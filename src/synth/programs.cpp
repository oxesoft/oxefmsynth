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

#include "constants.h"
#ifdef __OXEDMO__
#include "soundbank.h"
#else
#include "persist.h"
#include "hostinterface.h"
#endif
#include "programs.h"
#include <string.h>
#include <algorithm>

void CPrograms::Init()
{
    currentbank = NULL;
    prgtmp      = 0;
    for (int i = 0; i < MIDICHANNELS; i++)
    {
        numprg[i]    = 0;
        isWaiting[i] = false;
    }
#ifndef __OXEDMO__
    hostinterface = NULL;
    iCurrentBank  = 0;
    isEditingName = false;
    bankMode      = false;
    haschanges    = false;
    if (persist.GetNumberBanks()) // always >= 2 since version 1.2.0
    {
        iCurrentBank = 0;
        currentbank  = (SBank*)persist.GetSoundBank(iCurrentBank);
    }
#else
    currentbank = (SBank*)soundbank;
#endif
    if (currentbank)
    {
        for (int i = 0; i < MIDICHANNELS; i++)
        {
            ReloadBuffer(i);
        }
    }
}

// Loads the stored program into the channel's edit buffer.
// Edits go to the buffer only; StoreProgram commits them to the bank.
void CPrograms::ReloadBuffer(char channel)
{
    if (currentbank)
    {
        memcpy(&prgbuffer[channel], &currentbank->prg[numprg[channel]], sizeof(SProgram));
    }
}

SProgram* CPrograms::GetProgram(char channel)
{
    return &prgbuffer[channel];
}

SGlobal* CPrograms::GetSGlobal(void)
{
    return &currentbank->global;
}

void CPrograms::SetNumProgr(char channel, unsigned char numprog)
{
    if (numprog >= MAX_PROGRAMS)
    {
        numprog = MAX_PROGRAMS - 1;
    }
    if (isWaiting[channel])
    {
        prgtmp = numprog;
    }
    else
    {
        numprg[channel] = numprog;
        ReloadBuffer(channel);
    }
#ifndef __OXEDMO__
    haschanges = true;
    if (hostinterface && channel == 0)
    {
        hostinterface->ReceiveMessageFromPlugin(SET_PROGRAM, channel, numprog);
    }
#endif
}

void CPrograms::SetBankIndex(int nbank)
{
#ifndef __OXEDMO__
    if (nbank >= persist.GetNumberBanks())
    {
        return;
    }
    iCurrentBank = nbank;
    currentbank = (SBank*)persist.GetSoundBank(iCurrentBank);
    for (int i = 0; i < MIDICHANNELS; i++)
    {
        if (!isWaiting[i])
        {
            SetNumProgr(i, numprg[i]);
        }
    }
    MarkChanged();
#endif
}

SBank* CPrograms::GetBank(void)
{
    return currentbank;
}

#ifndef __OXEDMO__

void CPrograms::NotifyDisplay()
{
    if (hostinterface)
    {
        hostinterface->ReceiveMessageFromPlugin(UPDATE_DISPLAY, 0, 0);
    }
}

void CPrograms::MarkChanged()
{
    haschanges = true;
    NotifyDisplay();
}

void CPrograms::GetName(char *str, const char *name)
{
    memcpy(str, name, PG_NAME_SIZE);
    str[PG_NAME_SIZE] = 0;
}

void CPrograms::SetName(char *name, const char *str)
{
    memset(name, 0, PG_NAME_SIZE);
    memcpy(name, str, std::min(strlen(str), (size_t)PG_NAME_SIZE));
}

// Parameter metadata: one entry per parameter, name and default value.
// SetDefault/GetPar/SetPar below are generated from these lists, so adding
// a parameter only requires touching this table.
#define PROGRAM_PARAMETERS(X)      \
    /* Operator A */               \
    X(OPAON, 1)                    \
    X(OPAWF, 0)                    \
    X(OPACT, 50)                   \
    X(OPAFT, 1.f)                  \
    X(OPAKT, 1)                    \
    X(OPAKS, 0)                    \
    X(OPAVS, 1.f)                  \
    X(OPADL, 0.f)                  \
    X(OPAAT, 0.000075f)            \
    X(OPADE, 0.000075f)            \
    X(OPASU, 1.f)                  \
    X(OPAST, 0.f)                  \
    X(OPARE, 0.000075f)            \
    /* Operator B */               \
    X(OPBON, 0)                    \
    X(OPBWF, 0)                    \
    X(OPBCT, 50)                   \
    X(OPBFT, 1.f)                  \
    X(OPBKT, 1)                    \
    X(OPBKS, 0.f)                  \
    X(OPBVS, 1.f)                  \
    X(OPBDL, 0.f)                  \
    X(OPBAT, 0.000075f)            \
    X(OPBDE, 0.000075f)            \
    X(OPBSU, 1.f)                  \
    X(OPBST, 0.f)                  \
    X(OPBRE, 0.000075f)            \
    /* Operator C */               \
    X(OPCON, 0)                    \
    X(OPCWF, 0)                    \
    X(OPCCT, 50)                   \
    X(OPCFT, 1.f)                  \
    X(OPCKT, 1)                    \
    X(OPCKS, 0)                    \
    X(OPCVS, 1.f)                  \
    X(OPCDL, 0.f)                  \
    X(OPCAT, 0.000075f)            \
    X(OPCDE, 0.000075f)            \
    X(OPCSU, 1.f)                  \
    X(OPCST, 0.f)                  \
    X(OPCRE, 0.000075f)            \
    /* Operator D */               \
    X(OPDON, 0)                    \
    X(OPDWF, 0)                    \
    X(OPDCT, 50)                   \
    X(OPDFT, 1.f)                  \
    X(OPDKT, 1)                    \
    X(OPDKS, 0)                    \
    X(OPDVS, 1.f)                  \
    X(OPDDL, 0.f)                  \
    X(OPDAT, 0.000075f)            \
    X(OPDDE, 0.000075f)            \
    X(OPDSU, 1.f)                  \
    X(OPDST, 0.f)                  \
    X(OPDRE, 0.000075f)            \
    /* Operator E */               \
    X(OPEON, 0)                    \
    X(OPEWF, 0)                    \
    X(OPECT, 50)                   \
    X(OPEFT, 1.f)                  \
    X(OPEKT, 1)                    \
    X(OPEKS, 0)                    \
    X(OPEVS, 1.f)                  \
    X(OPEDL, 0.f)                  \
    X(OPEAT, 0.000075f)            \
    X(OPEDE, 0.000075f)            \
    X(OPESU, 1.f)                  \
    X(OPEST, 0.f)                  \
    X(OPERE, 0.000075f)            \
    /* Operator F */               \
    X(OPFON, 0)                    \
    X(OPFWF, 0)                    \
    X(OPFCT, 50)                   \
    X(OPFFT, 1.f)                  \
    X(OPFKT, 1)                    \
    X(OPFKS, 0)                    \
    X(OPFVS, 1.f)                  \
    X(OPFDL, 0.f)                  \
    X(OPFAT, 0.000075f)            \
    X(OPFDE, 0.000075f)            \
    X(OPFSU, 1.f)                  \
    X(OPFST, 0.f)                  \
    X(OPFRE, 0.000075f)            \
    /* Operator X */               \
    X(OPXON, 0)                    \
    X(OPXCU, 50)                   \
    X(OPXRS, 0.5f)                 \
    X(OPXAM, 1.f)                  \
    X(OPXBP, 0)                    \
    X(OPXKS, 0)                    \
    X(OPXVS, 1.f)                  \
    X(OPXDL, 0.f)                  \
    X(OPXAT, 0.000075f)            \
    X(OPXDE, 0.000075f)            \
    X(OPXSU, 1.f)                  \
    X(OPXST, 0.f)                  \
    X(OPXRE, 0.000075f)            \
    /* Operator Z */               \
    X(OPZON, 0)                    \
    X(OPZCU, 0)                    \
    X(OPZRS, 0)                    \
    X(OPZTY, 0)                    \
    X(OPZKT, 1)                    \
    X(OPZKS, 0)                    \
    X(OPZVS, 1.f)                  \
    X(OPZDL, 0.f)                  \
    X(OPZAT, 0.000075f)            \
    X(OPZDE, 0.000075f)            \
    X(OPZSU, 1.f)                  \
    X(OPZST, 0.f)                  \
    X(OPZRE, 0.000075f)            \
    /* Half matrix */              \
    X(MAA, 0.f)                    \
    X(MAB, 0.f)                    \
    X(MAC, 0.f)                    \
    X(MAD, 0.f)                    \
    X(MAE, 0.f)                    \
    X(MAF, 0.f)                    \
    X(MAX, 0.f)                    \
    X(MAZ, 0.f)                    \
    X(MAO, 0.f)                    \
    X(MAP, 0.f)                    \
    X(MBB, 0.f)                    \
    X(MBC, 0.f)                    \
    X(MBD, 0.f)                    \
    X(MBE, 0.f)                    \
    X(MBF, 0.f)                    \
    X(MBX, 0.f)                    \
    X(MBZ, 0.f)                    \
    X(MBO, 0.f)                    \
    X(MBP, 0.f)                    \
    X(MCC, 0.f)                    \
    X(MCD, 0.f)                    \
    X(MCE, 0.f)                    \
    X(MCF, 0.f)                    \
    X(MCX, 0.f)                    \
    X(MCZ, 0.f)                    \
    X(MCO, 0.f)                    \
    X(MCP, 0.f)                    \
    X(MDD, 0.f)                    \
    X(MDE, 0.f)                    \
    X(MDF, 0.f)                    \
    X(MDX, 0.f)                    \
    X(MDZ, 0.f)                    \
    X(MDO, 0.f)                    \
    X(MDP, 0.f)                    \
    X(MEE, 0.f)                    \
    X(MEF, 0.f)                    \
    X(MEX, 0.f)                    \
    X(MEZ, 0.f)                    \
    X(MEO, 0.f)                    \
    X(MEP, 0.f)                    \
    X(MFF, 0.f)                    \
    X(MFX, 0.f)                    \
    X(MFZ, 0.f)                    \
    X(MFO, 0.f)                    \
    X(MFP, 0.f)                    \
    X(MXZ, 0.f)                    \
    X(MXO, 0.f)                    \
    X(MXP, 0.f)                    \
    X(MZO, 0.f)                    \
    X(MZP, 0.f)                    \
    /* LFO */                      \
    X(LFOWF, 0.f)                  \
    X(LFORA, 0.f)                  \
    X(LFODE, 0.f)                  \
    X(LFODL, 0.000075f)            \
    X(LFODS, 0.f)                  \
    /* Pitch */                    \
    X(PORTA, 0.f)                  \
    X(PTCCU, 1.f)                  \
    X(PTCTI, 0.f)                  \
    /* Modulation wheel */         \
    X(MDLDS, 6.f)                  \
    /* High quality */             \
    X(HQ, 1.f)

// Global (bank-wide) effect parameters.
#define GLOBAL_PARAMETERS(X)       \
    X(REVTI, 0.8f)                 \
    X(REVDA, 0.5f)                 \
    X(DLYTI, 0.f)                  \
    X(DLYFE, 0.f)                  \
    X(DLYLF, 0.f)                  \
    X(DLYLA, 0.f)

float CPrograms::SetDefault(char channel, int par)
{
    SProgram *p = &prgbuffer[channel];
    SGlobal  *g = &currentbank->global;
    switch (par)
    {
#define PROGRAM_DEFAULT(name, value) case name: p->name = value; return p->name;
        PROGRAM_PARAMETERS(PROGRAM_DEFAULT)
#undef PROGRAM_DEFAULT
#define GLOBAL_DEFAULT(name, value) case name: g->name = value; return g->name;
        GLOBAL_PARAMETERS(GLOBAL_DEFAULT)
#undef GLOBAL_DEFAULT
        default: return 0.f;
    }
}

float CPrograms::GetPar(char channel, int par)
{
    SProgram *p = &prgbuffer[channel];
    SGlobal  *g = &currentbank->global;
    switch (par)
    {
#define PROGRAM_GET(name, value) case name: return p->name;
        PROGRAM_PARAMETERS(PROGRAM_GET)
#undef PROGRAM_GET
#define GLOBAL_GET(name, value) case name: return g->name;
        GLOBAL_PARAMETERS(GLOBAL_GET)
#undef GLOBAL_GET
        default: return 0;
    }
}

void CPrograms::SetPar(char channel, int par, float val)
{
    isWaiting[channel] = false;
    SProgram *p = &prgbuffer[channel];
    SGlobal  *g = &currentbank->global;
    switch (par)
    {
#define PROGRAM_SET(name, value) case name: p->name = val; return;
        PROGRAM_PARAMETERS(PROGRAM_SET)
#undef PROGRAM_SET
#define GLOBAL_SET(name, value) case name: g->name = val; return;
        GLOBAL_PARAMETERS(GLOBAL_SET)
#undef GLOBAL_SET
        default: return;
    }
}

bool CPrograms::GetStandBy(char channel)
{
    return isWaiting[channel];
}

void CPrograms::SetStandBy(char channel, bool isWaiting)
{
    this->isWaiting[channel] = isWaiting;
}

bool CPrograms::IsEditingName()
{
    return isEditingName;
}

void CPrograms::SetEditingName(bool isEditingName)
{
    this->isEditingName = isEditingName;
}

bool CPrograms::GetBankMode()
{
    return bankMode;
}

void CPrograms::SetBankMode(bool bankMode)
{
    this->bankMode = bankMode;
}

void CPrograms::GetProgName(char* str, char channel)
{
    GetName(str, prgbuffer[channel].PNAME);
}

void CPrograms::SetProgName(char* str, char channel)
{
    SetName(prgbuffer[channel].PNAME, str);
    NotifyDisplay();
}

void CPrograms::GetProgName(char* str, int numpg)
{
    GetName(str, currentbank->prg[numpg].PNAME);
}

void CPrograms::SetProgName(char* str, int numpg)
{
    SetName(currentbank->prg[numpg].PNAME, str);
    MarkChanged();
}

void CPrograms::GetBankName(char *str)
{
    persist.GetBankName(str, iCurrentBank);
}

void CPrograms::GetBankName(char *str, int nbank)
{
    persist.GetBankName(str, nbank);
}

int CPrograms::GetBankCount()
{
    return persist.GetNumberBanks();
}

int CPrograms::GetBankIndex()
{
    return iCurrentBank;
}

unsigned char CPrograms::GetNumProgr(char channel)
{
    if (isWaiting[channel])
        return prgtmp;
    else
        return numprg[channel];
}

void CPrograms::StoreProgram(char channel)
{
    if (!isWaiting[channel])
    {
        prgtmp = numprg[channel];
        isWaiting[channel] = true;
    }
    else
    {
        isWaiting[channel] = false;
        // Commit the edit buffer to the destination slot in the bank
        memcpy(&currentbank->prg[prgtmp], &prgbuffer[channel], sizeof(SProgram));
        numprg[channel] = prgtmp;
        NotifyDisplay();
    }
}

void CPrograms::CopyProgram(int destination, int source)
{
    memcpy(&currentbank->prg[destination], &currentbank->prg[source], sizeof(SProgram));
    for (int i = 0; i < MIDICHANNELS; i++)
    {
        if (numprg[i] == destination)
        {
            ReloadBuffer(i);
        }
    }
    MarkChanged();
}

void CPrograms::SetBank(SBank *bank)
{
    iCurrentBank = persist.AddBank((void*)bank, sizeof(SBank), (char*)"Bank from Host", true);
    currentbank = (SBank*)persist.GetSoundBank(iCurrentBank);
    for (int i = 0; i < MIDICHANNELS; i++)
    {
        SetNumProgr(i, numprg[i]);
    }
    MarkChanged();
}

void CPrograms::SetProgram(char num, SProgram *program)
{
    memcpy(&currentbank->prg[num], program, sizeof(SProgram));
    for (int i = 0; i < MIDICHANNELS; i++)
    {
        if (numprg[i] == num)
        {
            SetNumProgr(i, num);
        }
    }
    MarkChanged();
}

void CPrograms::SetEditBuffer(char channel, const SProgram *program)
{
    if (channel < 0 || channel >= MIDICHANNELS || !program)
        return;
    memcpy(&prgbuffer[(int)channel], program, sizeof(SProgram));
    MarkChanged();
}

bool CPrograms::HasChanges()
{
    if (haschanges)
    {
        haschanges = false;
        return true;
    }
    return false;
}

void CPrograms::SetHostInterface(CHostInterface *hostinterface)
{
    this->hostinterface = hostinterface;
}
#endif
