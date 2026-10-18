#pragma once

#include <string>

#include "cJSON.h"


struct OutboundMessage
{
    uint64_t measurementId;
    cJSON* jsonPayload;
    std::string journalEntry;
    std::string endpoint;               // Include initial slash, i.e. "/weight"
};