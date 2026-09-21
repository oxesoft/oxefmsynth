/*
Oxe FM Synth: a software synthesizer
Copyright (C) 2004-2026  Daniel Moura <oxe@oxesoft.com>

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

#include "oxevst3editor.h"
#include "oxevst3.h"
#include "pluginterfaces/base/ustring.h"
#include "pluginterfaces/base/ibstream.h"
#include "pluginterfaces/vst/ivstevents.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

#include <cmath>
#include <algorithm>

namespace Steinberg {
namespace Vst {

COxeVst3::COxeVst3()
    : posExt(0), posInt(0), bufferPos(0), activeEditor(nullptr),
      isUpdatingProgramFromHost(false)
{
    memset(&events, 0, sizeof(events));
}

COxeVst3::~COxeVst3()
{
}

tresult PLUGIN_API COxeVst3::initialize(FUnknown* context)
{
    tresult result = SingleComponentEffect::initialize(context);
    if (result != kResultOk)
        return result;

    addAudioOutput(STR16("Stereo Out"), SpeakerArr::kStereo);
    addEventInput(STR16("MIDI In"), 16);

    // 1. Register Engine Parameters (0 .. PARAMETERS_COUNT - 1)
    CEditor tempEditor(&synthesizer);
    for (int i = 0; i < PARAMETERS_COUNT; i++)
    {
        char name[TEXT_SIZE] = {0};
        tempEditor.GetParName(i, name);
        if (name[0] == 0 || strcmp(name, "not found") == 0)
        {
            snprintf(name, sizeof(name), "Param %d", i + 1);
        }
        TChar uTitle[128] = {0};
        Steinberg::UString(uTitle, 128).assign(name);

        float val = tempEditor.GetPar(i);
        parameters.addParameter(uTitle, nullptr, 0, val, ParameterInfo::kCanAutomate, i);
    }

    // 2. Program List & Program Change Parameter
    addUnit(new Unit(USTRING("Root"), kRootUnitId, kNoParentUnitId, 0));
    ProgramList* prgList = new ProgramList(USTRING("Programs"), 0, kRootUnitId);
    StringListParameter* programParam = new StringListParameter(
        USTRING("Program"), kProgramParamId, nullptr,
        ParameterInfo::kIsProgramChange | ParameterInfo::kCanAutomate
    );
    for (int i = 0; i < MAX_PROGRAMS; i++)
    {
        char pName[PG_NAME_SIZE + 1] = {0};
        synthesizer.GetProgName(pName, i);
        TChar uName[128] = {0};
        Steinberg::UString(uName, 128).assign(pName);
        programParam->appendString(uName);
        prgList->addProgram(uName);
    }
    parameters.addParameter(programParam);
    addProgramList(prgList);

    // 3. MIDI Controller Parameters (mapped via IMidiMapping)
    for (int ch = 0; ch < 16; ch++)
    {
        parameters.addParameter(USTRING("Mod Wheel"), nullptr, 0, 0.0, ParameterInfo::kIsHidden, kModWheelBase + ch);
        parameters.addParameter(USTRING("Volume"), nullptr, 0, 1.0, ParameterInfo::kIsHidden, kVolumeBase + ch);
        parameters.addParameter(USTRING("Pan"), nullptr, 0, 0.5, ParameterInfo::kIsHidden, kPanBase + ch);
        parameters.addParameter(USTRING("Sustain"), nullptr, 0, 0.0, ParameterInfo::kIsHidden, kSustainBase + ch);
        parameters.addParameter(USTRING("Reverb"), nullptr, 0, 0.0, ParameterInfo::kIsHidden, kReverbBase + ch);
        parameters.addParameter(USTRING("Delay"), nullptr, 0, 0.0, ParameterInfo::kIsHidden, kDelayBase + ch);
        parameters.addParameter(USTRING("Pitch Bend"), nullptr, 0, 0.5, ParameterInfo::kIsHidden, kPitchBendBase + ch);
        parameters.addParameter(USTRING("Aftertouch"), nullptr, 0, 0.0, ParameterInfo::kIsHidden, kAftertouchBase + ch);
    }

    return kResultOk;
}

tresult PLUGIN_API COxeVst3::terminate()
{
    return SingleComponentEffect::terminate();
}

tresult PLUGIN_API COxeVst3::setActive(TBool state)
{
    if (!state)
    {
        synthesizer.KillNotes();
    }
    return SingleComponentEffect::setActive(state);
}

tresult PLUGIN_API COxeVst3::canProcessSampleSize(int32 symbolicSampleSize)
{
    return (symbolicSampleSize == kSample32) ? kResultTrue : kResultFalse;
}

tresult PLUGIN_API COxeVst3::setupProcessing(ProcessSetup& newSetup)
{
    SingleComponentEffect::setupProcessing(newSetup);
    synthesizer.SetSampleRate((float)newSetup.sampleRate);
    return kResultOk;
}

tresult PLUGIN_API COxeVst3::setBusArrangements(SpeakerArrangement* inputs, int32 numIns,
                                                SpeakerArrangement* outputs, int32 numOuts)
{
    if (numIns > 0 || numOuts != 1 || outputs[0] != SpeakerArr::kStereo)
        return kResultFalse;
    return kResultOk;
}

void COxeVst3::enqueueEvent(unsigned char bstat, unsigned char bdad1, unsigned char bdad2, int32 pos)
{
    int32 n = events.nextEvent + events.eventsCount;
    n &= EVENTS_MASK;
    events.event[n].bstat = bstat;
    events.event[n].bdad1 = bdad1;
    events.event[n].bdad2 = bdad2;
    events.event[n].pos = pos;
    events.eventsCount++;
}

void COxeVst3::handleParamChange(ParamID id, ParamValue value, int32 pos)
{
    if (id < PARAMETERS_COUNT)
    {
        CEditor tempEditor(&synthesizer);
        tempEditor.SetPar((int)id, (float)value);
        if (activeEditor)
        {
            activeEditor->getEditor()->Update();
        }
        return;
    }

    if (id == kProgramParamId)
    {
        int prog = (int)std::lrint(value * (MAX_PROGRAMS - 1));
        prog = std::max(0, std::min(MAX_PROGRAMS - 1, prog));
        enqueueEvent(0xC0, (unsigned char)prog, 0, pos);
        if (activeEditor)
        {
            activeEditor->getEditor()->ProgramChanged();
        }
        return;
    }

    if (id >= kModWheelBase && id < kModWheelBase + 16)
    {
        int ch = (int)(id - kModWheelBase);
        enqueueEvent(0xB0 | ch, 1, (unsigned char)std::lrint(value * 127.f), pos);
        return;
    }

    if (id >= kVolumeBase && id < kVolumeBase + 16)
    {
        int ch = (int)(id - kVolumeBase);
        enqueueEvent(0xB0 | ch, 7, (unsigned char)std::lrint(value * 127.f), pos);
        return;
    }

    if (id >= kPanBase && id < kPanBase + 16)
    {
        int ch = (int)(id - kPanBase);
        enqueueEvent(0xB0 | ch, 10, (unsigned char)std::lrint(value * 127.f), pos);
        return;
    }

    if (id >= kSustainBase && id < kSustainBase + 16)
    {
        int ch = (int)(id - kSustainBase);
        enqueueEvent(0xB0 | ch, 64, (unsigned char)std::lrint(value * 127.f), pos);
        return;
    }

    if (id >= kReverbBase && id < kReverbBase + 16)
    {
        int ch = (int)(id - kReverbBase);
        enqueueEvent(0xB0 | ch, 91, (unsigned char)std::lrint(value * 127.f), pos);
        return;
    }

    if (id >= kDelayBase && id < kDelayBase + 16)
    {
        int ch = (int)(id - kDelayBase);
        enqueueEvent(0xB0 | ch, 93, (unsigned char)std::lrint(value * 127.f), pos);
        return;
    }

    if (id >= kPitchBendBase && id < kPitchBendBase + 16)
    {
        int ch = (int)(id - kPitchBendBase);
        int pb = (int)std::lrint(value * 16383.f);
        pb = std::max(0, std::min(16383, pb));
        enqueueEvent(0xE0 | ch, pb & 0x7F, (pb >> 7) & 0x7F, pos);
        return;
    }

    if (id >= kAftertouchBase && id < kAftertouchBase + 16)
    {
        int ch = (int)(id - kAftertouchBase);
        enqueueEvent(0xD0 | ch, (unsigned char)std::lrint(value * 127.f), 0, pos);
        return;
    }
}

tresult PLUGIN_API COxeVst3::process(ProcessData& data)
{
    // 1. Process parameter changes
    if (data.inputParameterChanges)
    {
        int32 numParamChanges = data.inputParameterChanges->getParameterCount();
        for (int32 i = 0; i < numParamChanges; i++)
        {
            IParamValueQueue* queue = data.inputParameterChanges->getParameterData(i);
            if (!queue) continue;
            int32 numPoints = queue->getPointCount();
            if (numPoints <= 0) continue;

            int32 sampleOffset = 0;
            ParamValue value = 0.0;
            if (queue->getPoint(numPoints - 1, sampleOffset, value) == kResultOk)
            {
                handleParamChange(queue->getParameterId(), value, bufferPos + sampleOffset);
            }
        }
    }

    // 2. Process incoming MIDI note events
    if (data.inputEvents)
    {
        int32 numEvents = data.inputEvents->getEventCount();
        for (int32 i = 0; i < numEvents; i++)
        {
            Event event;
            if (data.inputEvents->getEvent(i, event) == kResultOk)
            {
                switch (event.type)
                {
                    case Event::kNoteOnEvent:
                    {
                        unsigned char ch = (unsigned char)(event.noteOn.channel & 0x0F);
                        unsigned char note = (unsigned char)event.noteOn.pitch;
                        unsigned char vel = (unsigned char)std::lrint(event.noteOn.velocity * 127.f);
                        enqueueEvent(0x90 | ch, note, vel, bufferPos + event.sampleOffset);
                        break;
                    }
                    case Event::kNoteOffEvent:
                    {
                        unsigned char ch = (unsigned char)(event.noteOff.channel & 0x0F);
                        unsigned char note = (unsigned char)event.noteOff.pitch;
                        unsigned char vel = (unsigned char)std::lrint(event.noteOff.velocity * 127.f);
                        enqueueEvent(0x80 | ch, note, vel, bufferPos + event.sampleOffset);
                        break;
                    }
                    case Event::kPolyPressureEvent:
                    {
                        unsigned char ch = (unsigned char)(event.polyPressure.channel & 0x0F);
                        unsigned char note = (unsigned char)event.polyPressure.pitch;
                        unsigned char press = (unsigned char)std::lrint(event.polyPressure.pressure * 127.f);
                        enqueueEvent(0xA0 | ch, note, press, bufferPos + event.sampleOffset);
                        break;
                    }
                    default:
                        break;
                }
            }
        }
    }

    if (data.numOutputs == 0 || data.numSamples == 0)
        return kResultOk;

    float* out1 = data.outputs[0].channelBuffers32[0];
    float* out2 = data.outputs[0].channelBuffers32[1];
    if (!out1 || !out2)
        return kResultOk;

    int32 tambufferInt = SAMPLES_PER_PROCESS << 1;
    int32 tambufferExt = data.numSamples;

    while (true)
    {
        if (!posInt)
        {
            // MIDI dispatching
            while (events.eventsCount)
            {
                if (events.event[events.nextEvent].pos > bufferPos + SAMPLES_PER_PROCESS)
                    break;
                if (events.event[events.nextEvent].pos < bufferPos)
                    events.event[events.nextEvent].pos = bufferPos;
                bool isProg = (events.event[events.nextEvent].bstat & 0xF0) == 0xC0;
                if (isProg)
                    isUpdatingProgramFromHost.store(true, std::memory_order_relaxed);
                synthesizer.SendEvent(events.event[events.nextEvent].bstat,
                                      events.event[events.nextEvent].bdad1,
                                      events.event[events.nextEvent].bdad2,
                                      events.event[events.nextEvent].pos);
                if (isProg)
                    isUpdatingProgramFromHost.store(false, std::memory_order_relaxed);
                events.eventsCount--;
                events.nextEvent++;
                events.nextEvent &= EVENTS_MASK;
            }
            synthesizer.Process(synthesizer.buffers.bSynthOut, SAMPLES_PER_PROCESS, bufferPos);
            bufferPos += SAMPLES_PER_PROCESS;
        }

        int32 iaux = std::min(tambufferInt - posInt, tambufferExt - posExt);
        while (iaux > 0)
        {
            out1[posExt] = float(synthesizer.buffers.bSynthOut[posInt++]) / 32767.f;
            out2[posExt] = float(synthesizer.buffers.bSynthOut[posInt++]) / 32767.f;
            posExt++;
            iaux -= 2;
        }

        if (posInt >= tambufferInt)
            posInt = 0;
        if (posExt >= tambufferExt)
        {
            posExt = 0;
            break;
        }
    }

    return kResultOk;
}

tresult PLUGIN_API COxeVst3::getState(IBStream* state)
{
    SBank* bank = synthesizer.GetBank();
    int32 bytesWritten = 0;
    if (state->write(bank, sizeof(SBank), &bytesWritten) != kResultOk || bytesWritten != sizeof(SBank))
        return kResultFalse;

    int32 currentProg = synthesizer.GetNumProgr(0);
    state->write(&currentProg, sizeof(int32), &bytesWritten);
    return kResultOk;
}

tresult PLUGIN_API COxeVst3::setState(IBStream* state)
{
    SBank bank;
    int32 bytesRead = 0;
    if (state->read(&bank, sizeof(SBank), &bytesRead) != kResultOk || bytesRead != sizeof(SBank))
        return kResultFalse;

    synthesizer.SetBank(&bank);

    int32 currentProg = 0;
    if (state->read(&currentProg, sizeof(int32), &bytesRead) == kResultOk && bytesRead == sizeof(int32))
    {
        isUpdatingProgramFromHost.store(true, std::memory_order_relaxed);
        synthesizer.SendEvent(0xC0, currentProg, 0, 0);
        isUpdatingProgramFromHost.store(false, std::memory_order_relaxed);
        SingleComponentEffect::setParamNormalized(kProgramParamId, (double)currentProg / (MAX_PROGRAMS - 1));
    }

    if (activeEditor)
    {
        activeEditor->getEditor()->ProgramChanged();
    }

    return kResultOk;
}

tresult PLUGIN_API COxeVst3::getEditorState(IBStream* /*state*/)
{
    return kResultOk;
}

