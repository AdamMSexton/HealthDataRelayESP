#include <string>

#include "OutboundWeightProcessor.h"
#include "MeasurementType.h"
#include "MessageCenter.h"



WeightMeasurement ConvertFromJournal(std::string journalEntry)
{

}

std::string ConvertToJournal(WeightMeasurement measurement, std::string measurementId)
{
    return measurementId + "," + std::to_string(static_cast<int>(MeasurementType::Weight)) + "," +  
    std::to_string(measurement.measurementTimeUTC) + "," + std::to_string(measurement.weightInKg) + "," + std::to_string(measurement.impedanceOhms);
}

std::string SerializeToJson(WeightMeasurement measurement)
{

}

std::string CreateMeasurementId(WeightMeasurement measurement)
{
    return std::to_string(measurement.measurementTimeUTC) + "," + std::to_string(measurement.weightInKg) + "," + std::to_string(measurement.impedanceOhms);
}

OutboundMessage CreateMessage(WeightMeasurement newMeasurement)
{
    std::string measurementId = CreateMeasurementId(newMeasurement);
    std::string journalEntry = ConvertToJournal(newMeasurement,measurementId);
    std::string jsonPayload = SerializeToJson(newMeasurement);

    OutboundMessage finalMessage = {measurementId, jsonPayload, journalEntry, "/weight"};

    //MessageCenter::EnqueueMessage(finalMessage);
}

OutboundMessage MessageFromJournal(std::string journalEntry)
{

}