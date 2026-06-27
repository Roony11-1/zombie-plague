#include "weapons.h"
#include "../reapi/reapi.h"
#include <extdll.h>
#include <meta_api.h>

extern enginefuncs_t g_engfuncs;

void ZP_GiveWeapon(edict_t *pPlayerEdict, const char *weaponName) {
    if (!pPlayerEdict || !g_pReGameFuncs) return;

    // Crear la entidad del arma
    edict_t *pWeapon = g_pReGameFuncs->CREATE_NAMED_ENTITY2(MAKE_STRING(weaponName));
    if (pWeapon) {
        // Activar el arma (llama a Spawn internamente)
        MDLL_Spawn(pWeapon);
        // Entregársela al jugador (llama a Touch)
        MDLL_Touch(pWeapon, pPlayerEdict);
        
        char buf[128];
        sprintf(buf, "[ZP] %s entregada al jugador %d\n", weaponName, ENTINDEX(pPlayerEdict));
        g_engfuncs.pfnServerPrint(buf);
    } else {
        char buf[128];
        sprintf(buf, "[ZP] ERROR: no se pudo crear %s\n", weaponName);
        g_engfuncs.pfnServerPrint(buf);
    }
}