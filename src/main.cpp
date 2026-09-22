#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

extern "C" void app_main()
{
    printf("HealthDataCollector ESP32 starting...\n");

    while (true)
    {
        printf("Still alive.\n");
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}