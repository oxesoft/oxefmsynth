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

#include "constants.h"
#ifdef __OXEDMO__
#include "soundbank.h"
#else
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "persist.h"
#endif
#include "programs.h"
#include <string.h>

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
	iCurrentBank  = 0;
	isEditingName = false;
	bankMode      = false;
	haschanges    = false;
	if (persist.GetNumberBanks()) // always >= 2 since version 1.2.0
	{
		iCurrentBank = 0;
		currentbank  = (SBank*)persist.GetSoundBank(iCurrentBank);
	}
	hWndEditor = NULL;
#else
	currentbank = (SBank*)soundbank;
#endif
}

SProgram* CPrograms::GetProgram(char channel)
{
	return &currentbank->prg[numprg[channel]];
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
	}
#ifndef __OXEDMO__
	haschanges = true;
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
	haschanges = true;
	PostMessage((HWND)hWndEditor, WM_UPDATE_DISPLAY, 0, 0);
#endif
}

SBank* CPrograms::GetBank(void)
{
	return currentbank;
}

#ifndef __OXEDMO__

float CPrograms::SetDefault(char channel, int par)
{
	SProgram *p = &currentbank->prg[numprg[channel]];
	SGlobal  *g = &currentbank->global;
	switch (par)
	{
		case OPAON: p->OPAON = 1;         return p->OPAON;
		case OPAWF: p->OPAWF = 0;         return p->OPAWF;
		case OPACT: p->OPACT = 50;        return p->OPACT;
		case OPAFT: p->OPAFT = 1.f;       return p->OPAFT;
		case OPAKT: p->OPAKT = 1;         return p->OPAKT;
		case OPAKS: p->OPAKS = 0;         return p->OPAKS;
		case OPAVS: p->OPAVS = 1.f;       return p->OPAVS;
		case OPADL: p->OPADL = 0.f;       return p->OPADL;
		case OPAAT: p->OPAAT = 0.000075f; return p->OPAAT;
		case OPADE: p->OPADE = 0.000075f; return p->OPADE;
		case OPASU: p->OPASU = 0.7f;      return p->OPASU;
		case OPAST: p->OPAST = 0.f;       return p->OPAST;
		case OPARE: p->OPARE = 0.000075f; return p->OPARE;
		case OPBON: p->OPBON = 0;         return p->OPBON;
		case OPBWF: p->OPBWF = 0;         return p->OPBWF;
		case OPBCT: p->OPBCT = 50;        return p->OPBCT;
		case OPBFT: p->OPBFT = 1.f;       return p->OPBFT;
		case OPBKT: p->OPBKT = 1;         return p->OPBKT;
		case OPBKS: p->OPBKS = 0.f;       return p->OPBKS;
		case OPBVS: p->OPBVS = 1.f;       return p->OPBVS;
		case OPBDL: p->OPBDL = 0.f;       return p->OPBDL;
		case OPBAT: p->OPBAT = 0.000075f; return p->OPBAT;
		case OPBDE: p->OPBDE = 0.000075f; return p->OPBDE;
		case OPBSU: p->OPBSU = 0.7f;      return p->OPBSU;
		case OPBST: p->OPBST = 0.f;       return p->OPBST;
		case OPBRE: p->OPBRE = 0.000075f; return p->OPBRE;
		case OPCON: p->OPCON = 0;         return p->OPCON;
		case OPCWF: p->OPCWF = 0;         return p->OPCWF;
		case OPCCT: p->OPCCT = 50;        return p->OPCCT;
		case OPCFT: p->OPCFT = 1.f;       return p->OPCFT;
		case OPCKT: p->OPCKT = 1;         return p->OPCKT;
		case OPCKS: p->OPCKS = 0;         return p->OPCKS;
		case OPCVS: p->OPCVS = 1.f;       return p->OPCVS;
		case OPCDL: p->OPCDL = 0.f;       return p->OPCDL;
		case OPCAT: p->OPCAT = 0.000075f; return p->OPCAT;
		case OPCDE: p->OPCDE = 0.000075f; return p->OPCDE;
		case OPCSU: p->OPCSU = 0.7f;      return p->OPCSU;
		case OPCST: p->OPCST = 0.f;       return p->OPCST;
		case OPCRE: p->OPCRE = 0.000075f; return p->OPCRE;
		case OPDON: p->OPDON = 0;         return p->OPDON;
		case OPDWF: p->OPDWF = 0;         return p->OPDWF;
		case OPDCT: p->OPDCT = 50;        return p->OPDCT;
		case OPDFT: p->OPDFT = 1.f;       return p->OPDFT;
		case OPDKT: p->OPDKT = 1;         return p->OPDKT;
		case OPDKS: p->OPDKS = 0;         return p->OPDKS;
		case OPDVS: p->OPDVS = 1.f;       return p->OPDVS;
		case OPDDL: p->OPDDL = 0.f;       return p->OPDDL;
		case OPDAT: p->OPDAT = 0.000075f; return p->OPDAT;
		case OPDDE: p->OPDDE = 0.000075f; return p->OPDDE;
		case OPDSU: p->OPDSU = 0.7f;      return p->OPDSU;
		case OPDST: p->OPDST = 0.f;       return p->OPDST;
		case OPDRE: p->OPDRE = 0.000075f; return p->OPDRE;
		case OPEON: p->OPEON = 0;         return p->OPEON;
		case OPEWF: p->OPEWF = 0;         return p->OPEWF;
		case OPECT: p->OPECT = 50;        return p->OPECT;
		case OPEFT: p->OPEFT = 1.f;       return p->OPEFT;
		case OPEKT: p->OPEKT = 1;         return p->OPEKT;
		case OPEKS: p->OPEKS = 0;         return p->OPEKS;
		case OPEVS: p->OPEVS = 1.f;       return p->OPEVS;
		case OPEDL: p->OPEDL = 0.f;       return p->OPEDL;
		case OPEAT: p->OPEAT = 0.000075f; return p->OPEAT;
		case OPEDE: p->OPEDE = 0.000075f; return p->OPEDE;
		case OPESU: p->OPESU = 0.7f;      return p->OPESU;
		case OPEST: p->OPEST = 0.f;       return p->OPEST;
		case OPERE: p->OPERE = 0.000075f; return p->OPERE;
		case OPFON: p->OPFON = 0;         return p->OPFON;
		case OPFWF: p->OPFWF = 0;         return p->OPFWF;
		case OPFCT: p->OPFCT = 50;        return p->OPFCT;
		case OPFFT: p->OPFFT = 1.f;       return p->OPFFT;
		case OPFKT: p->OPFKT = 1;         return p->OPFKT;
		case OPFKS: p->OPFKS = 0;         return p->OPFKS;
		case OPFVS: p->OPFVS = 1.f;       return p->OPFVS;
		case OPFDL: p->OPFDL = 0.f;       return p->OPFDL;
		case OPFAT: p->OPFAT = 0.000075f; return p->OPFAT;
		case OPFDE: p->OPFDE = 0.000075f; return p->OPFDE;
		case OPFSU: p->OPFSU = 0.7f;      return p->OPFSU;
		case OPFST: p->OPFST = 0.f;       return p->OPFST;
		case OPFRE: p->OPFRE = 0.000075f; return p->OPFRE;
		case OPXON: p->OPXON = 0;         return p->OPXON;
		case OPXCU: p->OPXCU = 50;        return p->OPXCU;
		case OPXRS: p->OPXRS = 0.5f;      return p->OPXRS;
		case OPXAM: p->OPXAM = 1.f;       return p->OPXAM;
		case OPXBP: p->OPXBP = 0;         return p->OPXBP;
		case OPXKS: p->OPXKS = 0;         return p->OPXKS;
		case OPXVS: p->OPXVS = 1.f;       return p->OPXVS;
		case OPXDL: p->OPXDL = 0.f;       return p->OPXDL;
		case OPXAT: p->OPXAT = 0.000075f; return p->OPXAT;
		case OPXDE: p->OPXDE = 0.000075f; return p->OPXDE;
		case OPXSU: p->OPXSU = 0.7f;      return p->OPXSU;
		case OPXST: p->OPXST = 0.f;       return p->OPXST;
		case OPXRE: p->OPXRE = 0.000075f; return p->OPXRE;
		case OPZON: p->OPZON = 0;         return p->OPZON;
		case OPZCU: p->OPZCU = 0;         return p->OPZCU;
		case OPZRS: p->OPZRS = 0;         return p->OPZRS;
		case OPZTY: p->OPZTY = 0;         return p->OPZTY;
		case OPZKT: p->OPZKT = 1;         return p->OPZKT;
		case OPZKS: p->OPZKS = 0;         return p->OPZKS;
		case OPZVS: p->OPZVS = 1.f;       return p->OPZVS;
		case OPZDL: p->OPZDL = 0.f;       return p->OPZDL;
		case OPZAT: p->OPZAT = 0.000075f; return p->OPZAT;
		case OPZDE: p->OPZDE = 0.000075f; return p->OPZDE;
		case OPZSU: p->OPZSU = 0.7f;      return p->OPZSU;
		case OPZST: p->OPZST = 0.f;       return p->OPZST;
		case OPZRE: p->OPZRE = 0.000075f; return p->OPZRE;
		case MAA:   p->MAA   = 0.f;       return p->MAA  ;
		case MAB:   p->MAB   = 0.f;       return p->MAB  ;
		case MAC:   p->MAC   = 0.f;       return p->MAC  ;
		case MAD:   p->MAD   = 0.f;       return p->MAD  ;
		case MAE:   p->MAE   = 0.f;       return p->MAE  ;
		case MAF:   p->MAF   = 0.f;       return p->MAF  ;
		case MAX:   p->MAX   = 0.f;       return p->MAX  ;
		case MAZ:   p->MAZ   = 0.f;       return p->MAZ  ;
		case MAO:   p->MAO   = 0.f;       return p->MAO  ;
		case MAP:   p->MAP   = 0.f;       return p->MAP  ;
		case MBB:   p->MBB   = 0.f;       return p->MBB  ;
		case MBC:   p->MBC   = 0.f;       return p->MBC  ;
		case MBD:   p->MBD   = 0.f;       return p->MBD  ;
		case MBE:   p->MBE   = 0.f;       return p->MBE  ;
		case MBF:   p->MBF   = 0.f;       return p->MBF  ;
		case MBX:   p->MBX   = 0.f;       return p->MBX  ;
		case MBZ:   p->MBZ   = 0.f;       return p->MBZ  ;
		case MBO:   p->MBO   = 0.f;       return p->MBO  ;
		case MBP:   p->MBP   = 0.f;       return p->MBP  ;
		case MCC:   p->MCC   = 0.f;       return p->MCC  ;
		case MCD:   p->MCD   = 0.f;       return p->MCD  ;
		case MCE:   p->MCE   = 0.f;       return p->MCE  ;
		case MCF:   p->MCF   = 0.f;       return p->MCF  ;
		case MCX:   p->MCX   = 0.f;       return p->MCX  ;
		case MCZ:   p->MCZ   = 0.f;       return p->MCZ  ;
		case MCO:   p->MCO   = 0.f;       return p->MCO  ;
		case MCP:   p->MCP   = 0.f;       return p->MCP  ;
		case MDD:   p->MDD   = 0.f;       return p->MDD  ;
		case MDE:   p->MDE   = 0.f;       return p->MDE  ;
		case MDF:   p->MDF   = 0.f;       return p->MDF  ;
		case MDX:   p->MDX   = 0.f;       return p->MDX  ;
		case MDZ:   p->MDZ   = 0.f;       return p->MDZ  ;
		case MDO:   p->MDO   = 0.f;       return p->MDO  ;
		case MDP:   p->MDP   = 0.f;       return p->MDP  ;
		case MEE:   p->MEE   = 0.f;       return p->MEE  ;
		case MEF:   p->MEF   = 0.f;       return p->MEF  ;
		case MEX:   p->MEX   = 0.f;       return p->MEX  ;
		case MEZ:   p->MEZ   = 0.f;       return p->MEZ  ;
		case MEO:   p->MEO   = 0.f;       return p->MEO  ;
		case MEP:   p->MEP   = 0.f;       return p->MEP  ;
		case MFF:   p->MFF   = 0.f;       return p->MFF  ;
		case MFX:   p->MFX   = 0.f;       return p->MFX  ;
		case MFZ:   p->MFZ   = 0.f;       return p->MFZ  ;
		case MFO:   p->MFO   = 0.f;       return p->MFO  ;
		case MFP:   p->MFP   = 0.f;       return p->MFP  ;
		case MXZ:   p->MXZ   = 0.f;       return p->MXZ  ;
		case MXO:   p->MXO   = 0.f;       return p->MXO  ;
		case MXP:   p->MXP   = 0.f;       return p->MXP  ;
		case MZO:   p->MZO   = 0.f;       return p->MZO  ;
		case MZP:   p->MZP   = 0.f;       return p->MZP  ;
		case LFOWF: p->LFOWF = 0.f;       return p->LFOWF;
		case LFORA: p->LFORA = 0.f;       return p->LFORA;
		case LFODE: p->LFODE = 0.f;       return p->LFODE;
		case LFODL: p->LFODL = 0.000075f; return p->LFODL;
		case LFODS: p->LFODS = 0.f;       return p->LFODS;
		case PORTA: p->PORTA = 0.f;       return p->PORTA;
		case PTCCU: p->PTCCU = 1.f;       return p->PTCCU;
		case PTCTI: p->PTCTI = 0.f;       return p->PTCTI;
		case MDLDS: p->MDLDS = 6.f;       return p->MDLDS;
		case HQ   : p->HQ    = 1.f;       return p->HQ   ;
		case REVTI: g->REVTI = 0.8f;      return g->REVTI;
		case REVDA: g->REVDA = 0.5f;      return g->REVDA;
		case DLYTI: g->DLYTI = 0.f;       return g->DLYTI;
		case DLYFE: g->DLYFE = 0.f;       return g->DLYFE;
		case DLYLF: g->DLYLF = 0.f;       return g->DLYLF;
		case DLYLA: g->DLYLA = 0.f;       return g->DLYLA;
		default:                          return      0.f;
	}
}

