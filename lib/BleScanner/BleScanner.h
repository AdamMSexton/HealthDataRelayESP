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

class BleScanner
{
    public:
        void initalizeScanner();
        
    private:
        static BleScanner* currentScanner;
        static int onBleEvent(struct ble_gap_event* event, void* arg);
        void startScan();
        static void onSync();
        static void nimbleTask(void* param);
};