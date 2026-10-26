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

int BleManager::onGapEvent(struct ble_gap_event* event, void* arg)
{
    switch(event->type)
    {
        case BLE_GAP_EVENT_DISC:
            process_BLE_GAP_EVENT_DISC(event);
            break;
        case BLE_GAP_EVENT_CONNECT:
            process_BLE_GAP_EVENT_CONNECT(event);
            break;
        case BLE_GAP_EVENT_DISCONNECT:
            process_BLE_GAP_EVENT_DISCONNECT(event);
            break;
        default:
            break;
    }    
    return 0;
}

int BleManager::onGattServiceDiscovered(uint16_t conn_handle, const ble_gatt_error* error, const ble_gatt_svc* service, void* arg)
{
    if (error->status == 0)
    {
        printf("GATT service found: start=%u, end=%u\n",
               service->start_handle, service->end_handle);
    }
    else if (error->status == BLE_HS_EDONE)
    {
        printf("GATT service discovery complete.\n");


        int rc = ble_gap_terminate(conn_handle, BLE_ERR_REM_USER_CONN_TERM);
        printf("BLE terminate requested. rc=%d\n", rc);
    }
    else
    {
        printf("GATT service discovery failed: %d\n", error->status);
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
        onGapEvent,
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

    rc = ble_gap_connect(BLE_OWN_ADDR_PUBLIC,&targetAddress,10000,nullptr,onGapEvent,nullptr);

    if (rc != 0)
    {
        printf("BLE connection initiation failed: %d\n", rc);
        currentScanner->startScan();
    }
}



int BleManager::process_BLE_GAP_EVENT_DISC(ble_gap_event* event)
{
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
    
    return 0;
}

int BleManager::process_BLE_GAP_EVENT_CONNECT(ble_gap_event* event)
{
    if (event->connect.status == 0)
    {
        printf("BLE connected successfully. Handle: %d\n",event->connect.conn_handle);

        ble_uuid16_t serviceUuid = BLE_UUID16_INIT(0x181B);

        int rc = ble_gattc_disc_svc_by_uuid(event->connect.conn_handle,&serviceUuid.u,onGattServiceDiscovered,nullptr);

        if (rc != 0)
        {
            printf("Failed to start GATT service discovery: %d\n", rc);
            ble_gap_terminate(event->connect.conn_handle, BLE_ERR_REM_USER_CONN_TERM);
        }
    }
    else if (event->connect.status == BLE_HS_EDONE)
    {
        printf("GATT service discovery complete.\n");
        ble_gap_terminate(event->connect.conn_handle, BLE_ERR_REM_USER_CONN_TERM);
    }
    else
    {
        printf("BLE connection failed. Status: %d\n",event->connect.status);
        currentScanner->startScan();
    }
    
    return 0;
}

int BleManager::process_BLE_GAP_EVENT_DISCONNECT(ble_gap_event* event)
{
    printf("BLE disconnected. Reason: %d\n", event->disconnect.reason);

    currentScanner->startScan();

    return 0;
}