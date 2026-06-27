#include <string.h>
#include <extdll.h>
#include <meta_api.h>

enginefuncs_t g_engfuncs;
globalvars_t  *gpGlobals;

C_DLLEXPORT void WINAPI GiveFnptrsToDll(enginefuncs_t *pengfuncsFromEngine, globalvars_t *pGlobals)
{
    memcpy(&g_engfuncs, pengfuncsFromEngine, sizeof(enginefuncs_t));
    gpGlobals = pGlobals;
}

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

C_DLLEXPORT int Meta_Query(char *iv, plugin_info_t **pinfo, mutil_funcs_t *pMetaUtilFuncs) {
    *pinfo = &Plugin_info;
    return TRUE;
}

DLL_FUNCTIONS g_DllFunctionTable = { NULL };
DLL_FUNCTIONS g_DllFunctionTable_Post = { NULL };
NEW_DLL_FUNCTIONS g_NewDllFunctionTable = { NULL };
NEW_DLL_FUNCTIONS g_NewDllFunctionTable_Post = { NULL };

C_DLLEXPORT int GetEntityAPI2(DLL_FUNCTIONS *pTable, int *iv) {
    memcpy(pTable, &g_DllFunctionTable, sizeof(DLL_FUNCTIONS));
    return TRUE;
}
C_DLLEXPORT int GetEntityAPI2_Post(DLL_FUNCTIONS *pTable, int *iv) {
    memcpy(pTable, &g_DllFunctionTable_Post, sizeof(DLL_FUNCTIONS));
    return TRUE;
}
C_DLLEXPORT int GetNewDLLFunctions(NEW_DLL_FUNCTIONS *pTable, int *iv) {
    memcpy(pTable, &g_NewDllFunctionTable, sizeof(NEW_DLL_FUNCTIONS));
    return TRUE;
}
C_DLLEXPORT int GetNewDLLFunctions_Post(NEW_DLL_FUNCTIONS *pTable, int *iv) {
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

C_DLLEXPORT int GetEngineFunctions(enginefuncs_t *pTable, int *iv) {
    memcpy(pTable, &g_EngineFunctionsTable, sizeof(enginefuncs_t));
    return TRUE;
}
C_DLLEXPORT int GetEngineFunctions_Post(enginefuncs_t *pTable, int *iv) {
    memcpy(pTable, &g_EngineFunctionsTable_Post, sizeof(enginefuncs_t));
    return TRUE;
}

C_DLLEXPORT int Meta_Attach(PLUG_LOADTIME now, META_FUNCTIONS *pFunctionTable,
                            meta_globals_t *pMGlobals, gamedll_funcs_t *pGamedllFuncs) {
    g_engfuncs.pfnServerPrint("{{PLUGIN_NAME}} attached!\n");
    memcpy(pFunctionTable, &gMetaFunctionTable, sizeof(META_FUNCTIONS));
    return TRUE;
}

C_DLLEXPORT int Meta_Detach(PLUG_LOADTIME now, PL_UNLOAD_REASON reason) {
    return TRUE;
}
