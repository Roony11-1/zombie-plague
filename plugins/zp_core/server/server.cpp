#include "server.h"
#include <extdll.h>
#include <meta_api.h>

extern enginefuncs_t g_engfuncs;

void ZP_SetupServerSettings() {
    // Forzar la gravedad a 600
    g_engfuncs.pfnCVarSetFloat("sv_gravity", 600);
    
    // Mensaje de confirmación en consola del servidor
    g_engfuncs.pfnServerPrint("[ZP] Gravedad ajustada a 600\n");
}