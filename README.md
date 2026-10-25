# HealthDataRelay

HealthDataRelay is an ESP32-based firmware project designed to collect measurement data from Bluetooth Low Energy (BLE) health devices and forward it to a self-hosted API.

The project is being developed alongside **HomeDataCollector**, a separate C#/.NET application responsible for receiving and processing measurements.

The primary goal is to collect device measurements locally without requiring manufacturer cloud services.

## Overview

HealthDataRelay acts as an intermediary between BLE devices and a local server.

The intended data flow is:

```text
BLE Health Device
       |
       | Bluetooth Low Energy
       v
ESP32 / HealthDataRelay
       |
       | Wi-Fi / HTTP
       v
HomeDataCollector (.NET API)
       |
       v
Persistent Storage
```

The ESP32 handles device communication, measurement processing, and eventual transmission to the server.

## Technology

- **Language:** C++
- **Hardware:** ESP32
- **Communication:** Bluetooth Low Energy (BLE), Wi-Fi
- **Data handling:** JSON, planned LittleFS persistence
- **Backend:** HomeDataCollector (C# / ASP.NET Core), maintained separately

## Current Development

The firmware is under active development.

Implemented functionality includes:

- Processing BLE advertisements from a supported Bluetooth scale.
- Decoding weight and impedance measurements.
- Identifying stable measurements.
- Generating deterministic 64-bit measurement identifiers from measurement data.
- Structuring measurements for subsequent processing.

The current development priority is transmitting measurements to the HomeDataCollector HTTP API.

Supporting components for acknowledgement tracking, retry handling, and local persistence are being developed but are not yet fully integrated.

## Reliability Design

The firmware is designed around reliable measurement delivery, including recovery from temporary network or API outages.

The planned delivery process includes:

- Tracking measurements using deterministic 64-bit identifiers.
- Sending measurements to the receiving API.
- Tracking acknowledgements and retrying unacknowledged measurements.
- Using progressive retry backoff.
- Journaling pending measurements to LittleFS.
- Recovering pending measurements after a restart.
- Compacting the journal to remove completed entries.

The measurement identifier generation is implemented. HTTP delivery and persistent journaling are still under development.

## Time Handling

The project uses UTC for internal timestamps and stored measurements.

The ESP32 synchronizes its clock using NTP and receives apparent clock-offset measurements from HomeDataCollector to help determine when additional synchronization is needed. Timestamps received from connected IoT devices are also evaluated for excessive clock drift, with time corrections sent to those devices when necessary.

Timezone conversion is handled by the receiving application when presenting data to users.

## Configuration

Wi-Fi credentials are maintained in a local configuration file excluded from version control.

Users must provide their own network configuration before building and deploying the firmware.

No production API credentials are included in this repository.

## Project Status

**Active development / experimental**

The project currently focuses on BLE measurement acquisition and the supporting message-processing architecture.

HTTP transmission, complete acknowledgement handling, and end-to-end integration with HomeDataCollector remain future development milestones.

## Related Project

**HomeDataCollector** — A self-hosted ASP.NET Core application intended to receive and persist measurements from HealthDataRelay and other local data sources.

## License

See the repository's `LICENSE` file for licensing terms.