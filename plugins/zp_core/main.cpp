#include <string.h>
#include <extdll.h>
#include <meta_api.h>
#include "reapi.h"
#include <enginecallback.h>

enginefuncs_t g_engfuncs;
globalvars_t  *gpGlobals;
meta_globals_t *gpMetaGlobals;
gamedll_funcs_t *gpGamedllFuncs;


C_DLLEXPORT void WINAPI GiveFnptrsToDll(enginefuncs_t *pengfuncsFromEngine, globalvars_t *pGlobals) {
    memcpy(&g_engfuncs, pengfuncsFromEngine, sizeof(enginefuncs_t));
    gpGlobals = pGlobals;
}

plugin_info_t Plugin_info = {
    META_INTERFACE_VERSION,
    "zp_core",
    "0.1",
    __DATE__,
    "Roony11-1",
    "https://github.com/Roony11-1",
    "ZP_CORE",
    PT_ANYTIME,
    PT_ANYTIME
};

C_DLLEXPORT int Meta_Query(char *iv, plugin_info_t **pinfo, mutil_funcs_t *pMetaUtilFuncs) {
    *pinfo = &Plugin_info;
    return TRUE;
}

void OnClientPutInServer(edict_t *pEntity) {
    if (pEntity && g_pReGameFuncs) {
        //g_engfuncs.pfnServerPrint("[ZP] Jugador equipado con cuchillo!\n");
    }
    RETURN_META(MRES_IGNORED);
}

DLL_FUNCTIONS g_DllFunctionTable = {
    NULL,                               // pfnGameInit              [0]
    NULL,                               // pfnSpawn                 [1]
    NULL,                               // pfnThink                 [2]
    NULL,                               // pfnUse                   [3]
    NULL,                               // pfnTouch                 [4]
    NULL,                               // pfnBlocked               [5]
    NULL,                               // pfnKeyValue              [6]
    NULL,                               // pfnSave                  [7]
    NULL,                               // pfnRestore               [8]
    NULL,                               // pfnSetAbsBox             [9]
    NULL,                               // pfnSaveWriteFields       [10]
    NULL,                               // pfnSaveReadFields        [11]
    NULL,                               // pfnSaveGlobalState       [12]
    NULL,                               // pfnRestoreGlobalState    [13]
    NULL,                               // pfnResetGlobalState      [14]
    NULL,                               // pfnClientConnect         [15]
    NULL,                               // pfnClientDisconnect      [16]
    NULL,                               // pfnClientKill            [17]
    OnClientPutInServer,                // pfnClientPutInServer     [18]
    NULL,                    // pfnClientCommand         [19]
    NULL,                               // pfnClientUserInfoChanged [20]
    NULL,                               // pfnServerActivate        [21]
    NULL,                               // pfnServerDeactivate      [22]
    NULL,                               // pfnPlayerPreThink        [23]
    NULL,                               // pfnPlayerPostThink       [24]
    NULL,                               // pfnStartFrame            [25]
    NULL,                               // pfnParmsNewLevel         [26]
    NULL,                               // pfnParmsChangeLevel      [27]
    NULL,                               // pfnGetGameDescription    [28]
    NULL,                               // pfnPlayerCustomization   [29]
    NULL,                               // pfnSpectatorConnect      [30]
    NULL,                               // pfnSpectatorDisconnect   [31]
    NULL,                               // pfnSpectatorThink        [32]
    NULL,                               // pfnSys_Error             [33]
    NULL,                               // pfnPM_Move               [34]
    NULL,                               // pfnPM_Init               [35]
    NULL,                               // pfnPM_FindTextureType    [36]
    NULL,                               // pfnSetupVisibility       [37]
    NULL,                               // pfnUpdateClientData      [38]
    NULL,                               // pfnAddToFullPack         [39]
    NULL,                               // pfnCreateBaseline        [40]
    NULL,                               // pfnRegisterEncoders      [41]
    NULL,                               // pfnGetWeaponData         [42]
    NULL,                               // pfnCmdStart              [43]
    NULL,                               // pfnCmdEnd                [44]
    NULL,                               // pfnConnectionlessPacket  [45]
    NULL,                               // pfnGetHullBounds         [46]
    NULL,                               // pfnCreateInstancedBaselines [47]
    NULL,                               // pfnInconsistentFile      [48]
    NULL                                // pfnAllowLagCompensation  [49]
};

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
    gpMetaGlobals = pMGlobals;
    gpGamedllFuncs = pGamedllFuncs;

    if (ZP_InitReGameApi())
        g_engfuncs.pfnServerPrint("ReGameDLL API inicializada correctamente.\n");
    else
        g_engfuncs.pfnServerPrint("ERROR: No se pudo obtener la API de ReGameDLL.\n");

    g_engfuncs.pfnServerPrint("zp_core attached!\n");
    memcpy(pFunctionTable, &gMetaFunctionTable, sizeof(META_FUNCTIONS));
    return TRUE;
}

C_DLLEXPORT int Meta_Detach(PLUG_LOADTIME now, PL_UNLOAD_REASON reason) {
    return TRUE;
}