float CPrograms::GetPar(char channel, int par)
{
	SProgram *p = &currentbank->prg[numprg[channel]];
	SGlobal  *g = &currentbank->global;
	switch (par)
	{
		case OPAON: return p->OPAON;
		case OPAWF: return p->OPAWF;
		case OPACT: return p->OPACT;
		case OPAFT: return p->OPAFT;
		case OPAKT: return p->OPAKT;
		case OPAKS: return p->OPAKS;
		case OPAVS: return p->OPAVS;
		case OPADL: return p->OPADL;
		case OPAAT: return p->OPAAT;
		case OPADE: return p->OPADE;
		case OPASU: return p->OPASU;
		case OPAST: return p->OPAST;
		case OPARE: return p->OPARE;
		case OPBON: return p->OPBON;
		case OPBWF: return p->OPBWF;
		case OPBCT: return p->OPBCT;
		case OPBFT: return p->OPBFT;
		case OPBKT: return p->OPBKT;
		case OPBKS: return p->OPBKS;
		case OPBVS: return p->OPBVS;
		case OPBDL: return p->OPBDL;
		case OPBAT: return p->OPBAT;
		case OPBDE: return p->OPBDE;
		case OPBSU: return p->OPBSU;
		case OPBST: return p->OPBST;
		case OPBRE: return p->OPBRE;
		case OPCON: return p->OPCON;
		case OPCWF: return p->OPCWF;
		case OPCCT: return p->OPCCT;
		case OPCFT: return p->OPCFT;
		case OPCKT: return p->OPCKT;
		case OPCKS: return p->OPCKS;
		case OPCVS: return p->OPCVS;
		case OPCDL: return p->OPCDL;
		case OPCAT: return p->OPCAT;
		case OPCDE: return p->OPCDE;
		case OPCSU: return p->OPCSU;
		case OPCST: return p->OPCST;
		case OPCRE: return p->OPCRE;
		case OPDON: return p->OPDON;
		case OPDWF: return p->OPDWF;
		case OPDCT: return p->OPDCT;
		case OPDFT: return p->OPDFT;
		case OPDKT: return p->OPDKT;
		case OPDKS: return p->OPDKS;
		case OPDVS: return p->OPDVS;
		case OPDDL: return p->OPDDL;
		case OPDAT: return p->OPDAT;
		case OPDDE: return p->OPDDE;
		case OPDSU: return p->OPDSU;
		case OPDST: return p->OPDST;
		case OPDRE: return p->OPDRE;
		case OPEON: return p->OPEON;
		case OPEWF: return p->OPEWF;
		case OPECT: return p->OPECT;
		case OPEFT: return p->OPEFT;
		case OPEKT: return p->OPEKT;
		case OPEKS: return p->OPEKS;
		case OPEVS: return p->OPEVS;
		case OPEDL: return p->OPEDL;
		case OPEAT: return p->OPEAT;
		case OPEDE: return p->OPEDE;
		case OPESU: return p->OPESU;
		case OPEST: return p->OPEST;
		case OPERE: return p->OPERE;
		case OPFON: return p->OPFON;
		case OPFWF: return p->OPFWF;
		case OPFCT: return p->OPFCT;
		case OPFFT: return p->OPFFT;
		case OPFKT: return p->OPFKT;
		case OPFKS: return p->OPFKS;
		case OPFVS: return p->OPFVS;
		case OPFDL: return p->OPFDL;
		case OPFAT: return p->OPFAT;
		case OPFDE: return p->OPFDE;
		case OPFSU: return p->OPFSU;
		case OPFST: return p->OPFST;
		case OPFRE: return p->OPFRE;
		case OPXON: return p->OPXON;
		case OPXCU: return p->OPXCU;
		case OPXRS: return p->OPXRS;
		case OPXAM: return p->OPXAM;
		case OPXBP: return p->OPXBP;
		case OPXKS: return p->OPXKS;
		case OPXVS: return p->OPXVS;
		case OPXDL: return p->OPXDL;
		case OPXAT: return p->OPXAT;
		case OPXDE: return p->OPXDE;
		case OPXSU: return p->OPXSU;
		case OPXST: return p->OPXST;
		case OPXRE: return p->OPXRE;
		case OPZON: return p->OPZON;
		case OPZCU: return p->OPZCU;
		case OPZRS: return p->OPZRS;
		case OPZTY: return p->OPZTY;
		case OPZKT: return p->OPZKT;
		case OPZKS: return p->OPZKS;
		case OPZVS: return p->OPZVS;
		case OPZDL: return p->OPZDL;
		case OPZAT: return p->OPZAT;
		case OPZDE: return p->OPZDE;
		case OPZSU: return p->OPZSU;
		case OPZST: return p->OPZST;
		case OPZRE: return p->OPZRE;
		case MAA:   return p->MAA  ;
		case MAB:   return p->MAB  ;
		case MAC:   return p->MAC  ;
		case MAD:   return p->MAD  ;
		case MAE:   return p->MAE  ;
		case MAF:   return p->MAF  ;
		case MAX:   return p->MAX  ;
		case MAZ:   return p->MAZ  ;
		case MAO:   return p->MAO  ;
		case MAP:   return p->MAP  ;
		case MBB:   return p->MBB  ;
		case MBC:   return p->MBC  ;
		case MBD:   return p->MBD  ;
		case MBE:   return p->MBE  ;
		case MBF:   return p->MBF  ;
		case MBX:   return p->MBX  ;
		case MBZ:   return p->MBZ  ;
		case MBO:   return p->MBO  ;
		case MBP:   return p->MBP  ;
		case MCC:   return p->MCC  ;
		case MCD:   return p->MCD  ;
		case MCE:   return p->MCE  ;
		case MCF:   return p->MCF  ;
		case MCX:   return p->MCX  ;
		case MCZ:   return p->MCZ  ;
		case MCO:   return p->MCO  ;
		case MCP:   return p->MCP  ;
		case MDD:   return p->MDD  ;
		case MDE:   return p->MDE  ;
		case MDF:   return p->MDF  ;
		case MDX:   return p->MDX  ;
		case MDZ:   return p->MDZ  ;
		case MDO:   return p->MDO  ;
		case MDP:   return p->MDP  ;
		case MEE:   return p->MEE  ;
		case MEF:   return p->MEF  ;
		case MEX:   return p->MEX  ;
		case MEZ:   return p->MEZ  ;
		case MEO:   return p->MEO  ;
		case MEP:   return p->MEP  ;
		case MFF:   return p->MFF  ;
		case MFX:   return p->MFX  ;
		case MFZ:   return p->MFZ  ;
		case MFO:   return p->MFO  ;
		case MFP:   return p->MFP  ;
		case MXZ:   return p->MXZ  ;
		case MXO:   return p->MXO  ;
		case MXP:   return p->MXP  ;
		case MZO:   return p->MZO  ;
		case MZP:   return p->MZP  ;
		case LFOWF: return p->LFOWF;
		case LFORA: return p->LFORA;
		case LFODE: return p->LFODE;
		case LFODL: return p->LFODL;
		case LFODS: return p->LFODS;
		case PORTA: return p->PORTA;
		case PTCCU: return p->PTCCU;
		case PTCTI: return p->PTCTI;
		case MDLDS: return p->MDLDS;
		case HQ   : return p->HQ   ;
		case REVTI: return g->REVTI;
		case REVDA: return g->REVDA;
		case DLYTI: return g->DLYTI;
		case DLYFE: return g->DLYFE;
		case DLYLF: return g->DLYLF;
		case DLYLA: return g->DLYLA;
		default:    return        0;
	}
}

