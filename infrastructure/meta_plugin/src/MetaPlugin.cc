#include "MetaPlugin.h"
#include <string.h>

MetaPlugin* MetaPlugin::instance = nullptr;

MetaPlugin::MetaPlugin() {}
MetaPlugin::~MetaPlugin() {}

bool MetaPlugin::Initialize(
    enginefuncs_t* eng, 
    globalvars_t* globals,
    meta_globals_t* meta, 
    gamedll_funcs_t* gameFuncs)
{
    if (!reApi.Initialize())
    {
        if (engFuncs)
            engFuncs -> pfnServerPrint("ERROR: No se pudo inicializar ReGameDLL API.\n");

        return false;
    }

    if (engFuncs)
        engFuncs -> pfnServerPrint("MetaPlugin base inicializado correctamente.\n");

    return true;
}

void MetaPlugin::Shutdown()
{

}

void MetaPlugin_Dispatch_ClientPutInServer(edict_t* entity)
{
    if (MetaPlugin::instance)
        MetaPlugin::instance -> OnClientPutInServer(entity);

    RETURN_META(MRES_IGNORED);
}