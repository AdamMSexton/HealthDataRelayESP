#include "BleTrafficHandler.h"
#include "BleUuid.h"


struct BleSubscription
{
    BleUuid serviceUuid;
    BleTrafficHandler* handler;

};