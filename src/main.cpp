#include <stdio.h>
#include "BleScanner.h"
#include "BleUuid.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "host/util/util.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

#include "XiaomiScale.h"
#include "MessageCenter.h"
#include "HttpTransport.h"


// Create scale
XiaomiScale scale;
// Create BT Scanner object
BleScanner scanner;
// Create Message center
MessageCenter messageCenter;
// Create HTTP Transport Handler
HttpTransport httpTransport;


extern "C" int app_main()
{
    printf("HealthDataCollector ESP32 starting...\n");

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