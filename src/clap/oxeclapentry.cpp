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
#include <cstdio>
#include <cstring>
#include <memory>
#include "oxeclap.h"

static uint32_t CLAP_ABI clap_factory_get_plugin_count(const clap_plugin_factory_t *factory)
{
    return 1;
}

static const clap_plugin_descriptor_t *CLAP_ABI clap_factory_get_plugin_descriptor(
    const clap_plugin_factory_t *factory, uint32_t index)
{
    if (index == 0)
        return &COxeClapPlugin::s_descriptor;
    return nullptr;
}

static const clap_plugin_t *CLAP_ABI clap_factory_create_plugin(
    const clap_plugin_factory_t *factory, const clap_host_t *host, const char *plugin_id)
{
    if (!host || !clap_version_is_compatible(host->clap_version))
        return nullptr;

    if (!plugin_id || strcmp(plugin_id, COxeClapPlugin::s_descriptor.id) != 0)
        return nullptr;

    COxeClapPlugin *plug = new COxeClapPlugin(host);
    return plug->clapPlugin();
}

static const clap_plugin_factory_t s_plugin_factory = {
    clap_factory_get_plugin_count,
    clap_factory_get_plugin_descriptor,
    clap_factory_create_plugin,
};

// Preset Discovery Provider
struct OxePresetProvider
{
    clap_preset_discovery_provider_t provider;
    const clap_preset_discovery_indexer_t *indexer;
};

static const clap_preset_discovery_provider_descriptor_t s_preset_provider_desc = {
    CLAP_VERSION,
    "com.oxesoft.oxefmsynth",
    "Oxe FM Synth",
    "Oxe Music Software",
};

static bool CLAP_ABI preset_provider_init(const clap_preset_discovery_provider_t *provider)
{
    OxePresetProvider *prov = (OxePresetProvider *)provider->provider_data;
    if (!prov || !prov->indexer || !prov->indexer->declare_location)
        return false;

    clap_preset_discovery_location_t loc;
    memset(&loc, 0, sizeof(loc));
    loc.flags = CLAP_PRESET_DISCOVERY_IS_FACTORY_CONTENT;
    loc.name = "Factory Presets";
    loc.kind = CLAP_PRESET_DISCOVERY_LOCATION_PLUGIN;
    loc.location = nullptr;

    return prov->indexer->declare_location(prov->indexer, &loc);
}

static void CLAP_ABI preset_provider_destroy(const clap_preset_discovery_provider_t *provider)
{
    if (!provider)
        return;
    OxePresetProvider *prov = (OxePresetProvider *)provider->provider_data;
    delete prov;
}

static bool CLAP_ABI preset_provider_get_metadata(
    const clap_preset_discovery_provider_t *provider,
    uint32_t location_kind,
    const char *location,
    const clap_preset_discovery_metadata_receiver_t *metadata_receiver)
{
    if (location_kind != CLAP_PRESET_DISCOVERY_LOCATION_PLUGIN || !metadata_receiver || !metadata_receiver->begin_preset)
        return false;

    std::unique_ptr<CPrograms> programs(new CPrograms());
    programs->Init();

    clap_universal_plugin_id_t plugin_id = {
        "clap",
        COxeClapPlugin::s_descriptor.id,
    };

    for (int i = 0; i < MAX_PROGRAMS; ++i)
    {
        char pName[PG_NAME_SIZE + 1] = {0};
        programs->GetProgName(pName, i);
        if (pName[0] == '\0')
        {
            snprintf(pName, sizeof(pName), "Program %d", i);
        }

        char loadKey[16] = {0};
        snprintf(loadKey, sizeof(loadKey), "%d", i);

        if (!metadata_receiver->begin_preset(metadata_receiver, pName, loadKey))
            return true;

        if (metadata_receiver->add_plugin_id)
            metadata_receiver->add_plugin_id(metadata_receiver, &plugin_id);

        if (metadata_receiver->set_flags)
            metadata_receiver->set_flags(metadata_receiver, CLAP_PRESET_DISCOVERY_IS_FACTORY_CONTENT);
    }

    return true;
}

static const void *CLAP_ABI preset_provider_get_extension(
    const clap_preset_discovery_provider_t *provider,
    const char *extension_id)
{
    return nullptr;
}

static uint32_t CLAP_ABI preset_discovery_count(const clap_preset_discovery_factory_t *factory)
{
    return 1;
}

static const clap_preset_discovery_provider_descriptor_t *CLAP_ABI preset_discovery_get_descriptor(
    const clap_preset_discovery_factory_t *factory, uint32_t index)
{
    if (index == 0)
        return &s_preset_provider_desc;
    return nullptr;
}

static const clap_preset_discovery_provider_t *CLAP_ABI preset_discovery_create(
    const clap_preset_discovery_factory_t *factory,
    const clap_preset_discovery_indexer_t *indexer,
    const char *provider_id)
{
    if (!indexer || !clap_version_is_compatible(indexer->clap_version))
        return nullptr;

    if (!provider_id || strcmp(provider_id, s_preset_provider_desc.id) != 0)
        return nullptr;

    OxePresetProvider *prov = new OxePresetProvider();
    memset(prov, 0, sizeof(*prov));
    prov->indexer = indexer;
    prov->provider.desc = &s_preset_provider_desc;
    prov->provider.provider_data = prov;
    prov->provider.init = preset_provider_init;
    prov->provider.destroy = preset_provider_destroy;
    prov->provider.get_metadata = preset_provider_get_metadata;
    prov->provider.get_extension = preset_provider_get_extension;
    return &prov->provider;
}

static const clap_preset_discovery_factory_t s_preset_discovery_factory = {
    preset_discovery_count,
    preset_discovery_get_descriptor,
    preset_discovery_create,
};

static bool CLAP_ABI clap_entry_init(const char *plugin_path)
{
    return true;
}

static void CLAP_ABI clap_entry_deinit(void)
{
}

static const void *CLAP_ABI clap_entry_get_factory(const char *factory_id)
{
    if (!factory_id)
        return nullptr;
    if (strcmp(factory_id, CLAP_PLUGIN_FACTORY_ID) == 0)
        return &s_plugin_factory;
    if (strcmp(factory_id, CLAP_PRESET_DISCOVERY_FACTORY_ID) == 0 ||
        strcmp(factory_id, CLAP_PRESET_DISCOVERY_FACTORY_ID_COMPAT) == 0)
        return &s_preset_discovery_factory;
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
