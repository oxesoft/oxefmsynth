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

#include <clap/clap.h>
#include <atomic>
#include <cstdint>
#ifdef RELEASE
#undef RELEASE
#endif

#include "editor.h"
#include "ostoolkit.h"

class CClapHostInterface;

constexpr clap_id kProgramParamId = 1000;

#define MAX_EVENTS_AT_ONCE_POWEROFTWO 8
#define EVENTS_MASK                   ((1 << MAX_EVENTS_AT_ONCE_POWEROFTWO) - 1)

struct QueuedParamEvent
{
    clap_id param_id;
    double value;
};

template <typename T, size_t Capacity>
class LockFreeQueue
{
public:
    LockFreeQueue() : head(0), tail(0) {}

    bool push(const T& item)
    {
        size_t current_tail = tail.load(std::memory_order_relaxed);
        size_t next_tail = (current_tail + 1) % Capacity;
        if (next_tail == head.load(std::memory_order_acquire))
        {
            return false; // queue is full
        }
        buffer[current_tail] = item;
        tail.store(next_tail, std::memory_order_release);
        return true;
    }

    bool pop(T& item)
    {
        size_t current_head = head.load(std::memory_order_relaxed);
        if (current_head == tail.load(std::memory_order_acquire))
        {
            return false; // queue is empty
        }
        item = buffer[current_head];
        head.store((current_head + 1) % Capacity, std::memory_order_release);
        return true;
    }

private:
    T buffer[Capacity];
    std::atomic<size_t> head;
    std::atomic<size_t> tail;
};

class COxeClapPlugin
{
public:
    explicit COxeClapPlugin(const clap_host_t *host);
    ~COxeClapPlugin();

    const clap_plugin_t *clapPlugin() const { return &plugin; }

    static const clap_plugin_descriptor_t s_descriptor;

    // GUI callbacks
    void setParameterFromGUI(int index, float value);
    void setProgramFromGUI(int program);
    void updateHostDisplay();

    // CLAP plugin callbacks
    static bool CLAP_ABI plugin_init(const struct clap_plugin *plugin);
    static void CLAP_ABI plugin_destroy(const struct clap_plugin *plugin);
    static bool CLAP_ABI plugin_activate(const struct clap_plugin *plugin, double sample_rate, uint32_t min_frames_count, uint32_t max_frames_count);
    static void CLAP_ABI plugin_deactivate(const struct clap_plugin *plugin);
    static bool CLAP_ABI plugin_start_processing(const struct clap_plugin *plugin);
    static void CLAP_ABI plugin_stop_processing(const struct clap_plugin *plugin);
    static void CLAP_ABI plugin_reset(const struct clap_plugin *plugin);
    static clap_process_status CLAP_ABI plugin_process(const struct clap_plugin *plugin, const clap_process_t *process);
    static const void *CLAP_ABI plugin_get_extension(const struct clap_plugin *plugin, const char *id);
    static void CLAP_ABI plugin_on_main_thread(const struct clap_plugin *plugin);

    // Audio ports extension
    static uint32_t CLAP_ABI audio_ports_count(const clap_plugin_t *plugin, bool is_input);
    static bool CLAP_ABI audio_ports_get(const clap_plugin_t *plugin, uint32_t index, bool is_input, clap_audio_port_info_t *info);

    // Note ports extension
    static uint32_t CLAP_ABI note_ports_count(const clap_plugin_t *plugin, bool is_input);
    static bool CLAP_ABI note_ports_get(const clap_plugin_t *plugin, uint32_t index, bool is_input, clap_note_port_info_t *info);

    // Params extension
    static uint32_t CLAP_ABI params_count(const clap_plugin_t *plugin);
    static bool CLAP_ABI params_get_info(const clap_plugin_t *plugin, uint32_t param_index, clap_param_info_t *param_info);
    static bool CLAP_ABI params_get_value(const clap_plugin_t *plugin, clap_id param_id, double *out_value);
    static bool CLAP_ABI params_value_to_text(const clap_plugin_t *plugin, clap_id param_id, double value, char *out_buffer, uint32_t out_buffer_capacity);
    static bool CLAP_ABI params_text_to_value(const clap_plugin_t *plugin, clap_id param_id, const char *param_value_text, double *out_value);
    static void CLAP_ABI params_flush(const clap_plugin_t *plugin, const clap_input_events_t *in, const clap_output_events_t *out);

    // State extension
    static bool CLAP_ABI state_save(const clap_plugin_t *plugin, const clap_ostream_t *stream);
    static bool CLAP_ABI state_load(const clap_plugin_t *plugin, const clap_istream_t *stream);

    // GUI extension
    static bool CLAP_ABI gui_is_api_supported(const clap_plugin_t *plugin, const char *api, bool is_floating);
    static bool CLAP_ABI gui_get_preferred_api(const clap_plugin_t *plugin, const char **api, bool *is_floating);
    static bool CLAP_ABI gui_create(const clap_plugin_t *plugin, const char *api, bool is_floating);
    static void CLAP_ABI gui_destroy(const clap_plugin_t *plugin);
    static bool CLAP_ABI gui_set_scale(const clap_plugin_t *plugin, double scale);
    static bool CLAP_ABI gui_get_size(const clap_plugin_t *plugin, uint32_t *width, uint32_t *height);
    static bool CLAP_ABI gui_can_resize(const clap_plugin_t *plugin);
    static bool CLAP_ABI gui_get_resize_hints(const clap_plugin_t *plugin, clap_gui_resize_hints_t *hints);
    static bool CLAP_ABI gui_adjust_size(const clap_plugin_t *plugin, uint32_t *width, uint32_t *height);
    static bool CLAP_ABI gui_set_size(const clap_plugin_t *plugin, uint32_t width, uint32_t height);
    static bool CLAP_ABI gui_set_parent(const clap_plugin_t *plugin, const clap_window_t *window);
    static bool CLAP_ABI gui_set_transient(const clap_plugin_t *plugin, const clap_window_t *window);
    static void CLAP_ABI gui_suggest_title(const clap_plugin_t *plugin, const char *title);
    static bool CLAP_ABI gui_show(const clap_plugin_t *plugin);
    static bool CLAP_ABI gui_hide(const clap_plugin_t *plugin);

private:

    // Internal helpers
    void enqueueEvent(unsigned char bstat, unsigned char bdad1, unsigned char bdad2, int32_t pos);
    void handleParamChange(clap_id id, double value, int32_t pos);
    void drainOutputEvents(const clap_output_events_t *out);
    void queueParamChange(clap_id param_id, double value);

    clap_plugin_t plugin;
    const clap_host_t *host;
    const clap_host_params_t *hostParams;

    CSynthesizer synthesizer;
    CEditor *synthEditor;

    int32_t posExt;
    int32_t posInt;
    int32_t bufferPos;

    struct MIDIEvent
    {
        unsigned char bstat;
        unsigned char bdad1;
        unsigned char bdad2;
        int32_t pos;
    };
    struct MIDIEvents
    {
        int32_t eventsCount;
        int32_t nextEvent;
        MIDIEvent event[1 << MAX_EVENTS_AT_ONCE_POWEROFTWO];
    };
    MIDIEvents events;

    LockFreeQueue<QueuedParamEvent, 256> outputEventQueue;

    double defaultParamValues[PARAMETERS_COUNT];

    // Active GUI
    CEditor *guiEditor;
    COSToolkit *guiToolkit;
    CClapHostInterface *guiHostInterface;
    std::atomic<bool> isUpdatingProgramFromHost;
};

