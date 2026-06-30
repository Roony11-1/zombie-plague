#pragma once

#include "ReApi.h"
#include <extdll.h>
#include <meta_api.h>

class MetaPlugin
{
    public:
        static MetaPlugin* instance;

        MetaPlugin();
        virtual ~MetaPlugin();

        bool Initialize(
            enginefuncs_t* eng,
            globalvars_t* globals,
            meta_globals_t* meta,
            gamedll_funcs_t* gameFuncs);

        void Shutdown();

        ReApi& GetReApi() { return reApi; }
        bool IsReApiOk() const { return reApi.IsInitialized(); }

        // Hooks
        virtual void OnClientPutInServer(edict_t* entity) {}

    protected:
        ReApi reApi;
        enginefuncs_t* engFuncs = nullptr;
        globalvars_t* globalVars = nullptr;
        meta_globals_t* globalMeta = nullptr;
        gamedll_funcs_t* gameFuncs = nullptr;
};

void MetaPlugin_Dispatch_ClientPutInServer(edict_t* pEntity);