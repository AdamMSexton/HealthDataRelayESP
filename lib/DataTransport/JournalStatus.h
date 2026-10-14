#pragma once
#include <cstdint>

enum class JournalStatus : uint8_t
{
    Undefined = 0,
    Pending = 1,
    Invalid = 9,
    Accepted = 69
};