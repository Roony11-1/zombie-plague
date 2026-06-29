#pragma once

#include "IReGameApi.h"

bool ZP_InitReGameApi();

extern IReGameApi *g_pReGameApi;
extern const ReGameFuncs_t *g_pReGameFuncs;