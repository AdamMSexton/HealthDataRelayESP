#pragma once
#include "BleAdvertisementHandler.h"
#include <stdio.h>

class XiaomiScale : public BleAdvertisementHandler
{
    private:
        static uint16_t lastMeasurement[7];

    public:
        void ProcessAdvertisement(const BleAdvertisement& advertisement) override;

};