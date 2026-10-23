#include "HttpTransport.h"


bool HttpTransport::Send(OutboundMessage* message)
{
    if(true)
    {
        OutboundMessage* newMessage = message;

        char* json = cJSON_PrintUnformatted(newMessage->jsonPayload);

        printf("***** RECEIVED FOR HTML XMIT *****");
        printf("\nMeasurement ID: %llu\n",
            static_cast<unsigned long long>(newMessage->measurementId));
        printf("JSON Payload:   %s\n", json ? json : "(null)");
        printf("Journal Entry:  %s\n", newMessage->journalEntry.c_str());
        printf("Endpoint:       %s\n", newMessage->endpoint.c_str());

        printf("NTP synchronized: %s\n", timeManager.IsTimeSynchronized() ? "YES" : "NO");
    }

    return true;
}