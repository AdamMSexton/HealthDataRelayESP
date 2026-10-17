#include <string>
#include <cJSON.h>
#include <ctime>

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

cJSON* SerializeToJson(WeightMeasurement measurement, std::string measurementId)
{
    // Convert time to ISO 8601 string
    time_t measurementTime = static_cast<time_t>(measurement.measurementTimeUTC);
    tm utcTime{};
    gmtime_r(&measurementTime, &utcTime);
    char buffer[25];
    strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%SZ", &utcTime);

    cJSON* message = cJSON_CreateObject();
    cJSON_AddStringToObject(message, "MeasurementId", measurementId.c_str());
    

}

std::string CreateMeasurementId(WeightMeasurement measurement)
{
    return std::to_string(measurement.measurementTimeUTC) + "," + std::to_string(measurement.weightInKg) + "," + std::to_string(measurement.impedanceOhms);
}

OutboundMessage CreateOutboundMessage(WeightMeasurement newMeasurement)
{
    std::string measurementId = CreateMeasurementId(newMeasurement);
    std::string journalEntry = ConvertToJournal(newMeasurement,measurementId);
    cJSON* jsonPayload = SerializeToJson(newMeasurement,measurementId);

    OutboundMessage finalMessage = {measurementId, jsonPayload, journalEntry, "/weight"};

    messageCenter.EnqueueMessage(finalMessage);
}

OutboundMessage MessageFromJournal(std::string journalEntry)
{

}