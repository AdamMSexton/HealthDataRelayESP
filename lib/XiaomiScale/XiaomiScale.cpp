#include <cstring>
#include <string.h>
#include <ctime>
#include <time.h>
#include <cmath>

#include "XiaomiScale.h"
#include "BleAdvertisementHandler.h"
#include "WeightMeasurement.h"
#include "OutboundWeightProcessor.h"
#include "OutboundMessage.h"


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

// Outbound weight processor
OutboundWeightProcessor outboundWeightProcessor;

void XiaomiScale::ProcessAdvertisement(const BleAdvertisement& advertisement)
{
    // CHeck advertisement payuload service data for flags indicating a new measurement
    if ((advertisement.serviceData[2] == 0x02) && ((advertisement.serviceData[3] == 0xA4)||(advertisement.serviceData[3] == 0xA6)))
    {
        // Check timestamp for new
        if (memcmp(lastMeasurementTimeStamp, &advertisement.serviceData[3],7) != 0)
        {
            memcpy(lastMeasurementTimeStamp, &advertisement.serviceData[3],7);
            printf("**NEW**");
        
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
                printf("\n");

                // Create WeightMeasurement
                WeightMeasurement measurement;
                // Weight is bytes 11 & 12, little endian divided by 200
                double decodeWeight = (double)(static_cast<uint16_t>(advertisement.serviceData[13]) | (static_cast<uint16_t>(advertisement.serviceData[14]) << 8))/200;
                measurement.weightInKg = (decodeWeight * 100.0) / 100.0;
                
                // Impedence is bytes 9 & 10, little endian
                uint16_t decodeImpedence = (static_cast<uint16_t>(advertisement.serviceData[11]) | (static_cast<uint16_t>(advertisement.serviceData[12]) << 8));
                if (decodeImpedence == 0xFFFE || decodeImpedence == 0xFFFD || decodeImpedence == 0x0000)
                {
                    measurement.impedanceOhms = 0;
                }
                else
                {
                    measurement.impedanceOhms = decodeImpedence;
                }
                
                // Decode time stamp.  UTC assumed
                struct tm timestamp = {};
                timestamp.tm_year = (advertisement.serviceData[4] | (advertisement.serviceData[5] << 8)) - 1900;
                timestamp.tm_mon  = advertisement.serviceData[6] - 1;
                timestamp.tm_mday = advertisement.serviceData[7];
                timestamp.tm_hour = advertisement.serviceData[8];
                timestamp.tm_min  = advertisement.serviceData[9];
                timestamp.tm_sec  = advertisement.serviceData[10];
                measurement.measurementTimeUTC = static_cast<uint64_t>(mktime(&timestamp));
                
                OutboundMessage outboundPayload = outboundWeightProcessor.CreateOutboundMessage(measurement);
                messageCenter.EnqueueMessage(outboundPayload);
            }
            else
            {
                printf("<null>");
            }
        }
        else
        {
            printf("D");
        }
    }
}
