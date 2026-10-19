#pragma once

#include<string>

#include "WeightMeasurement.h"
#include "OutboundMessage.h"
#include "MessageCenter.h"

extern MessageCenter messageCenter;

class OutboundWeightProcessor
{
    private:
        WeightMeasurement ConvertFromJournal(std::string journalEntry);
        std::string ConvertToJournal(WeightMeasurement measurement, uint64_t measurementId);
        cJSON* SerializeToJson(WeightMeasurement measurement, uint64_t measurementId);
        uint64_t CreateMeasurementId(WeightMeasurement measurement);
        uint64_t fnv1a64(const std::string& input);
    public:
        
        void SendOutboundMessage(WeightMeasurement newMeasurement);
        OutboundMessage MessageFromJournal(std::string journalEntry);
};