#include "BleScanner.h"
#include "BleSubscription.h"
#include "BleAdvertisement.h"

BleScanner* BleScanner::currentScanner = nullptr;
BleSubscription subscriptions[8];
uint8_t subscriptionCount = 0;

bool BleScanner::subscribe(const BleUuid& serviceUuid, BleAdvertisementHandler* handler)
{
  if (subscriptionCount >= 8)
  {
    return false;
  }

  if (handler == nullptr)
  {
    return false;
  }

  subscriptions[subscriptionCount].handler = handler;
  subscriptions[subscriptionCount].serviceUuid = serviceUuid;
  subscriptionCount++;

  return true;
}

int BleScanner::onBleEvent(struct ble_gap_event* event, void* arg)
{
    if (event->type == BLE_GAP_EVENT_DISC)
    {
        //const uint8_t* a = event->disc.addr.val;
        struct ble_hs_adv_fields fields;
        ble_hs_adv_parse_fields(&fields,event->disc.data,event->disc.length_data);

        for(int i = 0; i < fields.num_uuids16; i++)
        {
            uint16_t uuid = fields.svc_data_uuid16[0] | (fields.svc_data_uuid16[1] << 8);
            BleUuid thisUuid(uuid);
            
            for (int j = 0; j < subscriptionCount; j++)
            {
                if (thisUuid == subscriptions[j].serviceUuid)
                {
                    // Create advertisement
                    BleAdvertisement newAdvertisement;
                    newAdvertisement.serviceUuid = thisUuid;
                    newAdvertisement.rssi = event->disc.rssi;
                    for (int j = 0; j < 6; j++)
                    {
                        newAdvertisement.address[j] = event->disc.addr.val[j];
                    }

                    if (fields.svc_data_uuid16 != nullptr && fields.svc_data_uuid16_len >= 2)
                    {
                        newAdvertisement.serviceData =
                            fields.svc_data_uuid16;

                        newAdvertisement.serviceDataLength =
                            fields.svc_data_uuid16_len;
                    }

                    subscriptions[j].handler->ProcessAdvertisement(newAdvertisement);
                }
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