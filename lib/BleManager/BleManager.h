#pragma once

#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"

#include "host/ble_hs.h"

#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "host/util/util.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

#include "BleUuid.h"
#include "BleAdvertisementHandler.h"

class BleManager
{
    public:
        void initalizeScanner();
        bool subscribe(const BleUuid& serviceUuid, BleAdvertisementHandler* handler);
        void SendTimeCorrectionNow(ble_addr_t targetAddress);
        
    private:
        static BleManager* currentScanner;
        static int onBleEvent(struct ble_gap_event* event, void* arg);
        void startScan();
        static void onSync();
        static void nimbleTask(void* param);

        static int process_BLE_GAP_EVENT_DISC(ble_gap_event* event);
        static int process_BLE_GAP_EVENT_CONNECT(ble_gap_event* event);
        static int process_BLE_GAP_EVENT_DISCONNECT(ble_gap_event* event);
};