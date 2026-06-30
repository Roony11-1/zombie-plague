#include <string.h>
#include <extdll.h>
#include <meta_api.h>
#include <MetaPlugin.h>   // Infraestructura central

// ------------------------------------------------------------
//  Variables globales del engine (necesarias para inicializar
//  MetaPlugin). Se pasan a MetaPlugin::Initialize()
// ------------------------------------------------------------
enginefuncs_t g_engfuncs;
globalvars_t  *gpGlobals;
meta_globals_t *gpMetaGlobals;
gamedll_funcs_t *gpGamedllFuncs;

// ------------------------------------------------------------
//  Información del plugin (placeholders reemplazables)
// ------------------------------------------------------------
plugin_info_t Plugin_info = {
    META_INTERFACE_VERSION,
    "{{PLUGIN_NAME}}",
    "{{VERSION}}",
    __DATE__,
    "{{AUTHOR}}",
    "{{URL}}",
    "{{LOG_TAG}}",
    PT_ANYTIME,
    PT_ANYTIME
};

// ------------------------------------------------------------
//  Tu plugin concreto (hereda de MetaPlugin)
// ------------------------------------------------------------
class {{PLUGIN_CLASS}} : public MetaPlugin {
public:
    void OnClientPutInServer(edict_t* pEntity) override {
        // Lógica específica de tu plugin
        if (pEntity && IsReApiOk()) {
            // ...
        }
    }
    // Aquí puedes sobrescribir otros hooks virtuales
};

// ------------------------------------------------------------
//  Tablas de funciones del juego (se rellenan en InitTables)
// ------------------------------------------------------------
DLL_FUNCTIONS g_DllFunctionTable;
DLL_FUNCTIONS g_DllFunctionTable_Post;
NEW_DLL_FUNCTIONS g_NewDllFunctionTable;
NEW_DLL_FUNCTIONS g_NewDllFunctionTable_Post;

// Inicializa las tablas a NULL y asigna los hooks que uses
static void InitDllFunctionTables() {
    memset(&g_DllFunctionTable, 0, sizeof(g_DllFunctionTable));
    memset(&g_DllFunctionTable_Post, 0, sizeof(g_DllFunctionTable_Post));
    memset(&g_NewDllFunctionTable, 0, sizeof(g_NewDllFunctionTable));
    memset(&g_NewDllFunctionTable_Post, 0, sizeof(g_NewDllFunctionTable_Post));

    // --- Registrar hooks aquí ---
    g_DllFunctionTable.pfnClientPutInServer = MetaPlugin_Dispatch_ClientPutInServer;

    // Ejemplo: cuando añadas más hooks
    // g_DllFunctionTable.pfnStartFrame = MetaPlugin_Dispatch_StartFrame;
}

// ------------------------------------------------------------
//  Funciones de exportación obligatorias (Metamod)
// ------------------------------------------------------------
C_DLLEXPORT void WINAPI GiveFnptrsToDll(enginefuncs_t* pengfuncsFromEngine, globalvars_t* pGlobals) {
    memcpy(&g_engfuncs, pengfuncsFromEngine, sizeof(enginefuncs_t));
    gpGlobals = pGlobals;
}

C_DLLEXPORT int Meta_Query(char* iv, plugin_info_t** pinfo, mutil_funcs_t* pMetaUtilFuncs) {
    *pinfo = &Plugin_info;
    return TRUE;
}

C_DLLEXPORT int GetEntityAPI2(DLL_FUNCTIONS* pTable, int* iv) {
    memcpy(pTable, &g_DllFunctionTable, sizeof(DLL_FUNCTIONS));
    return TRUE;
}
C_DLLEXPORT int GetEntityAPI2_Post(DLL_FUNCTIONS* pTable, int* iv) {
    memcpy(pTable, &g_DllFunctionTable_Post, sizeof(DLL_FUNCTIONS));
    return TRUE;
}
C_DLLEXPORT int GetNewDLLFunctions(NEW_DLL_FUNCTIONS* pTable, int* iv) {
    memcpy(pTable, &g_NewDllFunctionTable, sizeof(NEW_DLL_FUNCTIONS));
    return TRUE;
}
C_DLLEXPORT int GetNewDLLFunctions_Post(NEW_DLL_FUNCTIONS* pTable, int* iv) {
    memcpy(pTable, &g_NewDllFunctionTable_Post, sizeof(NEW_DLL_FUNCTIONS));
    return TRUE;
}

META_FUNCTIONS gMetaFunctionTable = {
    NULL, NULL,
    GetEntityAPI2, GetEntityAPI2_Post,
    GetNewDLLFunctions, GetNewDLLFunctions_Post,
    GetEngineFunctions, GetEngineFunctions_Post
};

enginefuncs_t g_EngineFunctionsTable = { NULL };
enginefuncs_t g_EngineFunctionsTable_Post = { NULL };

C_DLLEXPORT int GetEngineFunctions(enginefuncs_t* pTable, int* iv) {
    memcpy(pTable, &g_EngineFunctionsTable, sizeof(enginefuncs_t));
    return TRUE;
}
C_DLLEXPORT int GetEngineFunctions_Post(enginefuncs_t* pTable, int* iv) {
    memcpy(pTable, &g_EngineFunctionsTable_Post, sizeof(enginefuncs_t));
    return TRUE;
}

// ------------------------------------------------------------
//  Meta_Attach / Meta_Detach (usan MetaPlugin)
// ------------------------------------------------------------
C_DLLEXPORT int Meta_Attach(PLUG_LOADTIME now, META_FUNCTIONS* pFunctionTable,
                            meta_globals_t* pMGlobals, gamedll_funcs_t* pGamedllFuncs) {
    if (MetaPlugin::instance) return FALSE;

    gpMetaGlobals = pMGlobals;
    gpGamedllFuncs = pGamedllFuncs;

    // Inicializar tablas de hooks antes de copiarlas
    InitDllFunctionTables();

    MetaPlugin::instance = new {{PLUGIN_CLASS}}();

    if (!MetaPlugin::instance->Initialize(&g_engfuncs, gpGlobals, pMGlobals, pGamedllFuncs)) {
        g_engfuncs.pfnServerPrint("ERROR: No se pudo inicializar {{PLUGIN_NAME}}.\n");
        delete MetaPlugin::instance;
        MetaPlugin::instance = nullptr;
        return FALSE;
    }

    g_engfuncs.pfnServerPrint("{{PLUGIN_NAME}} attached!\n");
    memcpy(pFunctionTable, &gMetaFunctionTable, sizeof(META_FUNCTIONS));
    return TRUE;
}

C_DLLEXPORT int Meta_Detach(PLUG_LOADTIME now, PL_UNLOAD_REASON reason) {
    if (MetaPlugin::instance) {
        MetaPlugin::instance->Shutdown();
        delete MetaPlugin::instance;
        MetaPlugin::instance = nullptr;
    }
    return TRUE;
}