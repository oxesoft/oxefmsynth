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

#include "oxeclap.h"
#include "claphostinterface.h"

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

static const char *const s_features[] = {
    CLAP_PLUGIN_FEATURE_INSTRUMENT,
    CLAP_PLUGIN_FEATURE_SYNTHESIZER,
    CLAP_PLUGIN_FEATURE_STEREO,
    nullptr
};

const clap_plugin_descriptor_t COxeClapPlugin::s_descriptor = {
    CLAP_VERSION,
    "com.oxesoft.oxefmsynth",
    "Oxe FM Synth",
    "Oxesoft",
    "https://github.com/oxesoft/oxefmsynth",
    "",
    "",
    VERSION_STR,
    "Oxe FM Synth is an open source 8-operator FM synthesizer",
    s_features
};

static const clap_plugin_audio_ports_t s_audio_ports = {
    COxeClapPlugin::audio_ports_count,
    COxeClapPlugin::audio_ports_get,
};

static const clap_plugin_note_ports_t s_note_ports = {
    COxeClapPlugin::note_ports_count,
    COxeClapPlugin::note_ports_get,
};

static const clap_plugin_params_t s_params = {
    COxeClapPlugin::params_count,
    COxeClapPlugin::params_get_info,
    COxeClapPlugin::params_get_value,
    COxeClapPlugin::params_value_to_text,
    COxeClapPlugin::params_text_to_value,
    COxeClapPlugin::params_flush,
};

static const clap_plugin_state_t s_state = {
    COxeClapPlugin::state_save,
    COxeClapPlugin::state_load,
};

static const clap_plugin_gui_t s_gui = {
    COxeClapPlugin::gui_is_api_supported,
    COxeClapPlugin::gui_get_preferred_api,
    COxeClapPlugin::gui_create,
    COxeClapPlugin::gui_destroy,
    COxeClapPlugin::gui_set_scale,
    COxeClapPlugin::gui_get_size,
    COxeClapPlugin::gui_can_resize,
    COxeClapPlugin::gui_get_resize_hints,
    COxeClapPlugin::gui_adjust_size,
    COxeClapPlugin::gui_set_size,
    COxeClapPlugin::gui_set_parent,
    COxeClapPlugin::gui_set_transient,
    COxeClapPlugin::gui_suggest_title,
    COxeClapPlugin::gui_show,
    COxeClapPlugin::gui_hide,
};

COxeClapPlugin::COxeClapPlugin(const clap_host_t *host)
    : host(host), hostParams(nullptr), synthEditor(nullptr),
      posExt(0), posInt(0), bufferPos(0),
      guiEditor(nullptr), guiToolkit(nullptr), guiHostInterface(nullptr),
      isUpdatingProgramFromHost(false)
{
    memset(&plugin, 0, sizeof(plugin));
    plugin.desc = &s_descriptor;
    plugin.plugin_data = this;
    plugin.init = plugin_init;
    plugin.destroy = plugin_destroy;
    plugin.activate = plugin_activate;
    plugin.deactivate = plugin_deactivate;
    plugin.start_processing = plugin_start_processing;
    plugin.stop_processing = plugin_stop_processing;
    plugin.reset = plugin_reset;
    plugin.process = plugin_process;
    plugin.get_extension = plugin_get_extension;
    plugin.on_main_thread = plugin_on_main_thread;

    memset(&events, 0, sizeof(events));
    synthEditor = new CEditor(&synthesizer);
    for (int i = 0; i < PARAMETERS_COUNT; i++)
    {
        defaultParamValues[i] = (double)synthEditor->GetPar(i);
    }
}


COxeClapPlugin::~COxeClapPlugin()
{
    gui_destroy(&plugin);
    delete synthEditor;
}

bool COxeClapPlugin::plugin_init(const struct clap_plugin *plugin)
{
    COxeClapPlugin *plug = (COxeClapPlugin *)plugin->plugin_data;
    if (plug->host)
    {
        plug->hostParams = (const clap_host_params_t *)plug->host->get_extension(plug->host, CLAP_EXT_PARAMS);
    }
    return true;
}

