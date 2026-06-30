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

    public:
        struct HookDescriptor
        {
            size_t offset;
            void* dispatchFunc;
            HookDescriptor* next;
        };

        static HookDescriptor*& GetHookList()
        {
            static HookDescriptor* head = nullptr;

            return head;
        }

        static void RegisterHook(size_t offset, void* func)
        {
            auto* desc = new HookDescriptor(offset, func, GetHookList());

            GetHookList() = desc;
        }

        static void ApplyHooks(DLL_FUNCTIONS& table) 
        {
            for (auto* desc = GetHookList(); desc; desc = desc->next) 
            {
                void** slot = reinterpret_cast<void**>(
                    reinterpret_cast<char*>(&table) + desc -> offset
                );

                *slot = desc -> dispatchFunc;
            }
        }
};

// Macro mágica: declara la función dispatch, registra el hook y llama al método virtual
#define META_HOOK(method) \
    private: \
    static void dispatch_##method(edict_t* pEntity) { \
        if (MetaPlugin::instance) \
            MetaPlugin::instance->method(pEntity); \
        RETURN_META(MRES_IGNORED); \
    } \
    static const bool registered_##method; \
    public: \
    void method(edict_t* pEntity) override

// En el .cpp se inicializan las variables estáticas de registro
// (esto se hace en el archivo de implementación del plugin)
#define META_HOOK_IMPL(method) \
    const bool MetaPlugin::registered_##method = []() -> bool { \
        MetaPlugin::RegisterHook(offsetof(DLL_FUNCTIONS, pfn##method), (void*)&MetaPlugin::dispatch_##method); \
        return true; \
    }()