#pragma once

#include <extdll.h>

class CBasePlayer;
class CBasePlayerItem;

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

extern IReGameApi *g_pReGameApi;
extern const ReGameFuncs_t *g_pReGameFuncs;

bool ZP_InitReGameApi();