void COxeClapPlugin::plugin_destroy(const struct clap_plugin *plugin)
{
    COxeClapPlugin *plug = (COxeClapPlugin *)plugin->plugin_data;
    delete plug;
}

bool COxeClapPlugin::plugin_activate(const struct clap_plugin *plugin, double sample_rate, uint32_t min_frames_count, uint32_t max_frames_count)
{
    COxeClapPlugin *plug = (COxeClapPlugin *)plugin->plugin_data;
    plug->synthesizer.SetSampleRate((float)sample_rate);
    plug->posExt = 0;
    plug->posInt = 0;
    plug->bufferPos = 0;
    return true;
}

void COxeClapPlugin::plugin_deactivate(const struct clap_plugin *plugin)
{
    COxeClapPlugin *plug = (COxeClapPlugin *)plugin->plugin_data;
    plug->synthesizer.KillNotes();
}

bool COxeClapPlugin::plugin_start_processing(const struct clap_plugin *plugin)
{
    return true;
}

void COxeClapPlugin::plugin_stop_processing(const struct clap_plugin *plugin)
{
}

void COxeClapPlugin::plugin_reset(const struct clap_plugin *plugin)
{
    COxeClapPlugin *plug = (COxeClapPlugin *)plugin->plugin_data;
    plug->synthesizer.KillNotes();
    plug->posExt = 0;
    plug->posInt = 0;
}

const void *COxeClapPlugin::plugin_get_extension(const struct clap_plugin *plugin, const char *id)
{
    if (strcmp(id, CLAP_EXT_AUDIO_PORTS) == 0)
        return &s_audio_ports;
    if (strcmp(id, CLAP_EXT_NOTE_PORTS) == 0)
        return &s_note_ports;
    if (strcmp(id, CLAP_EXT_PARAMS) == 0)
        return &s_params;
    if (strcmp(id, CLAP_EXT_STATE) == 0)
        return &s_state;
    if (strcmp(id, CLAP_EXT_GUI) == 0)
        return &s_gui;
    return nullptr;
}

void COxeClapPlugin::plugin_on_main_thread(const struct clap_plugin *plugin)
{
}

void COxeClapPlugin::enqueueEvent(unsigned char bstat, unsigned char bdad1, unsigned char bdad2, int32_t pos)
{
    if (events.eventsCount >= (1 << MAX_EVENTS_AT_ONCE_POWEROFTWO))
        return;
    int n = (events.nextEvent + events.eventsCount) & EVENTS_MASK;
    events.event[n].bstat = bstat;
    events.event[n].bdad1 = bdad1;
    events.event[n].bdad2 = bdad2;
    events.event[n].pos = pos;
    events.eventsCount++;
}

void COxeClapPlugin::handleParamChange(clap_id id, double value, int32_t pos)
{
    if (id < PARAMETERS_COUNT)
    {
        synthEditor->SetPar((int)id, (float)value);
        if (guiEditor)
        {
            guiEditor->SetPar((int)id, (float)value);
            guiEditor->Update();
        }
        return;
    }

    if (id == kProgramParamId)
    {
        int prog = (int)std::lrint(value);
        prog = std::max(0, std::min(MAX_PROGRAMS - 1, prog));
        isUpdatingProgramFromHost.store(true, std::memory_order_relaxed);
        synthesizer.SendEvent(0xC0, (unsigned char)prog, 0, pos);
        isUpdatingProgramFromHost.store(false, std::memory_order_relaxed);
        if (guiEditor)
        {
            guiEditor->ProgramChanged();
        }
        return;
    }

}

void COxeClapPlugin::queueParamChange(clap_id param_id, double value)
{
    QueuedParamEvent evt;
    evt.param_id = param_id;
    evt.value = value;
    outputEventQueue.push(evt);

    if (hostParams && host)
    {
        hostParams->request_flush(host);
    }
    else if (host)
    {
        host->request_process(host);
    }
}

