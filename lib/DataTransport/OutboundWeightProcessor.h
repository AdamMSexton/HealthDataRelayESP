#pragma once

#include<string>

#include "WeightMeasurement.h"
#include "OutboundMessage.h"

extern MessageCenter messageCenter;

class OutboundWeightProcessor
{
    private:
        WeightMeasurement ConvertFromJournal(std::string journalEntry);
        std::string ConvertToJournal(WeightMeasurement measurement, std::string measurementId);
        std::string SerializeToJson(WeightMeasurement measurement);
        std::string CreateMeasurementId(WeightMeasurement measurement);

    public:
        
        OutboundMessage CreateOutboundMessage(WeightMeasurement newMeasurement);
        OutboundMessage MessageFromJournal(std::string journalEntry);
};