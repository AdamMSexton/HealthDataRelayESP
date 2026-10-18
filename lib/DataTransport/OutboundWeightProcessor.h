#pragma once

#include<string>

#include "WeightMeasurement.h"
#include "OutboundMessage.h"

class OutboundWeightProcessor
{
    private:
        WeightMeasurement ConvertFromJournal(std::string journalEntry);
        std::string ConvertToJournal(WeightMeasurement measurement, uint64_t measurementId);
        cJSON* SerializeToJson(WeightMeasurement measurement, uint64_t measurementId);
        uint64_t CreateMeasurementId(WeightMeasurement measurement);
        uint64_t fnv1a64(const std::string& input);
    public:
        
        OutboundMessage CreateOutboundMessage(WeightMeasurement newMeasurement);
        OutboundMessage MessageFromJournal(std::string journalEntry);
};