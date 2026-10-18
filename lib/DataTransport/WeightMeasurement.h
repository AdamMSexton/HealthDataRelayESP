#pragma once
#include <cstdint>

struct WeightMeasurement
{
    double weightInKg;
    uint16_t impedanceOhms;
    int64_t measurementTimeUTC;
};