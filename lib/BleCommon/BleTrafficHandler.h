#pragma once
#include "BleAdvertisement.h"

class BleTrafficHandler
{
    public:
        virtual void IngestTraffic(const BleAdvertisement& advertisement) = 0;
};