#include "reapi.h"

#ifdef _WIN32
#include <windows.h>
#endif

IReGameApi *g_pReGameApi = nullptr;
const ReGameFuncs_t *g_pReGameFuncs = nullptr;

typedef void *(*CreateInterfaceFn)(const char *pName, int *pReturnCode);

bool ZP_InitReGameApi() 
{
    HMODULE hGameDll = GetModuleHandleA("mp.dll");
    if (!hGameDll)
        return false;

    auto ci = (CreateInterfaceFn)GetProcAddress(hGameDll, "CreateInterface");
    if (!ci)
        return false;

    g_pReGameApi = (IReGameApi *)ci("VRE_GAMEDLL_API_VERSION001", nullptr);
    if (!g_pReGameApi)
        return false;

    g_pReGameFuncs = g_pReGameApi->GetFuncs();
    return (g_pReGameFuncs != nullptr);
}