#include "BleAdvertisement.h"

class BleTrafficHandler
{
    virtual void IngestTraffic(const BleAdvertisement& advertisement) = 0;
};