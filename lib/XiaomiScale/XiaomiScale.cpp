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

uint64_t XiaomiScale::nextClockSyncAllowed = 0;     // Uses systems monolithic/uptime clock as opposed to clock clock
uint8_t XiaomiScale::clockCorrectionCountdown = 0;  // Repeat message counter to ensure clock correction triggered after the new measurement is received
ble_addr_t XiaomiScale::correctionAddress = {};

void XiaomiScale::ProcessAdvertisement(const BleAdvertisement& advertisement)
{
    // Check advertisement payload service data for flags indicating a new measurement
    if ((advertisement.serviceData[2] == 0x02) && ((advertisement.serviceData[3] == 0xA4)||(advertisement.serviceData[3] == 0xA6)))
    {
        // Check timestamp for new
        if (memcmp(lastMeasurementTimeStamp, &advertisement.serviceData[3],7) != 0)
        {
            memcpy(lastMeasurementTimeStamp, &advertisement.serviceData[3],7);
                
            if (advertisement.serviceData != nullptr)
            {
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
                if (advertisement.serviceDataLength >= 11)
                {
                    measurement.measurementTimeUTC = DecodeTimestampUTC(advertisement.serviceData);
                }
                else
                {
                    // Figure something out later
                    printf("Malformed Time Advertisement packet. Do something");
                }
               
                outboundWeightProcessor.SendOutboundMessage(measurement);
            }
            else
            {
                printf("<null>");
            }
        }
        else
        {
            if (clockCorrectionCountdown > 0)
            {
                clockCorrectionCountdown--;
                if (clockCorrectionCountdown == 0)
                {
                    printf("Clock correction triggered.");
                    bleManager.SendTimeCorrectionNow(correctionAddress);
                    correctionAddress = {};
                }
            }
        }
    }
    else if ((advertisement.serviceData[2] == 0x02) && (advertisement.serviceData[3] == 0x04))  // Clock currently weighing so we know this is a current time stamp from scale
    {
        constexpr int8_t MAX_CLOCK_DRIFT_SECONDS = 5;

        uint64_t currentUpTime = esp_timer_get_time();
        if (currentUpTime >= nextClockSyncAllowed)
        {
            int64_t scaleTime = 0;
            if (advertisement.serviceDataLength >= 11)
            {
                scaleTime = DecodeTimestampUTC(advertisement.serviceData);
            }
            else
            {
                // Figure something out later
                printf("Malformed Time Advertisement packet. Do something");
            }

            int64_t scaleDrift = abs(static_cast<int64_t>(time(nullptr)) - scaleTime);
            int64_t recheckOffset = 0;      // Used to adjust nextCLockSync to verify a sync

            if (scaleDrift > MAX_CLOCK_DRIFT_SECONDS)
            {
                if (timeManager.IsTimeSynchronized())
                {
                    // Send update to scale
                    printf("Scale off by %lld seconds", scaleDrift);
                    // Store Bt address for clock correction
                    correctionAddress.type = advertisement.addressType;
                    memcpy(correctionAddress.val, advertisement.address, 6);
                    // Setup clock correction to fire after a 3rd repeated completed measurement so we dont interrupt Ble Communications
                    clockCorrectionCountdown = 3;
                    recheckOffset = 23LL * 60 * 60 * 1000000;       // Set Next check offset to subtract 23hours thus recheck in an hour
                }
                else
                {
                    printf("Can not adjust scale, ESP time not synchronized.");
                }
            }

            // Reset check for another 24 hours from now
            nextClockSyncAllowed = (currentUpTime + (24ULL * 60 * 60 * 1000000)) - recheckOffset;
        }
    }
}

uint64_t XiaomiScale::DecodeTimestampUTC(const uint8_t* serviceData)
{
    struct tm timestamp = {};

    timestamp.tm_year = (serviceData[4] | (serviceData[5] << 8)) - 1900;
    timestamp.tm_mon  = serviceData[6] - 1;
    timestamp.tm_mday = serviceData[7];
    timestamp.tm_hour = serviceData[8];
    timestamp.tm_min  = serviceData[9];
    timestamp.tm_sec  = serviceData[10];

    timestamp.tm_isdst = 0;

    return static_cast<uint64_t>(mktime(&timestamp));
}