#include <stdio.h>
#include "BleManager.h"
#include "BleUuid.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"

#include "host/util/util.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

#include "XiaomiScale.h"
#include "MessageCenter.h"
#include "HttpTransport.h"
#include "WiFiManager.h"
#include "TimeManager.h"

// Create BT Manager object
BleManager scanner;
// Create WiFi controller
WiFiManager wifiManager;
// Create Message center
MessageCenter messageCenter;
// Create HTTP Transport Handler
HttpTransport httpTransport;
// Create TimeManager
TimeManager timeManager;

// Create scale
XiaomiScale scale;

extern "C" int app_main()
{
    printf("HealthDataCollector ESP32 starting...\n");

    // Initalize Non-Volitile Storage, mainly for WiFi Credentials
    esp_err_t result = nvs_flash_init();

    if (result != ESP_OK)
    {
        printf("NVS initialization failed: %s\n", esp_err_to_name(result));
        return false;
    }

    wifiManager.Init();

    messageCenter.Init();                           // Initalize message queue and create worker.

    BleUuid scaleServiceUuid;                       // Hardcode scale Uuid for now
    scaleServiceUuid.size = BleUuidSize::Uuid16;
    scaleServiceUuid.value[0] = 0x1B;
    scaleServiceUuid.value[1] = 0x18;

    // Register scale with scanner
    if (scanner.subscribe(scaleServiceUuid, &scale))
    {
        printf("Scanner registered\n");
    }
    else
    {
        printf("Scanner registration failed\n");
        return(1);
    }
   
    // Initalize scanner
    scanner.initalizeScanner();

    return(0);
}