tresult PLUGIN_API COxeVst3::setEditorState(IBStream* /*state*/)
{
    return kResultOk;
}

IPlugView* PLUGIN_API COxeVst3::createView(FIDString name)
{
    if (name == nullptr || strcmp(name, ViewType::kEditor) == 0)
    {
        return new COxeVst3Editor(this, &synthesizer);
    }
    return nullptr;
}

tresult PLUGIN_API COxeVst3::setParamNormalized(ParamID tag, ParamValue value)
{
    if (tag < PARAMETERS_COUNT)
    {
        CEditor tempEditor(&synthesizer);
        tempEditor.SetPar((int)tag, (float)value);
        if (activeEditor)
        {
            activeEditor->getEditor()->Update();
        }
    }
    else if (tag == kProgramParamId)
    {
        int prog = (int)std::lrint(value * (MAX_PROGRAMS - 1));
        prog = std::max(0, std::min(MAX_PROGRAMS - 1, prog));
        if (prog != synthesizer.GetNumProgr(0))
        {
            isUpdatingProgramFromHost.store(true, std::memory_order_relaxed);
            synthesizer.SendEvent(0xC0, (unsigned char)prog, 0, 0);
            isUpdatingProgramFromHost.store(false, std::memory_order_relaxed);
            if (activeEditor)
            {
                activeEditor->getEditor()->ProgramChanged();
            }
        }
    }
    return SingleComponentEffect::setParamNormalized(tag, value);
}