void COxeClapPlugin::drainOutputEvents(const clap_output_events_t *out)
{
    if (!out) return;

    QueuedParamEvent qevt;
    while (outputEventQueue.pop(qevt))
    {
        clap_event_param_gesture_t begin_gest;
        memset(&begin_gest, 0, sizeof(begin_gest));
        begin_gest.header.size = sizeof(begin_gest);
        begin_gest.header.time = 0;
        begin_gest.header.space_id = CLAP_CORE_EVENT_SPACE_ID;
        begin_gest.header.type = CLAP_EVENT_PARAM_GESTURE_BEGIN;
        begin_gest.header.flags = 0;
        begin_gest.param_id = qevt.param_id;
        out->try_push(out, &begin_gest.header);

        clap_event_param_value_t val_evt;
        memset(&val_evt, 0, sizeof(val_evt));
        val_evt.header.size = sizeof(val_evt);
        val_evt.header.time = 0;
        val_evt.header.space_id = CLAP_CORE_EVENT_SPACE_ID;
        val_evt.header.type = CLAP_EVENT_PARAM_VALUE;
        val_evt.header.flags = 0;
        val_evt.param_id = qevt.param_id;
        val_evt.cookie = nullptr;
        val_evt.note_id = -1;
        val_evt.port_index = 0;
        val_evt.channel = -1;
        val_evt.key = -1;
        val_evt.value = qevt.value;
        out->try_push(out, &val_evt.header);

        clap_event_param_gesture_t end_gest;
        memset(&end_gest, 0, sizeof(end_gest));
        end_gest.header.size = sizeof(end_gest);
        end_gest.header.time = 0;
        end_gest.header.space_id = CLAP_CORE_EVENT_SPACE_ID;
        end_gest.header.type = CLAP_EVENT_PARAM_GESTURE_END;
        end_gest.header.flags = 0;
        end_gest.param_id = qevt.param_id;
        out->try_push(out, &end_gest.header);
    }
}

