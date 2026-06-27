#pragma once
#include <extdll.h>

// Entrega un arma por nombre de clase (ej. "weapon_usp", "weapon_m4a1")
void ZP_GiveWeapon(edict_t *pPlayerEdict, const char *weaponName);