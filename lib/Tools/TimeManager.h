#pragma once

#include <string>
#include <ctime>
#include <cmath>
#include <atomic>
#include <sys/time.h>

#include "esp_netif_sntp.h"
#include "esp_timer.h"

   
class TimeManager
{
    public:
        bool Init();
        bool IsTimeSynchronized() const;
        static std::string GetNowUtcIso8601();
        static std::string t64bToIso8601(uint64_t timeIn);
    
    private:
        static std::atomic<int64_t> lastNtpSync;
        static void SyncCallback(struct timeval* tv);
};


