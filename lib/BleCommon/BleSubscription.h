#pragma once
#include "BleAdvertisementHandler.h"
#include "BleUuid.h"


struct BleSubscription
{
    BleUuid serviceUuid;
    BleAdvertisementHandler* handler;

};