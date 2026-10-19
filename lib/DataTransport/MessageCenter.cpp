#include "MessageCenter.h"


MessageCenter::MessageCenter()
{
    queueMutex = xSemaphoreCreateMutex();
    messageAvailable = xSemaphoreCreateBinary();
}

void MessageCenter::EnqueueMessage(OutboundMessage* newMessage)
{
    // Sanity check overall message contents.
    if ((newMessage->endpoint.length() <= 2 ) || (newMessage->endpoint[0] != '/'))
    {
        // Invalid endpoint, do something 
        return;       
    }

    if (newMessage->journalEntry.length() <= 10)
    {
        // Journal entry probably bad, do something
        return;       
    }

    if ((newMessage->jsonPayload == nullptr) || (!cJSON_IsObject(newMessage->jsonPayload)))
    {
        // JSON payload probably bad, do something
        return;       
    }

    if (newMessage->measurementId == 0)
    {
        // Invalid measurementId
        return;       
    }

    // Build new ticket
    MessageTicket* newTicket = new MessageTicket;
    newTicket->payloadPtr = newMessage;

    if (xSemaphoreTake(queueMutex, portMAX_DELAY)== pdTRUE) // Hold for Mutex
    {
        messageQueue.push(newTicket);   // Enqueue new ticket
        xSemaphoreGive(queueMutex);     // Release Mutex

        xSemaphoreGive(messageAvailable);   // Notify worker message is available.
    }



    if (false)
    {
        char* json = cJSON_PrintUnformatted(newMessage->jsonPayload);
        printf("Measurement ID: %llu\n", newMessage->measurementId);
        printf("JSON Payload:   %s\n", json ? json : "(null)");
        printf("Journal Entry:  %s\n", newMessage->journalEntry.c_str());
        printf("Endpoint:       %s\n", newMessage->endpoint.c_str());
        
        printf("Before free JSON");
        if (json) cJSON_free(json);
        printf("After free JSON");
    }
}