clap_process_status COxeClapPlugin::plugin_process(const struct clap_plugin *plugin, const clap_process_t *process)
{
    COxeClapPlugin *plug = (COxeClapPlugin *)plugin->plugin_data;

    // 1. Process incoming events
    if (process->in_events)
    {
        uint32_t size = process->in_events->size(process->in_events);
        for (uint32_t i = 0; i < size; ++i)
        {
            const clap_event_header_t *hdr = process->in_events->get(process->in_events, i);
            if (hdr->space_id != CLAP_CORE_EVENT_SPACE_ID)
                continue;

            switch (hdr->type)
            {
                case CLAP_EVENT_NOTE_ON:
                {
                    const clap_event_note_t *note = (const clap_event_note_t *)hdr;
                    unsigned char ch = (unsigned char)(note->channel < 0 ? 0 : (note->channel & 0x0F));
                    unsigned char key = (unsigned char)(note->key < 0 ? 0 : (note->key & 0x7F));
                    int velInt = (int)std::lrint(note->velocity * 127.0);
                    unsigned char vel = (unsigned char)std::clamp(velInt, 0, 127);
                    plug->enqueueEvent(0x90 | ch, key, vel, plug->bufferPos + hdr->time);
                    break;
                }
                case CLAP_EVENT_NOTE_OFF:
                {
                    const clap_event_note_t *note = (const clap_event_note_t *)hdr;
                    unsigned char ch = (unsigned char)(note->channel < 0 ? 0 : (note->channel & 0x0F));
                    unsigned char key = (unsigned char)(note->key < 0 ? 0 : (note->key & 0x7F));
                    int velInt = (int)std::lrint(note->velocity * 127.0);
                    unsigned char vel = (unsigned char)std::clamp(velInt, 0, 127);
                    plug->enqueueEvent(0x80 | ch, key, vel, plug->bufferPos + hdr->time);
                    break;
                }
                case CLAP_EVENT_NOTE_CHOKE:
                {
                    const clap_event_note_t *note = (const clap_event_note_t *)hdr;
                    unsigned char ch = (unsigned char)(note->channel < 0 ? 0 : (note->channel & 0x0F));
                    unsigned char key = (unsigned char)(note->key < 0 ? 0 : (note->key & 0x7F));
                    plug->enqueueEvent(0x80 | ch, key, 0, plug->bufferPos + hdr->time);
                    break;
                }
                case CLAP_EVENT_MIDI:
                {
                    const clap_event_midi_t *midi = (const clap_event_midi_t *)hdr;
                    plug->enqueueEvent(midi->data[0], midi->data[1], midi->data[2], plug->bufferPos + hdr->time);
                    if ((midi->data[0] & 0xF0) == 0xC0 && plug->guiEditor)
                    {
                        plug->guiEditor->ProgramChanged();
                    }
                    break;
                }
                case CLAP_EVENT_NOTE_EXPRESSION:
                {
                    const clap_event_note_expression_t *expr = (const clap_event_note_expression_t *)hdr;
                    if (expr->expression_id == CLAP_NOTE_EXPRESSION_PRESSURE)
                    {
                        unsigned char ch = (unsigned char)(expr->channel < 0 ? 0 : (expr->channel & 0x0F));
                        unsigned char key = (unsigned char)(expr->key < 0 ? 0 : (expr->key & 0x7F));
                        int val = (int)std::lrint(expr->value * 127.0);
                        plug->enqueueEvent(0xA0 | ch, key, (unsigned char)std::clamp(val, 0, 127), plug->bufferPos + hdr->time);
                    }
                    break;
                }
                case CLAP_EVENT_PARAM_VALUE:
                {
                    const clap_event_param_value_t *pv = (const clap_event_param_value_t *)hdr;
                    plug->handleParamChange(pv->param_id, pv->value, plug->bufferPos + hdr->time);
                    break;
                }
                default:
                    break;
            }
        }
    }

    // 2. Synthesize audio frames
    if (process->audio_outputs_count > 0 && process->frames_count > 0 && process->audio_outputs[0].data32)
    {
        float *out1 = process->audio_outputs[0].data32[0];
        float *out2 = process->audio_outputs[0].data32[1];

        if (out1 && out2)
        {
            int32_t tambufferInt = SAMPLES_PER_PROCESS << 1;
            int32_t tambufferExt = process->frames_count;

            while (true)
            {
                if (!plug->posInt)
                {
                    while (plug->events.eventsCount)
                    {
                        if (plug->events.event[plug->events.nextEvent].pos > plug->bufferPos + SAMPLES_PER_PROCESS)
                            break;
                        if (plug->events.event[plug->events.nextEvent].pos < plug->bufferPos)
                            plug->events.event[plug->events.nextEvent].pos = plug->bufferPos;
                        bool isProg = (plug->events.event[plug->events.nextEvent].bstat & 0xF0) == 0xC0;
                        if (isProg)
                            plug->isUpdatingProgramFromHost.store(true, std::memory_order_relaxed);
                        plug->synthesizer.SendEvent(plug->events.event[plug->events.nextEvent].bstat,
                                                    plug->events.event[plug->events.nextEvent].bdad1,
                                                    plug->events.event[plug->events.nextEvent].bdad2,
                                                    plug->events.event[plug->events.nextEvent].pos);
                        if (isProg)
                            plug->isUpdatingProgramFromHost.store(false, std::memory_order_relaxed);
                        plug->events.eventsCount--;
                        plug->events.nextEvent++;
                        plug->events.nextEvent &= EVENTS_MASK;
                    }
                    plug->synthesizer.Process(plug->synthesizer.buffers.bSynthOut, SAMPLES_PER_PROCESS, plug->bufferPos);
                    plug->bufferPos += SAMPLES_PER_PROCESS;
                }

                int32_t iaux = std::min(tambufferInt - plug->posInt, tambufferExt - plug->posExt);
                while (iaux > 0)
                {
                    out1[plug->posExt] = float(plug->synthesizer.buffers.bSynthOut[plug->posInt++]) / 32767.f;
                    out2[plug->posExt] = float(plug->synthesizer.buffers.bSynthOut[plug->posInt++]) / 32767.f;
                    plug->posExt++;
                    iaux -= 2;
                }

                if (plug->posInt >= tambufferInt)
                    plug->posInt = 0;
                if (plug->posExt >= tambufferExt)
                {
                    plug->posExt = 0;
                    break;
                }
            }
        }
    }

    // 3. Drain GUI-originated parameter changes
    plug->drainOutputEvents(process->out_events);

    return CLAP_PROCESS_CONTINUE;
}

