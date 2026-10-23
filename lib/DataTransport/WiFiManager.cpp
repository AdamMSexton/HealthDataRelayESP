#include "WiFiManager.h"

#include <cstring>
#include "WiFiConfig.h"
#include "esp_event.h"



bool WiFiManager::Init()
{
    // Init Network interface
    esp_err_t result = esp_netif_init();
    if (result != ESP_OK)
    {
        printf("Network interface initialization failed: %s\n", esp_err_to_name(result));
        return false;
    }

    // Setup event loop
    result = esp_event_loop_create_default();
    if (result != ESP_OK)
    {
        printf("Event loop initialization failed: %s\n",
            esp_err_to_name(result));
        return false;
    }

    esp_netif_t* initResult = esp_netif_create_default_wifi_sta();
    if (initResult == nullptr)
    {
        printf("WiFi initialization failed: %s\n",
            esp_err_to_name(result));
        return false;
    }

    // Create config structure
    wifi_init_config_t wifiConfig = WIFI_INIT_CONFIG_DEFAULT();
    result = esp_wifi_init(&wifiConfig);
    if (result != ESP_OK)
    {
        printf("Wi-Fi driver initialization failed: %s\n",
            esp_err_to_name(result));
        return false;
    }

    // Configure Wi-Fi as a client
    result = esp_wifi_set_mode(WIFI_MODE_STA);
    if (result != ESP_OK)
    {
        printf("Failed to set Wi-Fi station mode: %s\n",
            esp_err_to_name(result));
        return false;
    }

    // Load configuration
    wifi_config_t stationConfig = {};
    std::strncpy(
        reinterpret_cast<char*>(stationConfig.sta.ssid),
        WIFI_SSID,
        sizeof(stationConfig.sta.ssid) - 1
    );
    std::strncpy(
        reinterpret_cast<char*>(stationConfig.sta.password),
        WIFI_PASSWORD,
        sizeof(stationConfig.sta.password) - 1
    );

    // Pass configuration to WiFi driver  (ESP-IDF persists to NCS by default)
    result = esp_wifi_set_config(WIFI_IF_STA, &stationConfig);
    if (result != ESP_OK)
    {
        printf("Failed to configure Wi-Fi station: %s\n",
            esp_err_to_name(result));
        return false;
    }

    // Register event WiFi event handler
    result = esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &WiFiManager::EventHandler, this);
    if (result != ESP_OK)
    {
        printf("Failed to register Wi-Fi event handler: %s\n",
            esp_err_to_name(result));
        return false;
    }

    // Register IP event handler
    result = esp_event_handler_register( IP_EVENT, IP_EVENT_STA_GOT_IP, &WiFiManager::EventHandler, this);
    if (result != ESP_OK)
    {
        printf("Failed to register IP event handler: %s\n",
            esp_err_to_name(result));
        return false;
    }

    // Start WiFi driver
    result = esp_wifi_start();
    if (result != ESP_OK)
    {
        printf("Failed to start Wi-Fi: %s\n",
            esp_err_to_name(result));
        return false;
    }

    return true; 
}

void WiFiManager::EventHandler(void* arg, esp_event_base_t eventBase, int32_t eventId, void* eventData)
{
    if (eventBase == WIFI_EVENT)
    {
        if (eventId == WIFI_EVENT_STA_START)
        {
            // Initiate connection
            printf("Wi-Fi station started. Connecting...\n");

            esp_err_t result = esp_wifi_connect();

            if (result != ESP_OK)
            {
                printf("Wi-Fi connection attempt failed: %s\n",
                    esp_err_to_name(result));
            }
        }
        else if (eventId == WIFI_EVENT_STA_DISCONNECTED)
        {
            printf("Wi-Fi disconnected. Reconnecting...\n");

            esp_err_t result = esp_wifi_connect();
            if (result != ESP_OK)
            {
                printf("Wi-Fi reconnection failed: %s\n", esp_err_to_name(result));
            }
        }
    }
    else if (eventBase == IP_EVENT && eventId == IP_EVENT_STA_GOT_IP)
    {
        printf("Wi-Fi received an IP address\n");
    }
}