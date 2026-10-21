#pragma once

#include <queue>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "OutboundMessage.h"
#include "MessageCenter.h"
#include "HttpTransport.h"

extern HttpTransport httpTransport;

struct MessageTicket
{
    uint8_t transmitAttempts = 0;
    uint64_t timeLastTry = 0;
    bool writtenToJournal = false;
    OutboundMessage* payloadPtr = nullptr;
};

class MessageCenter
{
    private:
        std::queue<MessageTicket*> messageQueue;
        SemaphoreHandle_t queueMutex = nullptr;
        SemaphoreHandle_t messageAvailable = nullptr;    
        void Worker();
        static void WorkerTask(void* parameter);

    public:
        void EnqueueMessage(OutboundMessage* newMessage);
        bool Init();
};