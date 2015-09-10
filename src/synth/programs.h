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

enum
{
	OPAON,
	OPAWF,
	OPACT,
	OPAFT,
	OPAKT,
	OPAKS,
	OPAVS,
	OPADL,
	OPAAT,
	OPADE,
	OPASU,
	OPAST,
	OPARE,
	OPBON,
	OPBWF,
	OPBCT,
	OPBFT,
	OPBKT,
	OPBKS,
	OPBVS,
	OPBDL,
	OPBAT,
	OPBDE,
	OPBSU,
	OPBST,
	OPBRE,
	OPCON,
	OPCWF,
	OPCCT,
	OPCFT,
	OPCKT,
	OPCKS,
	OPCVS,
	OPCDL,
	OPCAT,
	OPCDE,
	OPCSU,
	OPCST,
	OPCRE,
	OPDON,
	OPDWF,
	OPDCT,
	OPDFT,
	OPDKT,
	OPDKS,
	OPDVS,
	OPDDL,
	OPDAT,
	OPDDE,
	OPDSU,
	OPDST,
	OPDRE,
	OPEON,
	OPEWF,
	OPECT,
	OPEFT,
	OPEKT,
	OPEKS,
	OPEVS,
	OPEDL,
	OPEAT,
	OPEDE,
	OPESU,
	OPEST,
	OPERE,
	OPFON,
	OPFWF,
	OPFCT,
	OPFFT,
	OPFKT,
	OPFKS,
	OPFVS,
	OPFDL,
	OPFAT,
	OPFDE,
	OPFSU,
	OPFST,
	OPFRE,
	OPXON,
	OPXCU,
	OPXRS,
	OPXAM,
	OPXBP,
	OPXKS,
	OPXVS,
	OPXDL,
	OPXAT,
	OPXDE,
	OPXSU,
	OPXST,
	OPXRE,
	OPZON,
	OPZCU,
	OPZRS,
	OPZTY,
	OPZKT,
	OPZKS,
	OPZVS,
	OPZDL,
	OPZAT,
	OPZDE,
	OPZSU,
	OPZST,
	OPZRE,
	MAA,
	MAB,
	MAC,
	MAD,
	MAE,
	MAF,
	MAX,
	MAZ,
	MAO,
	MAP,
	MBB,
	MBC,
	MBD,
	MBE,
	MBF,
	MBX,
	MBZ,
	MBO,
	MBP,
	MCC,
	MCD,
	MCE,
	MCF,
	MCX,
	MCZ,
	MCO,
	MCP,
	MDD,
	MDE,
	MDF,
	MDX,
	MDZ,
	MDO,
	MDP,
	MEE,
	MEF,
	MEX,
	MEZ,
	MEO,
	MEP,
	MFF,
	MFX,
	MFZ,
	MFO,
	MFP,
	MXZ,
	MXO,
	MXP,
	MZO,
	MZP,
	LFOWF,
	LFORA,
	LFODE,
	LFODL,
	LFODS,
	PORTA,
	PTCCU,
	PTCTI,
	MDLDS,
	HQ,
	REVTI,
	REVDA,
	DLYTI,
	DLYFE,
	DLYLF,
	DLYLA,
	RVBLV,
	DLYLV,
	PARAMETERS_COUNT
};

