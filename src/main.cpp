#include <stdio.h>
#include "BleScanner.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"



#include "host/util/util.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"




extern "C" int app_main()
{
    printf("HealthDataCollector ESP32 starting...\n");

    BleScanner scanner;
    scanner.initalizeScanner();

    return(0);
}