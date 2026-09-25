/*
Oxe FM Synth: a software synthesizer
Copyright (C) 2004-2026  Daniel Moura <oxesoft@gmail.com>

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

#pragma once

#include <atomic>
#include "public.sdk/source/vst/vstsinglecomponenteffect.h"
#include "pluginterfaces/vst/ivstmidicontrollers.h"

class COxeVst3Editor;

namespace Steinberg {
namespace Vst {

enum CustomParamIDs
{
    // Engine parameters are 0 .. PARAMETERS_COUNT - 1
    kProgramParamId = 1000,

    // MIDI controllers (16 channels each)
    kModWheelBase = 1100,    // 1100 .. 1115
    kVolumeBase = 1120,      // 1120 .. 1135
    kPanBase = 1140,         // 1140 .. 1155
    kSustainBase = 1160,     // 1160 .. 1175
    kReverbBase = 1180,      // 1180 .. 1195
    kDelayBase = 1200,       // 1200 .. 1215
    kPitchBendBase = 1220,   // 1220 .. 1235
    kAftertouchBase = 1240   // 1240 .. 1255
};

#define MAX_EVENTS_AT_ONCE_POWEROFTWO 8
#define EVENTS_MASK                   ((1<<MAX_EVENTS_AT_ONCE_POWEROFTWO) - 1)

class COxeVst3 : public SingleComponentEffect,
                 public IMidiMapping
{
public:
    COxeVst3();
    ~COxeVst3() override;

    static FUnknown* createInstance(void* /*context*/)
    {
        return (IAudioProcessor*)new COxeVst3;
    }

    //---from IComponent-----------------------
    tresult PLUGIN_API initialize(FUnknown* context) SMTG_OVERRIDE;
    tresult PLUGIN_API terminate() SMTG_OVERRIDE;
    tresult PLUGIN_API setActive(TBool state) SMTG_OVERRIDE;
    tresult PLUGIN_API process(ProcessData& data) SMTG_OVERRIDE;
    tresult PLUGIN_API canProcessSampleSize(int32 symbolicSampleSize) SMTG_OVERRIDE;
    tresult PLUGIN_API setState(IBStream* state) SMTG_OVERRIDE;
    tresult PLUGIN_API getState(IBStream* state) SMTG_OVERRIDE;
    tresult PLUGIN_API setupProcessing(ProcessSetup& newSetup) SMTG_OVERRIDE;
    tresult PLUGIN_API setBusArrangements(SpeakerArrangement* inputs, int32 numIns,
                                          SpeakerArrangement* outputs, int32 numOuts) SMTG_OVERRIDE;

    //---from IEditController-------
    IPlugView* PLUGIN_API createView(FIDString name) SMTG_OVERRIDE;
    tresult PLUGIN_API setEditorState(IBStream* state) SMTG_OVERRIDE;
    tresult PLUGIN_API getEditorState(IBStream* state) SMTG_OVERRIDE;
    tresult PLUGIN_API setParamNormalized(ParamID tag, ParamValue value) SMTG_OVERRIDE;
    tresult PLUGIN_API getParamStringByValue(ParamID tag, ParamValue valueNormalized,
                                             String128 string) SMTG_OVERRIDE;

    //---from IMidiMapping-----------------
    tresult PLUGIN_API getMidiControllerAssignment(int32 busIndex, int16 channel,
                                                   CtrlNumber midiControllerNumber,
                                                   ParamID& tag) SMTG_OVERRIDE;

    //---Interface---------
    OBJ_METHODS(COxeVst3, SingleComponentEffect)
    tresult PLUGIN_API queryInterface(const TUID iid, void** obj) SMTG_OVERRIDE;
    REFCOUNT_METHODS(SingleComponentEffect)

    // GUI communication helpers
    void setParameterFromGUI(int index, float value);
    void setProgramFromGUI(int program);
    void updateHostDisplay();
    void setActiveEditor(COxeVst3Editor* editor);
    void onEditorDestroyed(COxeVst3Editor* editor);

private:
    void enqueueEvent(unsigned char bstat, unsigned char bdad1, unsigned char bdad2, int32 pos);
    void handleParamChange(ParamID id, ParamValue value, int32 pos);

    CSynthesizer synthesizer;
    int32 posExt;
    int32 posInt;
    int32 bufferPos;

    struct MIDIEvent
    {
        unsigned char bstat;
        unsigned char bdad1;
        unsigned char bdad2;
        int32 pos;
    };
    struct MIDIEvents
    {
        int32 eventsCount;
        int32 nextEvent;
        MIDIEvent event[1 << MAX_EVENTS_AT_ONCE_POWEROFTWO];
    };
    MIDIEvents events;

    COxeVst3Editor* activeEditor;
    std::atomic<bool> isUpdatingProgramFromHost;
};

} // namespace Vst
} // namespace Steinberg
