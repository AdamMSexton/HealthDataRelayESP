#pragma once
#include "BleTrafficHandler.h"
#include <stdio.h>

class XiaomiScale : public BleTrafficHandler
{
    void IngestTraffic(const BleAdvertisement& advertisement) override;

};