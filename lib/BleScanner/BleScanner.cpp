#include "BleScanner.h"

BleScanner* BleScanner::currentScanner = nullptr;

int BleScanner::onBleEvent(struct ble_gap_event* event, void* arg)
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

void BleScanner::startScan()
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

void BleScanner::nimbleTask(void* param)
{
    nimble_port_run();
    nimble_port_freertos_deinit();
}

void BleScanner::onSync()
{
    currentScanner->startScan();
}

void BleScanner::initalizeScanner()
{
    currentScanner = this;
    
    // Initalize NimBLE host stack
    nimble_port_init();

    // Initalize GAP (Generic Access Profile) services.  Find devices.
    ble_svc_gap_init();

    // Initialize the standard BLE Generic Attribute Profile (GATT) service.  Use device data.
    ble_svc_gatt_init();

    ble_hs_cfg.sync_cb = onSync;

    nimble_port_freertos_init(nimbleTask);
}