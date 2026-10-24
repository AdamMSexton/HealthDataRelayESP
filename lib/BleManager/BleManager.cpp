#include "BleManager.h"
#include "BleSubscription.h"
#include "BleAdvertisement.h"

BleManager* BleManager::currentScanner = nullptr;
BleSubscription subscriptions[8];
uint8_t subscriptionCount = 0;

bool BleManager::subscribe(const BleUuid& serviceUuid, BleAdvertisementHandler* handler)
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

int BleManager::onBleEvent(struct ble_gap_event* event, void* arg)
{
    if (event->type == BLE_GAP_EVENT_DISC)
    {
        //const uint8_t* a = event->disc.addr.val;
        struct ble_hs_adv_fields fields;
        ble_hs_adv_parse_fields(&fields,event->disc.data,event->disc.length_data);

        if ((fields.svc_data_uuid16 != nullptr) && (fields.svc_data_uuid16_len >= 2))
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

                    newAdvertisement.addressType = event->disc.addr.type;

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
    else if (event->type == BLE_GAP_EVENT_CONNECT)
    {
        if (event->connect.status == 0)
        {
            printf("BLE connected successfully. Handle: %d\n",event->connect.conn_handle);

            // For testing, immediately disconnect.
            ble_gap_terminate(event->connect.conn_handle,BLE_ERR_REM_USER_CONN_TERM);
        }
        else
        {
            printf("BLE connection failed. Status: %d\n",event->connect.status);
            currentScanner->startScan();
        }
    }
    else if (event->type == BLE_GAP_EVENT_DISCONNECT)
    {
        printf("BLE disconnected. Reason: %d\n",
            event->disconnect.reason);

        currentScanner->startScan();
    }


    return 0;
}

void BleManager::startScan()
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

void BleManager::nimbleTask(void* param)
{
    nimble_port_run();
    nimble_port_freertos_deinit();
}

void BleManager::onSync()
{
    currentScanner->startScan();
}

void BleManager::initalizeScanner()
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

void BleManager::SendTimeCorrectionNow(ble_addr_t targetAddress)
{
    int rc = ble_gap_disc_cancel();

    if (rc != 0)
    {
        printf("BLE scan cancel failed: %d\n", rc);
        return;
    }

    rc = ble_gap_connect(BLE_OWN_ADDR_PUBLIC,&targetAddress,10000,nullptr,onBleEvent,nullptr);

    if (rc != 0)
    {
        printf("BLE connection initiation failed: %d\n", rc);
        currentScanner->startScan();
    }
}