// Audio ports
uint32_t COxeClapPlugin::audio_ports_count(const clap_plugin_t *plugin, bool is_input)
{
    return is_input ? 0 : 1;
}

bool COxeClapPlugin::audio_ports_get(const clap_plugin_t *plugin, uint32_t index, bool is_input, clap_audio_port_info_t *info)
{
    if (is_input || index != 0 || !info)
        return false;
    info->id = 0;
    snprintf(info->name, sizeof(info->name), "Stereo Out");
    info->flags = CLAP_AUDIO_PORT_IS_MAIN;
    info->channel_count = 2;
    info->port_type = CLAP_PORT_STEREO;
    info->in_place_pair = CLAP_INVALID_ID;
    return true;
}

// Note ports
uint32_t COxeClapPlugin::note_ports_count(const clap_plugin_t *plugin, bool is_input)
{
    return is_input ? 1 : 0;
}

bool COxeClapPlugin::note_ports_get(const clap_plugin_t *plugin, uint32_t index, bool is_input, clap_note_port_info_t *info)
{
    if (!is_input || index != 0 || !info)
        return false;
    info->id = 0;
    info->supported_dialects = CLAP_NOTE_DIALECT_CLAP | CLAP_NOTE_DIALECT_MIDI;
    info->preferred_dialect = CLAP_NOTE_DIALECT_CLAP;
    snprintf(info->name, sizeof(info->name), "Note In");
    return true;
}

// Parameters
uint32_t COxeClapPlugin::params_count(const clap_plugin_t *plugin)
{
    return PARAMETERS_COUNT + 1;
}

bool COxeClapPlugin::params_get_info(const clap_plugin_t *plugin, uint32_t param_index, clap_param_info_t *param_info)
{
    COxeClapPlugin *plug = (COxeClapPlugin *)plugin->plugin_data;
    if (!param_info) return false;
    memset(param_info, 0, sizeof(*param_info));

    if (param_index < PARAMETERS_COUNT)
    {
        param_info->id = (clap_id)param_index;
        param_info->flags = CLAP_PARAM_IS_AUTOMATABLE;
        plug->synthEditor->GetParName((int)param_index, param_info->name);
        if (param_info->name[0] == 0 || strcmp(param_info->name, "not found") == 0)
        {
            snprintf(param_info->name, sizeof(param_info->name), "Param %u", param_index + 1);
        }

        if (param_index <= 12)
            snprintf(param_info->module, sizeof(param_info->module), "Operators/Op A");
        else if (param_index <= 25)
            snprintf(param_info->module, sizeof(param_info->module), "Operators/Op B");
        else if (param_index <= 38)
            snprintf(param_info->module, sizeof(param_info->module), "Operators/Op C");
        else if (param_index <= 51)
            snprintf(param_info->module, sizeof(param_info->module), "Operators/Op D");
        else if (param_index <= 64)
            snprintf(param_info->module, sizeof(param_info->module), "Operators/Op E");
        else if (param_index <= 77)
            snprintf(param_info->module, sizeof(param_info->module), "Operators/Op F");
        else if (param_index <= 90)
            snprintf(param_info->module, sizeof(param_info->module), "Operators/Op X (Noise)");
        else if (param_index <= 103)
            snprintf(param_info->module, sizeof(param_info->module), "Operators/Op Z (Filter)");
        else if (param_index <= 153)
            snprintf(param_info->module, sizeof(param_info->module), "Modulation Matrix");
        else if (param_index <= 158)
            snprintf(param_info->module, sizeof(param_info->module), "LFO");
        else if (param_index <= 163)
            snprintf(param_info->module, sizeof(param_info->module), "Master");
        else
            snprintf(param_info->module, sizeof(param_info->module), "Master/Effects");

        param_info->min_value = 0.0;
        param_info->max_value = 1.0;
        param_info->default_value = plug->defaultParamValues[param_index];
        return true;
    }

    if (param_index == PARAMETERS_COUNT)
    {
        param_info->id = kProgramParamId;
        param_info->flags = CLAP_PARAM_IS_AUTOMATABLE | CLAP_PARAM_IS_STEPPED;
        snprintf(param_info->name, sizeof(param_info->name), "Program");
        snprintf(param_info->module, sizeof(param_info->module), "Programs");
        param_info->min_value = 0.0;
        param_info->max_value = (double)(MAX_PROGRAMS - 1);
        param_info->default_value = 0.0;
        return true;
    }

    return false;
}

