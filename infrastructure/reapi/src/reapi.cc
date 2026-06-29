#include "ReApi.h"

#ifdef _WIN32
#include <windows.h>
#endif

using CreateInterfaceFunction = void* (*)(const char*, int*);

bool ReApi::Initialize()
{
#ifdef _WIN32
    HMODULE gameModule = GetModuleHandleA("mp.dll");
    if (!gameModule)
    {
        return false;
    }

    auto createInterface = reinterpret_cast<CreateInterfaceFunction>(
        GetProcAddress(gameModule, "CreateInterface"));

    if (!createInterface)
    {
        return false;
    }

    api = static_cast<IReGameApi*>(
        createInterface("VRE_GAMEDLL_API_VERSION001", nullptr));

    if (!api)
    {
        return false;
    }

    functions = api->GetFuncs();

    return functions != nullptr;
#else
    return false;
#endif
}

IReGameApi& ReApi::Api() const
{
    return *api;
}

const ReGameFunctions& ReApi::Functions() const
{
    return *functions;
}

bool ReApi::IsInitialized() const 
{
    return (api != nullptr) && (functions != nullptr);
}