struct SProgram
{
	char  PNAME[PG_NAME_SIZE];
	// Operator A
	float OPAON;  // On/Off
	float OPAWF;  // Waveform
	float OPACT;  // Coarse Tune
	float OPAFT;  // Fine Tune
	float OPAKT;  // Keyboard Track
	float OPAKS;  // Keyboard Scaling
	float OPAVS;  // Velocity Sensivity
	float OPADL;  // Delay Time
	float OPAAT;  // Attack Time
	float OPADE;  // Decay Time
	float OPASU;  // Sustain Level
	float OPAST;  // Sustain Time
	float OPARE;  // Release Time
	// Operator B
	float OPBON;  // On/Off
	float OPBWF;  // Waveform
	float OPBCT;  // Coarse Tune
	float OPBFT;  // Fine Tune
	float OPBKT;  // Keyboard Track
	float OPBKS;  // Keyboard Scaling
	float OPBVS;  // Velocity Sensivity
	float OPBDL;  // Delay Time
	float OPBAT;  // Attack Time
	float OPBDE;  // Decay Time
	float OPBSU;  // Sustain Level
	float OPBST;  // Sustain Time
	float OPBRE;  // Release Time
	// Operator C
	float OPCON;  // On/Off
	float OPCWF;  // Waveform
	float OPCCT;  // Coarse Tune
	float OPCFT;  // Fine Tune
	float OPCKT;  // Keyboard Track
	float OPCKS;  // Keyboard Scaling
	float OPCVS;  // Velocity Sensivity
	float OPCDL;  // Delay Time
	float OPCAT;  // Attack Time
	float OPCDE;  // Decay Time
	float OPCSU;  // Sustain Level
	float OPCST;  // Sustain Time
	float OPCRE;  // Release Time
	// Operator D
	float OPDON;  // On/Off
	float OPDWF;  // Waveform
	float OPDCT;  // Coarse Tune
	float OPDFT;  // Fine Tune
	float OPDKT;  // Keyboard Track
	float OPDKS;  // Keyboard Scaling
	float OPDVS;  // Velocity Sensivity
	float OPDDL;  // Delay Time
	float OPDAT;  // Attack Time
	float OPDDE;  // Decay Time
	float OPDSU;  // Sustain Level
	float OPDST;  // Sustain Time
	float OPDRE;  // Release Time
	// Operator E
	float OPEON;  // On/Off
	float OPEWF;  // Waveform
	float OPECT;  // Coarse Tune
	float OPEFT;  // Fine Tune
	float OPEKT;  // Keyboard Track
	float OPEKS;  // Keyboard Scaling
	float OPEVS;  // Velocity Sensivity
	float OPEDL;  // Delay Time
	float OPEAT;  // Attack Time
	float OPEDE;  // Decay Time
	float OPESU;  // Sustain Level
	float OPEST;  // Sustain Time
	float OPERE;  // Release Time
	// Operator F
	float OPFON;  // On/Off
	float OPFWF;  // Waveform
	float OPFCT;  // Coarse Tune
	float OPFFT;  // Fine Tune
	float OPFKT;  // Keyboard Track
	float OPFKS;  // Keyboard Scaling
	float OPFVS;  // Velocity Sensivity
	float OPFDL;  // Delay Time
	float OPFAT;  // Attack Time
	float OPFDE;  // Decay Time
	float OPFSU;  // Sustain Level
	float OPFST;  // Sustain Time
	float OPFRE;  // Release Time
	// Operator X
	float OPXON;  // On/Off
	float OPXCU;  // Cuttoff
	float OPXRS;  // Resonance
	float OPXAM;  // Amount
	float OPXBP;  // Bypass
	float OPXKS;  // Keyboard Scaling
	float OPXVS;  // Velocity Sensivity
	float OPXDL;  // Delay Time
	float OPXAT;  // Attack Time
	float OPXDE;  // Decay Time
	float OPXSU;  // Sustain Level
	float OPXST;  // Sustain Time
	float OPXRE;  // Release Time
	// Operator Z
	float OPZON;  // On/Off
	float OPZCU;  // Cuttof
	float OPZRS;  // Resonance
	float OPZTY;  // Type
	float OPZKT;  // Keyboard Track
	float OPZKS;  // Keyboard Scaling
	float OPZVS;  // Velocity Sensivity
	float OPZDL;  // Delay Time
	float OPZAT;  // Attack Time
	float OPZDE;  // Decay Time
	float OPZSU;  // Sustain Level
	float OPZST;  // Sustain Time
	float OPZRE;  // Release Time
	// half matrix
	float MAA;    // OPA Self-modulation
	float MAB;
	float MAC;
	float MAD;
	float MAE;
	float MAF;
	float MAX;
	float MAZ;
	float MAO;
	float MAP;
	float MBB;    // OPB Self-modulation
	float MBC;
	float MBD;
	float MBE;
	float MBF;
	float MBX;
	float MBZ;
	float MBO;
	float MBP;
	float MCC;    // OPC Self-modulation
	float MCD;
	float MCE;
	float MCF;
	float MCX;
	float MCZ;
	float MCO;
	float MCP;
	float MDD;    // OPD Self-modulation
	float MDE;
	float MDF;
	float MDX;
	float MDZ;
	float MDO;
	float MDP;
	float MEE;    // OPE Self-modulation
	float MEF;
	float MEX;
	float MEZ;
	float MEO;
	float MEP;
	float MFF;    // OPF Self-modulation
	float MFX;
	float MFZ;
	float MFO;
	float MFP;
	float MXZ;
	float MXO;
	float MXP;
	float MZO;
	float MZP;
	// LFO
	float LFOWF;  // Waveform
	float LFORA;  // Rate
	float LFODE;  // Depth
	float LFODL;  // Delay
	float LFODS;  // Destination (PITCH,OPX,OPZ)
	// Pitch
	float PORTA;  // Portamento
	float PTCCU;  // Pitch Curve
	float PTCTI;  // Pitch Time
	// Modulation wheel
	float MDLDS;  // Modulation wheel destination (LFO,OPX,OPZ)
	// High quality
	float HQ;     // High quality (oscillator interpolation)
	// reserved for future use
	float VAR01;
	float VAR02;
	float VAR03;
	float VAR04;
	float VAR05;
	float VAR06;
	float VAR07;
	float VAR08;
	float VAR09;
};

