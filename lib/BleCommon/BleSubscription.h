#pragma once
#include "BleTrafficHandler.h"
#include "BleUuid.h"


struct BleSubscription
{
    BleUuid serviceUuid;
    BleAdvertisementHandler* handler;

};