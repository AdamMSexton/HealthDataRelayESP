#include <string>
#include <cJSON.h>
#include <ctime>
#include <cmath>

#include "OutboundWeightProcessor.h"
#include "MeasurementType.h"
#include "MessageCenter.h"


uint64_t OutboundWeightProcessor::fnv1a64(const std::string& input)
{
    uint64_t hash = 14695981039346656037ULL;
    constexpr uint64_t prime = 1099511628211ULL;

    for (unsigned char c : input)
    {
        hash ^= c;
        hash *= prime;
    }

    return hash;
}

WeightMeasurement OutboundWeightProcessor::ConvertFromJournal(std::string journalEntry)
{
    WeightMeasurement measurement;
    measurement.impedanceOhms = 0;
    return measurement;
}

std::string OutboundWeightProcessor::ConvertToJournal(WeightMeasurement measurement, uint64_t measurementId)
{
    return std::to_string(measurementId) + "," + std::to_string(static_cast<int>(MeasurementType::Weight)) + "," + std::to_string(measurement.measurementTimeUTC) +
        "," + std::to_string(measurement.weightInKg) + "," + std::to_string(measurement.impedanceOhms);
}

cJSON* OutboundWeightProcessor::SerializeToJson(WeightMeasurement measurement, uint64_t measurementId)
{
    // Convert time to ISO 8601 string
    time_t measurementTime = static_cast<time_t>(measurement.measurementTimeUTC);
    tm utcTime{};
    gmtime_r(&measurementTime, &utcTime);
    char buffer[25];
    strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%SZ", &utcTime);

    cJSON* message = cJSON_CreateObject();
    cJSON_AddStringToObject(message, "MeasurementId", std::to_string(measurementId).c_str());
    cJSON_AddNumberToObject(message, "TransmittedAt", 0);
    cJSON_AddNumberToObject(message, "MeasuredAt", measurement.measurementTimeUTC);
    cJSON_AddNumberToObject(message, "WeightKg", measurement.weightInKg);
    cJSON_AddNumberToObject(message, "ImpedanceOhm", measurement.impedanceOhms);
    cJSON_AddNumberToObject(message, "UserId", 0);          // Placeholder, users not implemented yet
    
    return message;
}

uint64_t OutboundWeightProcessor::CreateMeasurementId(WeightMeasurement measurement)
{
    std::string rawSerial = std::to_string(measurement.measurementTimeUTC) + "-" + std::to_string(floor(measurement.weightInKg)) + "-" + std::to_string(measurement.impedanceOhms);
    return fnv1a64(rawSerial);
}

void OutboundWeightProcessor::SendOutboundMessage(WeightMeasurement newMeasurement)
{
    uint64_t measurementId = CreateMeasurementId(newMeasurement);
    std::string journalEntry = ConvertToJournal(newMeasurement,measurementId);
    cJSON* jsonPayload = SerializeToJson(newMeasurement,measurementId);
    
    OutboundMessage* finalMessage = new OutboundMessage{measurementId, jsonPayload, journalEntry, "/weight"};

    messageCenter.EnqueueMessage(finalMessage);
}

OutboundMessage OutboundWeightProcessor::MessageFromJournal(std::string journalEntry)
{
    OutboundMessage finalMessage;
    finalMessage.measurementId = 0;
    return finalMessage;
}