bool COxeClapPlugin::params_get_value(const clap_plugin_t *plugin, clap_id param_id, double *out_value)
{
    COxeClapPlugin *plug = (COxeClapPlugin *)plugin->plugin_data;
    if (!out_value) return false;

    if (param_id < PARAMETERS_COUNT)
    {
        *out_value = (double)plug->synthEditor->GetPar((int)param_id);
        return true;
    }

    if (param_id == kProgramParamId)
    {
        *out_value = (double)plug->synthesizer.GetNumProgr(0);
        return true;
    }

    return false;
}

bool COxeClapPlugin::params_value_to_text(const clap_plugin_t *plugin, clap_id param_id, double value, char *out_buffer, uint32_t out_buffer_capacity)
{
    return false;
}


bool COxeClapPlugin::params_text_to_value(const clap_plugin_t *plugin, clap_id param_id, const char *param_value_text, double *out_value)
{
    return false;
}


void COxeClapPlugin::params_flush(const clap_plugin_t *plugin, const clap_input_events_t *in, const clap_output_events_t *out)
{
    COxeClapPlugin *plug = (COxeClapPlugin *)plugin->plugin_data;

    if (in)
    {
        uint32_t size = in->size(in);
        for (uint32_t i = 0; i < size; ++i)
        {
            const clap_event_header_t *hdr = in->get(in, i);
            if (hdr->space_id == CLAP_CORE_EVENT_SPACE_ID && hdr->type == CLAP_EVENT_PARAM_VALUE)
            {
                const clap_event_param_value_t *pv = (const clap_event_param_value_t *)hdr;
                plug->handleParamChange(pv->param_id, pv->value, plug->bufferPos);
            }
        }
    }

    plug->drainOutputEvents(out);
}

// State
bool COxeClapPlugin::state_save(const clap_plugin_t *plugin, const clap_ostream_t *stream)
{
    COxeClapPlugin *plug = (COxeClapPlugin *)plugin->plugin_data;
    if (!stream || !stream->write) return false;

    auto writeAll = [stream](const void *data, uint64_t size) -> bool {
        const char *buf = (const char *)data;
        uint64_t written = 0;
        while (written < size)
        {
            int64_t n = stream->write(stream, buf + written, size - written);
            if (n <= 0) return false;
            written += (uint64_t)n;
        }
        return true;
    };

    SBank *bank = plug->synthesizer.GetBank();
    if (!writeAll(bank, sizeof(SBank)))
        return false;

    int32_t currentProg = plug->synthesizer.GetNumProgr(0);
    if (!writeAll(&currentProg, sizeof(int32_t)))
        return false;

    float revLevel = plug->synthesizer.GetPar(0, RVBLV);
    if (!writeAll(&revLevel, sizeof(float)))
        return false;

    float dlyLevel = plug->synthesizer.GetPar(0, DLYLV);
    if (!writeAll(&dlyLevel, sizeof(float)))
        return false;

    return true;
}

