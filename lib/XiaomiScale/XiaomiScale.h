#pragma once
#include "BleAdvertisementHandler.h"
#include <stdio.h>
#include "MessageCenter.h"

class XiaomiScale : public BleAdvertisementHandler
{
    private:
        static uint8_t lastMeasurementTimeStamp[7];

    public:
        void ProcessAdvertisement(const BleAdvertisement& advertisement) override;

};