void CPrograms::SetPar(char channel, int par, float val)
{
	isWaiting[channel] = false;
	SProgram *p = &currentbank->prg[numprg[channel]];
	SGlobal  *g = &currentbank->global;
	switch (par)
	{
		case OPAON: p->OPAON = val; return;
		case OPAWF: p->OPAWF = val; return;
		case OPACT: p->OPACT = val; return;
		case OPAFT: p->OPAFT = val; return;
		case OPAKT: p->OPAKT = val; return;
		case OPAKS: p->OPAKS = val; return;
		case OPAVS: p->OPAVS = val; return;
		case OPADL: p->OPADL = val; return;
		case OPAAT: p->OPAAT = val; return;
		case OPADE: p->OPADE = val; return;
		case OPASU: p->OPASU = val; return;
		case OPAST: p->OPAST = val; return;
		case OPARE: p->OPARE = val; return;
		case OPBON: p->OPBON = val; return;
		case OPBWF: p->OPBWF = val; return;
		case OPBCT: p->OPBCT = val; return;
		case OPBFT: p->OPBFT = val; return;
		case OPBKT: p->OPBKT = val; return;
		case OPBKS: p->OPBKS = val; return;
		case OPBVS: p->OPBVS = val; return;
		case OPBDL: p->OPBDL = val; return;
		case OPBAT: p->OPBAT = val; return;
		case OPBDE: p->OPBDE = val; return;
		case OPBSU: p->OPBSU = val; return;
		case OPBST: p->OPBST = val; return;
		case OPBRE: p->OPBRE = val; return;
		case OPCON: p->OPCON = val; return;
		case OPCWF: p->OPCWF = val; return;
		case OPCCT: p->OPCCT = val; return;
		case OPCFT: p->OPCFT = val; return;
		case OPCKT: p->OPCKT = val; return;
		case OPCKS: p->OPCKS = val; return;
		case OPCVS: p->OPCVS = val; return;
		case OPCDL: p->OPCDL = val; return;
		case OPCAT: p->OPCAT = val; return;
		case OPCDE: p->OPCDE = val; return;
		case OPCSU: p->OPCSU = val; return;
		case OPCST: p->OPCST = val; return;
		case OPCRE: p->OPCRE = val; return;
		case OPDON: p->OPDON = val; return;
		case OPDWF: p->OPDWF = val; return;
		case OPDCT: p->OPDCT = val; return;
		case OPDFT: p->OPDFT = val; return;
		case OPDKT: p->OPDKT = val; return;
		case OPDKS: p->OPDKS = val; return;
		case OPDVS: p->OPDVS = val; return;
		case OPDDL: p->OPDDL = val; return;
		case OPDAT: p->OPDAT = val; return;
		case OPDDE: p->OPDDE = val; return;
		case OPDSU: p->OPDSU = val; return;
		case OPDST: p->OPDST = val; return;
		case OPDRE: p->OPDRE = val; return;
		case OPEON: p->OPEON = val; return;
		case OPEWF: p->OPEWF = val; return;
		case OPECT: p->OPECT = val; return;
		case OPEFT: p->OPEFT = val; return;
		case OPEKT: p->OPEKT = val; return;
		case OPEKS: p->OPEKS = val; return;
		case OPEVS: p->OPEVS = val; return;
		case OPEDL: p->OPEDL = val; return;
		case OPEAT: p->OPEAT = val; return;
		case OPEDE: p->OPEDE = val; return;
		case OPESU: p->OPESU = val; return;
		case OPEST: p->OPEST = val; return;
		case OPERE: p->OPERE = val; return;
		case OPFON: p->OPFON = val; return;
		case OPFWF: p->OPFWF = val; return;
		case OPFCT: p->OPFCT = val; return;
		case OPFFT: p->OPFFT = val; return;
		case OPFKT: p->OPFKT = val; return;
		case OPFKS: p->OPFKS = val; return;
		case OPFVS: p->OPFVS = val; return;
		case OPFDL: p->OPFDL = val; return;
		case OPFAT: p->OPFAT = val; return;
		case OPFDE: p->OPFDE = val; return;
		case OPFSU: p->OPFSU = val; return;
		case OPFST: p->OPFST = val; return;
		case OPFRE: p->OPFRE = val; return;
		case OPXON: p->OPXON = val; return;
		case OPXCU: p->OPXCU = val; return;
		case OPXRS: p->OPXRS = val; return;
		case OPXAM: p->OPXAM = val; return;
		case OPXBP: p->OPXBP = val; return;
		case OPXKS: p->OPXKS = val; return;
		case OPXVS: p->OPXVS = val; return;
		case OPXDL: p->OPXDL = val; return;
		case OPXAT: p->OPXAT = val; return;
		case OPXDE: p->OPXDE = val; return;
		case OPXSU: p->OPXSU = val; return;
		case OPXST: p->OPXST = val; return;
		case OPXRE: p->OPXRE = val; return;
		case OPZON: p->OPZON = val; return;
		case OPZCU: p->OPZCU = val; return;
		case OPZRS: p->OPZRS = val; return;
		case OPZTY: p->OPZTY = val; return;
		case OPZKT: p->OPZKT = val; return;
		case OPZKS: p->OPZKS = val; return;
		case OPZVS: p->OPZVS = val; return;
		case OPZDL: p->OPZDL = val; return;
		case OPZAT: p->OPZAT = val; return;
		case OPZDE: p->OPZDE = val; return;
		case OPZSU: p->OPZSU = val; return;
		case OPZST: p->OPZST = val; return;
		case OPZRE: p->OPZRE = val; return;
		case MAA:   p->MAA   = val; return;
		case MAB:   p->MAB   = val; return;
		case MAC:   p->MAC   = val; return;
		case MAD:   p->MAD   = val; return;
		case MAE:   p->MAE   = val; return;
		case MAF:   p->MAF   = val; return;
		case MAX:   p->MAX   = val; return;
		case MAZ:   p->MAZ   = val; return;
		case MAO:   p->MAO   = val; return;
		case MAP:   p->MAP   = val; return;
		case MBB:   p->MBB   = val; return;
		case MBC:   p->MBC   = val; return;
		case MBD:   p->MBD   = val; return;
		case MBE:   p->MBE   = val; return;
		case MBF:   p->MBF   = val; return;
		case MBX:   p->MBX   = val; return;
		case MBZ:   p->MBZ   = val; return;
		case MBO:   p->MBO   = val; return;
		case MBP:   p->MBP   = val; return;
		case MCC:   p->MCC   = val; return;
		case MCD:   p->MCD   = val; return;
		case MCE:   p->MCE   = val; return;
		case MCF:   p->MCF   = val; return;
		case MCX:   p->MCX   = val; return;
		case MCZ:   p->MCZ   = val; return;
		case MCO:   p->MCO   = val; return;
		case MCP:   p->MCP   = val; return;
		case MDD:   p->MDD   = val; return;
		case MDE:   p->MDE   = val; return;
		case MDF:   p->MDF   = val; return;
		case MDX:   p->MDX   = val; return;
		case MDZ:   p->MDZ   = val; return;
		case MDO:   p->MDO   = val; return;
		case MDP:   p->MDP   = val; return;
		case MEE:   p->MEE   = val; return;
		case MEF:   p->MEF   = val; return;
		case MEX:   p->MEX   = val; return;
		case MEZ:   p->MEZ   = val; return;
		case MEO:   p->MEO   = val; return;
		case MEP:   p->MEP   = val; return;
		case MFF:   p->MFF   = val; return;
		case MFX:   p->MFX   = val; return;
		case MFZ:   p->MFZ   = val; return;
		case MFO:   p->MFO   = val; return;
		case MFP:   p->MFP   = val; return;
		case MXZ:   p->MXZ   = val; return;
		case MXO:   p->MXO   = val; return;
		case MXP:   p->MXP   = val; return;
		case MZO:   p->MZO   = val; return;
		case MZP:   p->MZP   = val; return;
		case LFOWF: p->LFOWF = val; return;
		case LFORA: p->LFORA = val; return;
		case LFODE: p->LFODE = val; return;
		case LFODL: p->LFODL = val; return;
		case LFODS: p->LFODS = val; return;
		case PORTA: p->PORTA = val; return;
		case PTCCU: p->PTCCU = val; return;
		case PTCTI: p->PTCTI = val; return;
		case MDLDS: p->MDLDS = val; return;
		case HQ   : p->HQ    = val; return;
		case REVTI: g->REVTI = val; return;
		case REVDA: g->REVDA = val; return;
		case DLYTI: g->DLYTI = val; return;
		case DLYFE: g->DLYFE = val; return;
		case DLYLF: g->DLYLF = val; return;
		case DLYLA: g->DLYLA = val; return;
		default:                    return;
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
	CopyMemory(str, currentbank->prg[numprg[channel]].PNAME, PG_NAME_SIZE);
}

void CPrograms::SetProgName(char* str, char channel)
{
	CopyMemory(currentbank->prg[numprg[channel]].PNAME, str, PG_NAME_SIZE);
	haschanges = true;
	PostMessage((HWND)hWndEditor, WM_UPDATE_DISPLAY, 0, 0);
}

void CPrograms::GetProgName(char* str, int numpg)
{
	CopyMemory(str, currentbank->prg[numpg].PNAME, PG_NAME_SIZE);
	str[PG_NAME_SIZE] = 0;
}

void CPrograms::SetProgName(char* str, int numpg)
{
	CopyMemory(currentbank->prg[numpg].PNAME, str, min(lstrlenA(str), PG_NAME_SIZE));
	for (int i=0;i<MIDICHANNELS;i++)
	{
		if (numprg[i] == numpg)
		{
			CopyMemory(currentbank->prg[numpg].PNAME, str, min(lstrlenA(str), PG_NAME_SIZE));
		}
	}
	haschanges = true;
	PostMessage((HWND)hWndEditor, WM_UPDATE_DISPLAY, 0, 0);
}

void CPrograms::GetBankName(char *str)
{
	persist.GetBankName(str, iCurrentBank);
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
		haschanges = true;
		isWaiting[channel] = false;
		CopyMemory(&currentbank->prg[prgtmp], &currentbank->prg[numprg[channel]],sizeof(SProgram));
		numprg[channel] = prgtmp;
		PostMessage((HWND)hWndEditor, WM_UPDATE_DISPLAY, 0, 0);
	}
}

