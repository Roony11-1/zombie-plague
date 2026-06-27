#include <string.h>
#include <extdll.h>
#include <meta_api.h>
#include <cbase.h>
#include <player.h>
#include <util.h>
#include <weapons.h>

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

// -----------------------------------------------
// Utilidades para manipular jugadores
// -----------------------------------------------

void RemoveAllPlayerItems(CBasePlayer *pPlayer)
{
    for (int i = 0; i < MAX_ITEM_TYPES; i++)
    {
        CBasePlayerItem *pItem = pPlayer->m_rgpPlayerItems[i];
        while (pItem)
        {
            CBasePlayerItem *pNext = pItem->m_pNext;
            pPlayer->RemovePlayerItem(pItem);
            pItem = pNext;
        }
    }
}

void UTIL_ClientPrintAll(PRINT_TYPE msg_dest, const char *msg)
{
    for (int i = 1; i <= gpGlobals->maxClients; i++)
    {
        edict_t *pEdict = INDEXENT(i);
        if (pEdict && !pEdict->free)
        {
            g_engfuncs.pfnClientPrintf(pEdict, msg_dest, msg);
        }
    }
}

void GiveWeaponToPlayer(CBasePlayer *pPlayer, const char *weaponName)
{
    edict_t *pWeaponEdict = g_engfuncs.pfnCreateNamedEntity(MAKE_STRING(weaponName));
    if (!pWeaponEdict) return;
    
    CBaseEntity *pWeaponEntity = CBaseEntity::Instance(pWeaponEdict);
    if (!pWeaponEntity) return;
    
    pWeaponEntity->Spawn();
    
    CBasePlayerItem *pItem = (CBasePlayerItem *)pWeaponEntity;
    pPlayer->AddPlayerItem(pItem);
}

// -----------------------------------------------
// Hook: jugador entra al servidor
// -----------------------------------------------
void OnClientPutInServer(edict_t *pEntity)
{
    if (pEntity)
    {
        CBasePlayer *pPlayer = (CBasePlayer *)CBaseEntity::Instance(pEntity);
        if (pPlayer && pPlayer->IsAlive())
        {
            pPlayer->pev->health = 2000;
            pPlayer->pev->max_health = 2000;

            RemoveAllPlayerItems(pPlayer);
            GiveWeaponToPlayer(pPlayer, "weapon_knife");

            UTIL_ClientPrintAll(print_chat, "[ZP] Un zombie ha llegado!\n");
        }
    }
    RETURN_META(MRES_IGNORED);
}

// -----------------------------------------------
// Tablas de funciones (38 miembros)
// -----------------------------------------------
DLL_FUNCTIONS g_DllFunctionTable = {
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, OnClientPutInServer, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL
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
    g_engfuncs.pfnServerPrint("{{PLUGIN_NAME}} attached!\n");
    memcpy(pFunctionTable, &gMetaFunctionTable, sizeof(META_FUNCTIONS));
    return TRUE;
}

C_DLLEXPORT int Meta_Detach(PLUG_LOADTIME now, PL_UNLOAD_REASON reason) {
    return TRUE;
}