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

#include "oxevsteditor.h"
#include "oxevst.h"

COxeVst::COxeVst(audioMasterCallback audioMaster) : AudioEffectX(audioMaster, kNumPrograms, kNumParams)
{
    editor = new COxeVstEditor(this, &synthesizer);
    if (!editor)
    {
        return;
    }
    if (audioMaster)
    {
        setUniqueID         (CCONST('O','X','F','M'));
        setNumInputs        (0);
        setNumOutputs       (kNumOutputs);
        canProcessReplacing ();
        programsAreChunks   ();
        isSynth             ();
    }
    suspend();
    posExt    = 0;
    posInt    = 0;
    bufferPos = 0;
    memset(&events,0,sizeof(events));
}

COxeVst::~COxeVst()
{
    delete editor;
    editor = NULL;
}

void COxeVst::setProgram(VstInt32 program)
{
    AudioEffectX::setProgram(program);
    synthesizer.SendEvent(0xC0, getProgram(), 0, 0);
}

void COxeVst::getProgramName(char *name) // kVstMaxProgNameLen
{
    synthesizer.GetProgName(name, getProgram());
}

void COxeVst::setProgramName(char *name) // kVstMaxProgNameLen
{
    synthesizer.SetProgName(name, getProgram());
}

VstInt32 COxeVst::getChunk(void **data, bool isPreset)
{
    if (isPreset)
    {
        *data = &synthesizer.GetBank()->prg[getProgram()];
        return sizeof(SProgram);
    }
    else
    {
        *data = synthesizer.GetBank();
        return sizeof(SBank);
    }
}

VstInt32 COxeVst::setChunk(void *data, VstInt32 byteSize, bool isPreset)
{
    if (isPreset)
    {
        if (byteSize != sizeof(SProgram))
            return 0;
        synthesizer.SetProgram(getProgram(), (SProgram*)data);
    }
    else
    {
        if (byteSize != sizeof(SBank))
            return 0;
        synthesizer.SetBank((SBank*)data);
    }
    return 1;
}

bool COxeVst::getOutputProperties(VstInt32 index, VstPinProperties* properties)
{
    if (index < kNumOutputs)
    {
        vst_strncpy (properties->label, "Vstx ", 63);
        char temp[11] = {0};
        int2string (index + 1, temp, 10);
        vst_strncat (properties->label, temp, 63);
        
        properties->flags = kVstPinIsActive;
        if (index < 2)
            properties->flags |= kVstPinIsStereo;    // make channel 1+2 stereo
        return true;
    }
    return false;
}

bool COxeVst::getProgramNameIndexed(VstInt32 category, VstInt32 index, char* text)
{
    synthesizer.GetProgName(text, index);
    if (strlen(text) < 1)
    {
        vst_strncpy(text, "(empty)", kVstMaxProgNameLen);
    }
    return true;
}

bool COxeVst::copyProgram(VstInt32 destination)
{
    synthesizer.CopyProgram(destination, getProgram());
    return true;
}

bool COxeVst::getEffectName(char* name)
{
    vst_strncpy(name, "Oxe FM Synth", kVstMaxEffectNameLen);
    return true;
}

bool COxeVst::getVendorString(char* text)
{
    vst_strncpy(text, "Oxe Music Software", kVstMaxVendorStrLen);
    return true;
}

bool COxeVst::getProductString(char* text)
{
    vst_strncpy(text, "Oxe FM Synth", kVstMaxProductStrLen);
    return true;
}

VstInt32 COxeVst::getVendorVersion()
{
    return VERSION_INT;
}

VstInt32 COxeVst::canDo(char* text)
{
    if (!strcmp(text, "receiveVstMidiEvent"))
        return 1;
    return -1;    // explicitly can't do; 0 => don't know
}

VstInt32 COxeVst::getNumMidiInputChannels()
{
    return MIDICHANNELS;
}

VstInt32 COxeVst::getNumMidiOutputChannels()
{
    return 0;
}

void COxeVst::setSampleRate(float sampleRate)
{
    AudioEffectX::setSampleRate(sampleRate);
    synthesizer.SetSampleRate(sampleRate);
}

void COxeVst::suspend()
{
    synthesizer.KillNotes();
}

void COxeVst::processReplacing(float **inputs, float **outputs, VstInt32 sampleFrames)
{
    float *out1 = outputs[0];
    float *out2 = outputs[1];

    VstInt32 tambufferInt = SAMPLES_PER_PROCESS<<1;
    VstInt32 tambufferExt = sampleFrames;

    while (true)
    {
        if (!posInt)
        {
            // MIDI -------------------------------------------------------------------------------------------------------
            while (events.eventsCount)
            {
                if (events.event[events.nextEvent].pos>bufferPos+SAMPLES_PER_PROCESS)
                    break;
                if (events.event[events.nextEvent].pos < bufferPos)
                    events.event[events.nextEvent].pos = bufferPos;
                synthesizer.SendEvent(events.event[events.nextEvent].bstat,events.event[events.nextEvent].bdad1,events.event[events.nextEvent].bdad2,events.event[events.nextEvent].pos);
                events.eventsCount--;
                events.nextEvent++;
                events.nextEvent &= EVENTS_MASK;
            }
            // ------------------------------------------------------------------------------------------------------------
            synthesizer.Process(synthesizer.buffers.bSynthOut, SAMPLES_PER_PROCESS, bufferPos);
            bufferPos += SAMPLES_PER_PROCESS;
        }
        VstInt32 iaux = min(tambufferInt-posInt,tambufferExt-posExt);
        while (iaux>0)
        {
            out1[posExt] = float(synthesizer.buffers.bSynthOut[posInt++])/32767.f;
            out2[posExt] = float(synthesizer.buffers.bSynthOut[posInt++])/32767.f;
            posExt++;
            iaux-=2;
        }
        if (posInt >= tambufferInt)
        {
            posInt = 0;
        }
        if (posExt >= tambufferExt)
        {
            posExt = 0;
            break;
        }
    }
}

VstInt32 COxeVst::processEvents(VstEvents* ev)
{
    VstInt32 n;
    for (VstInt32 i=0; i<ev->numEvents; i++)
    {
        if ((ev->events[i])->type != kVstMidiType)
            continue;
        VstMidiEvent* event     = (VstMidiEvent*)ev->events[i];
        unsigned char* midiData = (unsigned char*)event->midiData;
        n  = events.nextEvent + events.eventsCount;
        n &= EVENTS_MASK;
        events.event[n].bstat = midiData[0];
        events.event[n].bdad1 = midiData[1];
        events.event[n].bdad2 = midiData[2];
        events.event[n].pos   = event->deltaFrames + bufferPos;
        events.eventsCount++;
    }
    return 1;
}

void COxeVst::setParameter (VstInt32 index, float value)
{
    ((COxeVstEditor*)editor)->getEditor()->SetPar(index, value);
}

float COxeVst::getParameter (VstInt32 index)
{
    return ((COxeVstEditor*)editor)->getEditor()->GetPar(index);
}

void COxeVst::getParameterLabel (VstInt32 index, char* label) // kVstMaxParamStrLen
{
    ((COxeVstEditor*)editor)->getEditor()->GetParLabel(index, label);
}

void COxeVst::getParameterDisplay (VstInt32 index, char* text) // kVstMaxParamStrLen
{
    ((COxeVstEditor*)editor)->getEditor()->GetParDisplay(index, text);
}

void COxeVst::getParameterName (VstInt32 index, char* text) // kVstMaxParamStrLen
{
    ((COxeVstEditor*)editor)->getEditor()->GetParName(index, text);
}
