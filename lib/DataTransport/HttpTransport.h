#pragma once

#include <string>

#include "cJSON.h"
#include "OutboundMessage.h"
#include "WiFiManager.h"
#include "TimeManager.h"

extern WiFiManager wifiManager;
extern TimeManager timeManager;

class HttpTransport
{
    private:

    public:
      bool Send(OutboundMessage* message);
};