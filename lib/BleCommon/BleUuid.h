#include <cstdint>
#include "BleUuidSize.h"

struct BleUuid
{
    BleUuidSize size;
    uint8_t value[16];
};