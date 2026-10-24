#pragma once
#include "BleAdvertisementHandler.h"
#include <stdio.h>
#include "MessageCenter.h"

class XiaomiScale : public BleAdvertisementHandler
{
    private:
        static uint8_t lastMeasurementTimeStamp[7];
        static uint64_t nextClockSyncAllowed;
        
        static uint64_t DecodeTimestampUTC(const uint8_t* serviceData);
    public:
        void ProcessAdvertisement(const BleAdvertisement& advertisement) override;

};