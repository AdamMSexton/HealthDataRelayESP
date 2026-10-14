#pragma once
#include <string>

struct OutboundMessage
{
    std::string measurementId;
    std::string jsonPayload;
    std::string journalEntry;
    std::string endpoint;               // Include initial slash, i.e. "/weight"
};