bool COxeClapPlugin::state_load(const clap_plugin_t *plugin, const clap_istream_t *stream)
{
    COxeClapPlugin *plug = (COxeClapPlugin *)plugin->plugin_data;
    if (!stream || !stream->read) return false;

    auto readAll = [stream](void *data, uint64_t size) -> bool {
        char *buf = (char *)data;
        uint64_t readBytes = 0;
        while (readBytes < size)
        {
            int64_t n = stream->read(stream, buf + readBytes, size - readBytes);
            if (n <= 0) return false;
            readBytes += (uint64_t)n;
        }
        return true;
    };

    SBank bank;
    if (!readAll(&bank, sizeof(SBank)))
        return false;

    auto isValidFloat = [](float f, float minVal, float maxVal) {
        return std::isfinite(f) && f >= minVal && f <= maxVal;
    };
    if (!isValidFloat(bank.global.REVTI, 0.0f, 1.0f) ||
        !isValidFloat(bank.global.REVDA, 0.0f, 1.0f) ||
        !isValidFloat(bank.global.DLYTI, 0.0f, 1.0f) ||
        !isValidFloat(bank.global.DLYFE, 0.0f, 1.0f) ||
        !isValidFloat(bank.global.DLYLF, 0.0f, 20.0f) ||
        !isValidFloat(bank.global.DLYLA, 0.0f, 1.0f) ||
        !isValidFloat(bank.prg[0].OPAON, 0.0f, 1.0f) ||
        !isValidFloat(bank.prg[0].OPAWF, 0.0f, 6.0f) ||
        !isValidFloat(bank.prg[0].OPACT, 0.0f, 100.0f) ||
        !isValidFloat(bank.prg[0].MAP, -1.0f, 1.0f) ||
        !isValidFloat(bank.prg[0].MAO, 0.0f, 1.0f))
    {
        return false;
    }

    int32_t currentProg = 0;
    bool hasProg = readAll(&currentProg, sizeof(int32_t));
    if (hasProg)
    {
        if (currentProg < 0 || currentProg >= MAX_PROGRAMS)
            return false;
    }

    float revLevel = 0.0f;
    bool hasRev = readAll(&revLevel, sizeof(float));
    if (hasRev && !isValidFloat(revLevel, 0.0f, 1.0f))
        return false;

    float dlyLevel = 0.0f;
    bool hasDly = readAll(&dlyLevel, sizeof(float));
    if (hasDly && !isValidFloat(dlyLevel, 0.0f, 1.0f))
        return false;

    plug->synthesizer.SetBank(&bank);

    if (hasProg)
    {
        plug->isUpdatingProgramFromHost.store(true, std::memory_order_relaxed);
        plug->synthesizer.SendEvent(0xC0, (unsigned char)currentProg, 0, 0);
        plug->isUpdatingProgramFromHost.store(false, std::memory_order_relaxed);
    }
    if (hasRev)
    {
        plug->synthesizer.SetPar(0, RVBLV, revLevel);
    }
    if (hasDly)
    {
        plug->synthesizer.SetPar(0, DLYLV, dlyLevel);
    }

    if (plug->guiEditor)
    {
        plug->guiEditor->ProgramChanged();
        plug->guiEditor->Update();
    }

    plug->updateHostDisplay();
    return true;
}



// GUI
bool COxeClapPlugin::gui_is_api_supported(const clap_plugin_t *plugin, const char *api, bool is_floating)
{
    if (is_floating) return false;
#if defined(__APPLE__)
    return strcmp(api, CLAP_WINDOW_API_COCOA) == 0;
#elif defined(_WIN32)
    return strcmp(api, CLAP_WINDOW_API_WIN32) == 0;
#elif defined(__linux__)
    return strcmp(api, CLAP_WINDOW_API_X11) == 0;
#else
    return false;
#endif
}

bool COxeClapPlugin::gui_get_preferred_api(const clap_plugin_t *plugin, const char **api, bool *is_floating)
{
    if (!api || !is_floating) return false;
    *is_floating = false;
#if defined(__APPLE__)
    *api = CLAP_WINDOW_API_COCOA;
    return true;
#elif defined(_WIN32)
    *api = CLAP_WINDOW_API_WIN32;
    return true;
#elif defined(__linux__)
    *api = CLAP_WINDOW_API_X11;
    return true;
#else
    return false;
#endif
}

bool COxeClapPlugin::gui_create(const clap_plugin_t *plugin, const char *api, bool is_floating)
{
    return gui_is_api_supported(plugin, api, is_floating);
}

void COxeClapPlugin::gui_destroy(const clap_plugin_t *plugin)
{
    COxeClapPlugin *plug = (COxeClapPlugin *)plugin->plugin_data;
    if (plug->guiEditor)
    {
        plug->guiEditor->SetToolkit(nullptr);
        plug->guiEditor->SetHostInterface(nullptr);
    }
    delete plug->guiToolkit;
    plug->guiToolkit = nullptr;
    delete plug->guiHostInterface;
    plug->guiHostInterface = nullptr;
    delete plug->guiEditor;
    plug->guiEditor = nullptr;
}

