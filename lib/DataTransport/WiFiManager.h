#pragma once

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "nvs_flash.h"

class WiFiManager
{
    public:
        bool Init();

    private:
        static void EventHandler(void* arg, esp_event_base_t eventBase, int32_t eventId, void* eventData);
};