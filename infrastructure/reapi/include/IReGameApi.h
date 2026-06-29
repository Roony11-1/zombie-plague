#pragma once

#include <extdll.h>

class CBasePlayer;
class CBasePlayerItem;

struct ReGameFunctions
{
    edict_t *(*CREATE_NAMED_ENTITY2)(string_t iClass);
};

class IReGameApi
{
public:
    virtual ~IReGameApi() {}

    virtual int GetMajorVersion() = 0;
    virtual int GetMinorVersion() = 0;
    virtual const ReGameFunctions *GetFuncs() = 0;
};