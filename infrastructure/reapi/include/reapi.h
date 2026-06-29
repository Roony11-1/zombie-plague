#pragma once

#include "IReGameApi.h"

class ReApi
{
    public:
        bool Initialize();
        bool IsInitialized() const;

        const ReGameFunctions& Functions() const;
        IReGameApi& Api() const;

    private:
        IReGameApi* api{};
        const ReGameFunctions* functions{};
};