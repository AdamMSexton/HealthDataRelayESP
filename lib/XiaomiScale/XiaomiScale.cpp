#include "XiaomiScale.h"
#include "BleAdvertisementHandler.h"
#include <cstring>
#include <string.h>


// Xiaomi Mi Body Composition Scale 2 (XMTZC05HM) - 0x181B payload
// serviceData points AFTER the 2-byte BLE service UUID (1B 18).
//
// [0-1]   Flags/state
//          02 04 = weighing/settling
//          02 24 = stable, impedance phase
//          02 26 = valid impedance obtained
//          02 A4 = final weight, no impedance
//          02 A6 = final weight + impedance
//
// [2-3]   Year, uint16 little-endian
// [4]     Month
// [5]     Day
// [6]     Hour
// [7]     Minute
// [8]     Second
//
// [9-10]  Impedance, uint16 little-endian (ohms)
//          0x0000 = no value yet
//          0xFFFE = pending/unavailable
//          0xFFFD = failed
//
// [11-12] Weight, uint16 little-endian
//          kg = raw / 200.0
//
// Final records:
//   A4 -> accept weight, impedance = null
//   A6 -> accept weight + impedance

// Hold last valid timestamp
uint8_t XiaomiScale::lastMeasurementTimeStamp[7] = {};

void XiaomiScale::ProcessAdvertisement(const BleAdvertisement& advertisement)
{
    // CHeck advertisement payuload service data for flags indicating a new measurement
    if ((advertisement.serviceData[2] == 0x02) && ((advertisement.serviceData[3] == 0xA4)||(advertisement.serviceData[3] == 0xA6)))
    {
        // Check timestamp for new
        if (memcmp(lastMeasurementTimeStamp, &advertisement.serviceData[2],7) != 0)
        {
            memcpy(lastMeasurementTimeStamp, &advertisement.serviceData[2],7);
            printf("**NEW**");
        }
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
}
