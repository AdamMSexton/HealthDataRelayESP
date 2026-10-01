#include <stdio.h>
#include "BleScanner.h"
#include "BleUuid.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "host/util/util.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

#include "XiaomiScale.h"



extern "C" int app_main()
{
    printf("HealthDataCollector ESP32 starting...\n");

    // Create scale
    XiaomiScale scale;
    
    BleUuid scaleServiceUuid;                       // Hardcode scale Uuid for now
    scaleServiceUuid.size = BleUuidSize::Uuid16;
    scaleServiceUuid.value[0] = 0x1B;
    scaleServiceUuid.value[1] = 0x1B;
    
    // Create BT Scanner object
    BleScanner scanner;
    // Register scale with scanner
    if (scanner.subscribe(scaleServiceUuid, &scale))
    {
        printf("Scanner registered");
    }
    else
    {
        printf("Scanner registration failed");
        return(1);
    }
    

    // Initalize scanner
    scanner.initalizeScanner();

    return(0);
}