bool COxeClapPlugin::gui_set_scale(const clap_plugin_t *plugin, double scale)
{
    return false;
}

bool COxeClapPlugin::gui_get_size(const clap_plugin_t *plugin, uint32_t *width, uint32_t *height)
{
    if (!width || !height) return false;
    *width = GUI_WIDTH;
    *height = GUI_HEIGHT;
    return true;
}

bool COxeClapPlugin::gui_can_resize(const clap_plugin_t *plugin)
{
    return false;
}

bool COxeClapPlugin::gui_get_resize_hints(const clap_plugin_t *plugin, clap_gui_resize_hints_t *hints)
{
    if (!hints) return false;
    hints->can_resize_horizontally = false;
    hints->can_resize_vertically = false;
    hints->preserve_aspect_ratio = true;
    hints->aspect_ratio_width = GUI_WIDTH;
    hints->aspect_ratio_height = GUI_HEIGHT;
    return true;
}

bool COxeClapPlugin::gui_adjust_size(const clap_plugin_t *plugin, uint32_t *width, uint32_t *height)
{
    if (!width || !height) return false;
    *width = GUI_WIDTH;
    *height = GUI_HEIGHT;
    return true;
}

bool COxeClapPlugin::gui_set_size(const clap_plugin_t *plugin, uint32_t width, uint32_t height)
{
    return (width == GUI_WIDTH && height == GUI_HEIGHT);
}

bool COxeClapPlugin::gui_set_parent(const clap_plugin_t *plugin, const clap_window_t *window)
{
    COxeClapPlugin *plug = (COxeClapPlugin *)plugin->plugin_data;
    if (!window) return false;

    void *parentHandle = nullptr;
#if defined(__APPLE__)
    if (strcmp(window->api, CLAP_WINDOW_API_COCOA) != 0)
        return false;
    parentHandle = window->cocoa;
#elif defined(_WIN32)
    if (strcmp(window->api, CLAP_WINDOW_API_WIN32) != 0)
        return false;
    parentHandle = window->win32;
#elif defined(__linux__)
    if (strcmp(window->api, CLAP_WINDOW_API_X11) != 0)
        return false;
    parentHandle = (void *)window->x11;
#endif

    if (!parentHandle)
        return false;

    if (plug->guiEditor)
    {
        gui_destroy(plugin);
    }

    plug->guiHostInterface = new CClapHostInterface(plug);
    plug->guiEditor = new CEditor(&plug->synthesizer);
    plug->guiToolkit = new COSToolkit(parentHandle, plug->guiEditor);
    plug->guiEditor->SetToolkit(plug->guiToolkit);
    plug->guiEditor->SetHostInterface(plug->guiHostInterface);
    plug->guiToolkit->StartWindowProcesses();

    return true;
}

bool COxeClapPlugin::gui_set_transient(const clap_plugin_t *plugin, const clap_window_t *window)
{
    return false;
}

void COxeClapPlugin::gui_suggest_title(const clap_plugin_t *plugin, const char *title)
{
}

bool COxeClapPlugin::gui_show(const clap_plugin_t *plugin)
{
    return true;
}

bool COxeClapPlugin::gui_hide(const clap_plugin_t *plugin)
{
    return true;
}

void COxeClapPlugin::setParameterFromGUI(int index, float value)
{
    synthEditor->SetPar(index, value);
    queueParamChange((clap_id)index, (double)value);
}

void COxeClapPlugin::setProgramFromGUI(int program)
{
    if (isUpdatingProgramFromHost.load(std::memory_order_relaxed))
        return;

    queueParamChange(kProgramParamId, (double)program);
    updateHostDisplay();
}

void COxeClapPlugin::updateHostDisplay()
{
    if (!hostParams && host)
    {
        hostParams = (const clap_host_params_t *)host->get_extension(host, CLAP_EXT_PARAMS);
    }
    if (hostParams && host)
    {
        hostParams->rescan(host, CLAP_PARAM_RESCAN_VALUES | CLAP_PARAM_RESCAN_TEXT);
    }
}


