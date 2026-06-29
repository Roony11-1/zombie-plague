#pragma once

#include <extdll.h>

class CBasePlayer;
class CBasePlayerItem;

struct ReGameFuncs_t 
{
    edict_t *(*CREATE_NAMED_ENTITY2)(string_t iClass);
};

struct WeaponInfoStruct 
{
    int iSlot;
    int iPosition;
    const char *pszAmmo1;
    int iMaxAmmo1;
    const char *pszAmmo2;
    int iMaxAmmo2;
    const char *pszName;
    int iMaxClip;
    int iId;
    int iFlags;
    int iWeight;
};

class IReGameApi 
{
    public:
        virtual ~IReGameApi() {}
        virtual int GetMajorVersion() = 0;
        virtual int GetMinorVersion() = 0;
        virtual const ReGameFuncs_t *GetFuncs() = 0;

        virtual WeaponInfoStruct *GetWeaponInfo(int weaponID) = 0;
        virtual WeaponInfoStruct *GetWeaponInfo(const char *weaponName) = 0;
};

extern IReGameApi *g_pReGameApi;
extern const ReGameFuncs_t *g_pReGameFuncs;

bool ZP_InitReGameApi();