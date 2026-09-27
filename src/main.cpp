#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

static int onBleEvent(struct ble_gap_event* event, void* arg)
{
    if (event->type == BLE_GAP_EVENT_DISC)
    {
        const uint8_t* a = event->disc.addr.val;

        struct ble_hs_adv_fields fields;
        ble_hs_adv_parse_fields(&fields,event->disc.data,event->disc.length_data);

        for(int i = 0; i < fields.num_uuids16; i++)
        {
            if (fields.uuids16[i].value == 0x181B)
            {
                printf(
                    "BLE: %02X:%02X:%02X:%02X:%02X:%02X  RSSI: %d\n",
                    a[5], a[4], a[3], a[2], a[1], a[0],
                    event->disc.rssi);
                    }
            }
        }



    return 0;
}

static void startScan()
{
    struct ble_gap_disc_params params = {};

    params.passive = 1;
    params.itvl = 0;
    params.window = 0;
    params.filter_duplicates = 0;

    int rc = ble_gap_disc(
        BLE_OWN_ADDR_PUBLIC,
        BLE_HS_FOREVER,
        &params,
        onBleEvent,
        nullptr);

    printf("BLE scan started. rc=%d\n", rc);
}

static void onSync()
{
    startScan();
}

static void nimbleTask(void* param)
{
    nimble_port_run();
    nimble_port_freertos_deinit();
}

extern "C" void app_main()
{
    printf("HealthDataCollector ESP32 starting...\n");

    nimble_port_init();

    ble_svc_gap_init();
    ble_svc_gatt_init();

    ble_hs_cfg.sync_cb = onSync;

    nimble_port_freertos_init(nimbleTask);
}