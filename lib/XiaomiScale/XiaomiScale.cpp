#include "XiaomiScale.h"
#include "BleTrafficHandler.h"


void XiaomiScale::IngestTraffic(const BleAdvertisement& advertisement)
{

    printf("Msg: %02X %d %02X\n",
        advertisement.address[0],
        static_cast<int>(advertisement.serviceUuid.size),
        advertisement.serviceUuid.value[0]);
}
