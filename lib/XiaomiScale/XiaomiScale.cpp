#include "XiaomiScale.h"
#include "BleTrafficHandler.h"


void XiaomiScale::ProcessAdvertisement(const BleAdvertisement& advertisement)
{
    printf("BLE | MAC %02X:%02X:%02X:%02X:%02X:%02X | RSSI %d | UUID 0x%02X%02X | Data[%u]: ",
        advertisement.address[5],
        advertisement.address[4],
        advertisement.address[3],
        advertisement.address[2],
        advertisement.address[1],
        advertisement.address[0],
        advertisement.rssi,
        advertisement.serviceUuid.value[1],
        advertisement.serviceUuid.value[0],
        advertisement.serviceDataLength);

    if (advertisement.serviceData != nullptr)
    {
        for (uint16_t i = 0; i < advertisement.serviceDataLength; i++)
        {
            printf("%02X ", advertisement.serviceData[i]);
        }
    }
    else
    {
        printf("<null>");
    }

    printf("\n");
}
