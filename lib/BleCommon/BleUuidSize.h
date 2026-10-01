#pragma once
#include <cstdint>

enum class BleUuidSize : uint8_t
{
    Uuid16 = 2,
    Uuid32 = 4,
    Uuid128 = 16
};