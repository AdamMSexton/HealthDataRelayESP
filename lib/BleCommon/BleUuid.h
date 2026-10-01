#pragma once
#include <cstdint>
#include "BleUuidSize.h"

struct BleUuid
{
    BleUuidSize size;
    uint8_t value[16];

    BleUuid()
    {
        size = BleUuidSize::Uuid16;
        value[0] = 0;
        value[1] = 0;
    }

    BleUuid(uint16_t uuid)
    {
        size = BleUuidSize::Uuid16;
        
        value[0] = uuid & 0xFF;
        value[1] = (uuid >> 8) & 0xFF;
    }

    bool operator==(const BleUuid& other) const
    {
        bool match = false;
        if (this->size == other.size)
        {
            match = true;
            for (int i = 0; i < static_cast<uint8_t>(this->size); i++)
            {
                if (this->value[i] != other.value[i])
                {
                    return false;
                }
            }
        }
        return match;
    }
};