tresult PLUGIN_API COxeVst3::getParamStringByValue(ParamID tag, ParamValue valueNormalized,
                                                   String128 string)
{
    if (tag < PARAMETERS_COUNT)
    {
        CEditor tempEditor(&synthesizer);
        char text[TEXT_SIZE] = {0};
        tempEditor.GetParDisplay((int)tag, text);
        Steinberg::UString(string, 128).assign(text);
        return kResultTrue;
    }
    return SingleComponentEffect::getParamStringByValue(tag, valueNormalized, string);
}

tresult PLUGIN_API COxeVst3::getMidiControllerAssignment(int32 busIndex, int16 channel,
                                                         CtrlNumber midiControllerNumber,
                                                         ParamID& tag)
{
    if (busIndex != 0 || channel < 0 || channel >= 16)
        return kResultFalse;

    switch (midiControllerNumber)
    {
        case kCtrlModWheel:
            tag = kModWheelBase + channel;
            return kResultTrue;
        case kCtrlVolume:
            tag = kVolumeBase + channel;
            return kResultTrue;
        case kCtrlPan:
            tag = kPanBase + channel;
            return kResultTrue;
        case kCtrlSustainOnOff:
            tag = kSustainBase + channel;
            return kResultTrue;
        case 91: // Reverb
            tag = kReverbBase + channel;
            return kResultTrue;
        case 93: // Delay
            tag = kDelayBase + channel;
            return kResultTrue;
        case kPitchBend:
            tag = kPitchBendBase + channel;
            return kResultTrue;
        case kAfterTouch:
            tag = kAftertouchBase + channel;
            return kResultTrue;
        case kCtrlProgramChange:
            tag = kProgramParamId;
            return kResultTrue;
        default:
            break;
    }
    return kResultFalse;
}

