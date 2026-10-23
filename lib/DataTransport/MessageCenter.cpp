#include "MessageCenter.h"
#include "TimeManager.h"


bool MessageCenter::Init()
{
    queueMutex = xSemaphoreCreateMutex();
    messageAvailable = xSemaphoreCreateBinary();

    BaseType_t result = xTaskCreate(MessageCenter::WorkerTask, "MessageWorker", 4096, this, 5, nullptr);
    if (result != pdPASS)
    {
        printf("Failed to create MessageWorker task\n");
        return false;
    }

    return true;
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
        if (xSemaphoreTake(queueMutex, portMAX_DELAY) == pdTRUE)
        {
            // Copy queue so we can iterate without modifying the original
            std::queue<MessageTicket*> tempQueue = messageQueue;

            xSemaphoreGive(queueMutex);

            printf("\nQueued messages: %zu\n", tempQueue.size());

            while (!tempQueue.empty())
            {
                MessageTicket* ticket = tempQueue.front();
                OutboundMessage* newMessage = ticket->payloadPtr;

                char* json = cJSON_PrintUnformatted(newMessage->jsonPayload);

                printf("\nMeasurement ID: %llu\n",
                    static_cast<unsigned long long>(newMessage->measurementId));
                printf("JSON Payload:   %s\n", json ? json : "(null)");
                printf("Journal Entry:  %s\n", newMessage->journalEntry.c_str());
                printf("Endpoint:       %s\n", newMessage->endpoint.c_str());

                if (json) cJSON_free(json);

                tempQueue.pop();
            }
        }
    }
}

void MessageCenter::WorkerTask(void* parameter)
{
    MessageCenter* center = static_cast<MessageCenter*>(parameter);
    center->Worker();
}

void MessageCenter::Worker()
{
    while (true)
    {
        // Wait for messages
        xSemaphoreTake(messageAvailable, portMAX_DELAY);

        MessageTicket* ticketToSend = nullptr;

        // Dequeue and process messages
        if (xSemaphoreTake(queueMutex, portMAX_DELAY)== pdTRUE)     // Hold for Mutex
        {
            if (!messageQueue.empty())
            {
                ticketToSend = messageQueue.front();     // Get next ticket
                messageQueue.pop();                                     // Remove ticket
            }

            xSemaphoreGive(queueMutex);                             // Release Mutex
        }

        if (ticketToSend == nullptr)
            continue;

        // Set time Stamps, update counter
        ticketToSend->timeLastTry = static_cast<int64_t>(time(nullptr));
        cJSON_SetValuestring(cJSON_GetObjectItem(ticketToSend->payloadPtr->jsonPayload,"TransmittedAt"),TimeManager::t64bToIso8601(ticketToSend->timeLastTry).c_str());
        ticketToSend->transmitAttempts++;

        httpTransport.Send(ticketToSend->payloadPtr);
    }
}