#pragma once
#include <cstdint>

struct WeightMeasurement
{
    float weightInKg;
    uint16_t impedanceOhms;
    int64_t measurementTimeUTC;
};