//---Interface---------
tresult PLUGIN_API COxeVst3::queryInterface(const TUID iid, void** obj)
{
    DEF_INTERFACE(IMidiMapping)
    return SingleComponentEffect::queryInterface(iid, obj);
}

// GUI helpers
void COxeVst3::setParameterFromGUI(int index, float value)
{
    SingleComponentEffect::setParamNormalized(index, value);
    beginEdit(index);
    performEdit(index, value);
    endEdit(index);
}

void COxeVst3::setProgramFromGUI(int program)
{
    if (isUpdatingProgramFromHost.load(std::memory_order_relaxed))
        return;

    isUpdatingProgramFromHost.store(true, std::memory_order_relaxed);
    double norm = (double)program / (MAX_PROGRAMS - 1);
    SingleComponentEffect::setParamNormalized(kProgramParamId, norm);
    beginEdit(kProgramParamId);
    performEdit(kProgramParamId, norm);
    endEdit(kProgramParamId);
    isUpdatingProgramFromHost.store(false, std::memory_order_relaxed);
}

void COxeVst3::updateHostDisplay()
{
    if (componentHandler)
    {
        componentHandler->restartComponent(kParamTitlesChanged);
    }
}

void COxeVst3::setActiveEditor(COxeVst3Editor* editor)
{
    activeEditor = editor;
}

void COxeVst3::onEditorDestroyed(COxeVst3Editor* editor)
{
    if (activeEditor == editor)
    {
        activeEditor = nullptr;
    }
}

} // namespace Vst
} // namespace Steinberg
