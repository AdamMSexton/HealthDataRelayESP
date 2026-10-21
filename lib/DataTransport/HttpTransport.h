#pragma once

#include <string>

#include "cJSON.h"
#include "OutboundMessage.h"


class HttpTransport
{
    private:

    public:
      bool Send(OutboundMessage* message);
};