#pragma once
#include "BleAdvertisement.h"

class BleAdvertisementHandler
{
    public:
        virtual void ProcessAdvertisement(const BleAdvertisement& advertisement) = 0;
};