struct SGlobal
{
	// Reverb
	float REVTI;  // Reverb Time
	float REVDA;  // Reverb Damp
	// Delay
	float DLYTI;  // Delay Time
	float DLYFE;  // Delay Feedback
	float DLYLF;  // Delay LFO Rate
	float DLYLA;  // Delay LFO Amount
	// reserved for future use
	float VAR01;
	float VAR02;
	float VAR03;
	float VAR04;
	float VAR05;
	float VAR06;
	float VAR07;
	float VAR08;
	float VAR09;
	float VAR10;
};

struct SBank
{
	SGlobal     global;
	SProgram    prg[MAX_PROGRAMS];
};

class CPrograms
{
private:
	SBank*        currentbank;
	unsigned char numprg[MIDICHANNELS];    // programs numbers
	bool          isWaiting[MIDICHANNELS]; // indicates if the GUI is waiting for store confirmation
	unsigned char prgtmp;                  // the destination program slot to store
public:
	void          Init();
	SProgram*     GetProgram  (char channel);
	SGlobal*      GetSGlobal  (void);
	void          SetNumProgr (char channel, unsigned char numprog);
	void          SetBankIndex(int nbank);
	SBank*        GetBank     (void);
#ifndef __OXEDMO__
private:
	CPersist      persist;
	int           iCurrentBank;       // current bank index
	bool          isEditingName;      // indicates if the user is editing program name
	bool          bankMode;           // indicates if the user is choosing a bank
	bool          haschanges;         // self-explanatory
/*
	void          InitBank();
*/
	void          *hWndEditor;
public:
	float         SetDefault    (char channel, int par);
	float         GetPar        (char channel, int par);
	void          SetPar        (char channel, int par, float val);
	bool          GetStandBy    (char channel);
	void          SetStandBy    (char channel, bool isWaiting);
	bool          IsEditingName ();
	void          SetEditingName(bool isEditingName);
	bool          GetBankMode   ();
	void          SetBankMode   (bool bankMode);
	void          GetProgName   (char *str, char channel);
	void          SetProgName   (char *str, char channel);
	void          GetProgName   (char *str, int numpg);
	void          SetProgName   (char *str, int numpg);
	void          GetBankName   (char *str);
	int           GetBankCount  ();
	int           GetBankIndex  ();
	unsigned char GetNumProgr   (char channel);
	void          StoreProgram  (char channel);
	void          CopyProgram   (int destination, int source);
	void          SetBank       (SBank *bank);
	void          SetProgram    (char numprg, SProgram *program);
	bool          HasChanges    ();
	void          SetEditorHn   (void *hWndEditor);
#endif
};
