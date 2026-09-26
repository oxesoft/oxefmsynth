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

#include <clap/clap.h>
#include <cstring>
#include "oxeclap.h"

static uint32_t clap_factory_get_plugin_count(const clap_plugin_factory_t *factory)
{
    return 1;
}

static const clap_plugin_descriptor_t *clap_factory_get_plugin_descriptor(
    const clap_plugin_factory_t *factory, uint32_t index)
{
    if (index == 0)
        return &COxeClapPlugin::s_descriptor;
    return nullptr;
}

static const clap_plugin_t *clap_factory_create_plugin(
    const clap_plugin_factory_t *factory, const clap_host_t *host, const char *plugin_id)
{
    if (!host || !clap_version_is_compatible(host->clap_version))
        return nullptr;

    if (strcmp(plugin_id, COxeClapPlugin::s_descriptor.id) != 0)
        return nullptr;

    COxeClapPlugin *plug = new COxeClapPlugin(host);
    return plug->clapPlugin();
}

static const clap_plugin_factory_t s_plugin_factory = {
    clap_factory_get_plugin_count,
    clap_factory_get_plugin_descriptor,
    clap_factory_create_plugin,
};

static bool clap_entry_init(const char *plugin_path)
{
    return true;
}

static void clap_entry_deinit(void)
{
}

static const void *clap_entry_get_factory(const char *factory_id)
{
    if (strcmp(factory_id, CLAP_PLUGIN_FACTORY_ID) == 0)
        return &s_plugin_factory;
    return nullptr;
}

extern CLAP_EXPORT const clap_plugin_entry_t clap_entry = {
    CLAP_VERSION,
    clap_entry_init,
    clap_entry_deinit,
    clap_entry_get_factory,
};

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
void* hInstance = nullptr;
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    if (fdwReason == DLL_PROCESS_ATTACH)
        hInstance = (void*)hinstDLL;
    return TRUE;
}
#endif
