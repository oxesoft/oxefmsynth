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

#ifndef __COxeVst__
#define __COxeVst__

#include <string.h>

#ifndef __AudioEffectX__
#include "public.sdk/source/vst2.x/audioeffectx.h"
#endif

enum
{
	kNumPrograms = MAX_PROGRAMS,
	kNumOutputs = STEREOPHONIC,
	kNumParams = PARAMETERS_COUNT
};

#define MAX_EVENTS_AT_ONCE_POWEROFTWO 8
#define EVENTS_MASK                   ((1<<MAX_EVENTS_AT_ONCE_POWEROFTWO) - 1)

//------------------------------------------------------------------------------------------
class COxeVst : public AudioEffectX
{
public:
	COxeVst(audioMasterCallback audioMaster);

	virtual void processReplacing(float **inputs, float **outputs, VstInt32 sampleframes);
	virtual VstInt32 processEvents(VstEvents* events);
	virtual void setProgram(VstInt32 program);
	virtual void setProgramName (char *name);
	virtual void getProgramName(char *name);
	virtual bool getProgramNameIndexed (VstInt32 category, VstInt32 index, char* text);
	virtual VstInt32 getChunk (void** data, bool isPreset = false);
	virtual VstInt32 setChunk (void* data, VstInt32 byteSize, bool isPreset = false);
	virtual void setSampleRate (float sampleRate);
	virtual void suspend();
	virtual bool getOutputProperties (VstInt32 index, VstPinProperties* properties);
	virtual bool copyProgram (VstInt32 destination);
	virtual bool getEffectName (char* name);
	virtual bool getVendorString (char* text);
	virtual bool getProductString (char* text);
	virtual VstInt32 getVendorVersion ();
	virtual VstInt32 canDo (char* text);
	virtual VstInt32 getNumMidiInputChannels ();
	virtual VstInt32 getNumMidiOutputChannels ();
	virtual void setParameter (VstInt32 index, float value);
	virtual float getParameter (VstInt32 index);
	virtual void getParameterLabel (VstInt32 index, char* label);
	virtual void getParameterDisplay (VstInt32 index, char* text);
	virtual void getParameterName (VstInt32 index, char* text);
private:
	CSynthesizer *synthesizer;
	VstInt32 posExt;
	VstInt32 posInt;
	VstInt32 bufferPos;
	struct MIDIEvent
	{
		unsigned char bstat;
		unsigned char bdad1;
		unsigned char bdad2;
		VstInt32      pos;
	};
	struct MIDIEvents
	{
		VstInt32  eventsCount;
		VstInt32  nextEvent;
		MIDIEvent event[1<<MAX_EVENTS_AT_ONCE_POWEROFTWO];
	};
	MIDIEvents events;
};

#endif
