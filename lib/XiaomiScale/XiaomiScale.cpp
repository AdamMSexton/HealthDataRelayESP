#include "XiaomiScale.h"
#include "BleTrafficHandler.h"

class XiaomiScale : public BleTrafficHandler
{
public:
    void XiaomiScale::IngestTraffic(const BleAdvertisement& advertisement) override
    {
        // Xiaomi knows what to do.
    }
};