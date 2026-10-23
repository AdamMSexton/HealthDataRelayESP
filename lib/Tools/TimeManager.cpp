#include "TimeManager.h"
#include <string>
#include <cmath>


std::atomic<int64_t> TimeManager::lastNtpSync{0};

void TimeManager::SyncCallback(struct timeval* tv)
{
    lastNtpSync.store(esp_timer_get_time());

    printf("SNTP synchronization completed\n");
}

bool TimeManager::IsTimeSynchronized() const
{
    constexpr uint64_t MAX_SYNC_AGE = 24ULL * 60 * 60 * 1000000;    // Set max sync to 24hrs by default

    uint64_t upTimeNow = esp_timer_get_time();  // Store current uptime
    uint64_t lastSync = lastNtpSync.load();     // grab previously stored NTP sync uptime marker

    // Make sure the ESP has ever synced
    if (lastSync == 0)  
    {
        return false;
    }

    // Make sure there is not something funky with the counters
    if (upTimeNow < lastSync)
    {
        return false;
    }

    // Check to see if time is stale
    if ((upTimeNow - lastSync) > MAX_SYNC_AGE) 
    {
        return false;
    }

    return true;
}

std::string TimeManager::t64bToIso8601(uint64_t timeIn)  
{
    // Convert time to ISO 8601 string
    time_t numericTime = static_cast<time_t>(timeIn);
    tm utcTime{};
    gmtime_r(&numericTime, &utcTime);
    char stringTime[25];
    strftime(stringTime, sizeof(stringTime), "%Y-%m-%dT%H:%M:%SZ", &utcTime);
    
    return stringTime;
}

std::string GetNowUtcIso8601()
{
    return TimeManager::t64bToIso8601(static_cast<int64_t>(time(nullptr)));
}

bool TimeManager::Init()
{
    // Configure SNTP for DHCP with pool.ntp.org fallback
    esp_sntp_config_t sntpConfig = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");

    sntpConfig.server_from_dhcp = true;
    sntpConfig.renew_servers_after_new_IP = true;
    sntpConfig.index_of_first_server = 1;

    // Register SNTP callback
    sntpConfig.sync_cb = &TimeManager::SyncCallback;

    esp_err_t result = esp_netif_sntp_init(&sntpConfig);

    if (result != ESP_OK)
    {
        printf("SNTP initialization failed: %s\n",
            esp_err_to_name(result));
        return false;
    }
    printf("SNTP initialized successfully\n");
    return true;
}