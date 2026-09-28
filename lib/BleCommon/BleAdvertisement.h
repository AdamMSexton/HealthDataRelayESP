#include <cstdint>

struct BleAdvertisement
{
    uint8_t address[6];
    int8_t rssi;

    uint16_t serviceUuid;

    const uint8_t* serviceData;     // Points to the bytes in NimBLE memory, ensure to copy out if persistence is needed beyond callback.
    uint16_t serviceDataLength;
};