#pragma once
#include "BleAdvertisementHandler.h"
#include <stdio.h>

class XiaomiScale : public BleAdvertisementHandler
{
    void ProcessAdvertisement(const BleAdvertisement& advertisement) override;

};