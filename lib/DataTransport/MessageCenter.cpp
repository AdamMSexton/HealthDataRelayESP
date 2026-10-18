#include "MessageCenter.h"



void MessageCenter::EnqueueMessage(OutboundMessage newMessage)
{
    char* json = cJSON_PrintUnformatted(newMessage.jsonPayload);

    printf("Measurement ID: %llu\n", newMessage.measurementId);
    printf("JSON Payload:   %s\n", json ? json : "(null)");
    printf("Journal Entry:  %s\n", newMessage.journalEntry.c_str());
    printf("Endpoint:       %s\n", newMessage.endpoint.c_str());

    printf("Before free JSON");
    if (json) cJSON_free(json);
    printf("After free JSON");
}