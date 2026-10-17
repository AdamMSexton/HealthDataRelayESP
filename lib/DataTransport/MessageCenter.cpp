#include "MessageCenter.h"



void MessageCenter::EnqueueMessage(OutboundMessage newMessage)
{
    char* json = cJSON_PrintUnformatted(newMessage.jsonPayload);

    printf("Measurement ID: %s\n", newMessage.measurementId.c_str());
    printf("JSON Payload:   %s\n", json ? json : "(null)");
    printf("Journal Entry:  %s\n", newMessage.journalEntry.c_str());
    printf("Endpoint:       %s\n", newMessage.endpoint.c_str());

    if (json) cJSON_free(json);
}