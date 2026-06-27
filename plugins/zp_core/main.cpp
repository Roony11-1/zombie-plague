#include <string.h>
#include <extdll.h>
#include <meta_api.h>

// Necesario para GetModuleHandleA y GetProcAddress
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

enginefuncs_t g_engfuncs;
globalvars_t  *gpGlobals;
meta_globals_t *gpMetaGlobals;
gamedll_funcs_t *gpGamedllFuncs;

// ------------------------------------------------------------
// Declaración mínima de la API de ReGameDLL
// ------------------------------------------------------------
struct ReGameFuncs_t 
{
    edict_t *(*CREATE_NAMED_ENTITY2)(string_t iClass);
};

class IReGameApi 
{
public:
    virtual ~IReGameApi() {}
    virtual int GetMajorVersion() = 0;
    virtual int GetMinorVersion() = 0;
    virtual const ReGameFuncs_t *GetFuncs() = 0;
};

IReGameApi *g_pReGameApi = nullptr;
const ReGameFuncs_t *g_pReGameFuncs = nullptr;

typedef void *(*CreateInterfaceFn)(const char *pName, int *pReturnCode);

bool ZP_InitReGameApi();

// ------------------------------------------------------------
// Plugin
// ------------------------------------------------------------
C_DLLEXPORT void WINAPI GiveFnptrsToDll(enginefuncs_t *pengfuncsFromEngine, globalvars_t *pGlobals)
{
    memcpy(&g_engfuncs, pengfuncsFromEngine, sizeof(enginefuncs_t));
    gpGlobals = pGlobals;
}

plugin_info_t Plugin_info = 
{
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

C_DLLEXPORT int Meta_Query(char *iv, plugin_info_t **pinfo, mutil_funcs_t *pMetaUtilFuncs) 
{
    *pinfo = &Plugin_info;
    return TRUE;
}

void OnClientPutInServer(edict_t *pEntity)
{
    if (pEntity && g_pReGameFuncs)
    {

        g_engfuncs.pfnServerPrint("[ZP] Jugador equipado con cuchillo!\n");
    }
    RETURN_META(MRES_IGNORED);
}

DLL_FUNCTIONS g_DllFunctionTable = 
{
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

C_DLLEXPORT int GetEntityAPI2(DLL_FUNCTIONS *pTable, int *iv) 
{
    memcpy(pTable, &g_DllFunctionTable, sizeof(DLL_FUNCTIONS));
    return TRUE;
}

C_DLLEXPORT int GetEntityAPI2_Post(DLL_FUNCTIONS *pTable, int *iv) 
{
    memcpy(pTable, &g_DllFunctionTable_Post, sizeof(DLL_FUNCTIONS));
    return TRUE;
}

C_DLLEXPORT int GetNewDLLFunctions(NEW_DLL_FUNCTIONS *pTable, int *iv) 
{
    memcpy(pTable, &g_NewDllFunctionTable, sizeof(NEW_DLL_FUNCTIONS));
    return TRUE;
}

C_DLLEXPORT int GetNewDLLFunctions_Post(NEW_DLL_FUNCTIONS *pTable, int *iv) 
{
    memcpy(pTable, &g_NewDllFunctionTable_Post, sizeof(NEW_DLL_FUNCTIONS));
    return TRUE;
}

META_FUNCTIONS gMetaFunctionTable = 
{
    NULL, NULL,
    GetEntityAPI2, GetEntityAPI2_Post,
    GetNewDLLFunctions, GetNewDLLFunctions_Post,
    GetEngineFunctions, GetEngineFunctions_Post
};

enginefuncs_t g_EngineFunctionsTable = { NULL };
enginefuncs_t g_EngineFunctionsTable_Post = { NULL };

C_DLLEXPORT int GetEngineFunctions(enginefuncs_t *pTable, int *iv) 
{
    memcpy(pTable, &g_EngineFunctionsTable, sizeof(enginefuncs_t));
    return TRUE;
}

C_DLLEXPORT int GetEngineFunctions_Post(enginefuncs_t *pTable, int *iv) 
{
    memcpy(pTable, &g_EngineFunctionsTable_Post, sizeof(enginefuncs_t));
    return TRUE;
}

C_DLLEXPORT int Meta_Attach(PLUG_LOADTIME now, META_FUNCTIONS *pFunctionTable,
    meta_globals_t *pMGlobals, gamedll_funcs_t *pGamedllFuncs) 
{
    gpMetaGlobals = pMGlobals;

    if (ZP_InitReGameApi())
        g_engfuncs.pfnServerPrint("ReGameDLL API inicializada correctamente.\n");
    else
        g_engfuncs.pfnServerPrint("ERROR: No se pudo obtener la API de ReGameDLL.\n");

    g_engfuncs.pfnServerPrint("zp_core attached!\n");
    memcpy(pFunctionTable, &gMetaFunctionTable, sizeof(META_FUNCTIONS));
    return TRUE;
}

// Función auxiliar que intenta obtener la API de ReGameDLL.
// Devuelve true si se obtuvo correctamente, false en caso contrario.
bool ZP_InitReGameApi()
{
    HMODULE hGameDll = GetModuleHandleA("mp.dll");
    if (!hGameDll)
        return false;

    auto createInterface = (CreateInterfaceFn)GetProcAddress(hGameDll, "CreateInterface");
    if (!createInterface)
        return false;

    g_pReGameApi = (IReGameApi *)createInterface("VRE_GAMEDLL_API_VERSION001", nullptr);
    if (!g_pReGameApi)
        return false;

    g_pReGameFuncs = g_pReGameApi->GetFuncs();
    if (!g_pReGameFuncs)
        return false;

    return true;
}

C_DLLEXPORT int Meta_Detach(PLUG_LOADTIME now, PL_UNLOAD_REASON reason) {
    return TRUE;
}