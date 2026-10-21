#pragma once

#include <string>
#include <ctime>
#include <cmath>
   
    
    
inline std::string t64bToIso8601(uint64_t timeIn)    
{
    // Convert time to ISO 8601 string
    time_t numericTime = static_cast<time_t>(timeIn);
    tm utcTime{};
    gmtime_r(&numericTime, &utcTime);
    char stringTime[25];
    strftime(stringTime, sizeof(stringTime), "%Y-%m-%dT%H:%M:%SZ", &utcTime);
    
    return stringTime;
}