void CPrograms::CopyProgram(int destination, int source)
{
	CopyMemory(&currentbank->prg[destination], &currentbank->prg[source], sizeof(SProgram));
	haschanges = true;
	PostMessage((HWND)hWndEditor, WM_UPDATE_DISPLAY, 0, 0);
}

void CPrograms::SetBank(SBank *bank)
{
	iCurrentBank = persist.AddBank((void*)bank, sizeof(SBank), (char*)"bank from host", true);
	currentbank = (SBank*)persist.GetSoundBank(iCurrentBank);
	for (int i = 0; i < MIDICHANNELS; i++)
	{
		SetNumProgr(i, numprg[i]);
	}
	haschanges = true;
	PostMessage((HWND)hWndEditor, WM_UPDATE_DISPLAY, 0, 0);
}

void CPrograms::SetProgram(char num, SProgram *program)
{
	CopyMemory(&currentbank->prg[num], program, sizeof(SProgram));
	for (int i = 0; i < MIDICHANNELS; i++)
	{
		if (numprg[i] == num)
		{
			SetNumProgr(i, num);
		}
	}
	haschanges = true;
	PostMessage((HWND)hWndEditor, WM_UPDATE_DISPLAY, 0, 0);
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

void CPrograms::SetEditorHn(void *hWndEditor)
{
	this->hWndEditor = hWndEditor;
}

#endif
