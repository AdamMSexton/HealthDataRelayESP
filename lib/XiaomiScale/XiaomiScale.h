#pragma once
#include "BleAdvertisementHandler.h"
#include <stdio.h>
#include "MessageCenter.h"

#include "BleManager.h"

extern BleManager bleManager;

class XiaomiScale : public BleAdvertisementHandler
{
    private:
        static uint8_t lastMeasurementTimeStamp[7];
        static uint64_t nextClockSyncAllowed;
        static uint8_t clockCorrectionCountdown;
        static ble_addr_t correctionAddress;

        static uint64_t DecodeTimestampUTC(const uint8_t* serviceData);
    public:
        void ProcessAdvertisement(const BleAdvertisement& advertisement) override;

};