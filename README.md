# SmartLivestock

Smart livestock controller based on ESP32 + MQTT + Home Assistant.

## Features (MVP)
- WiFi + MQTT connection and reconnect
- Sensor manager scaffold (SHT31, DS18B20, HX711, water level)
- Auto rule engine by livestock profile
- Relay output control
- MQTT telemetry and command topics

## Build
- PlatformIO environment: `esp32dev`

## MQTT Namespace
- Base: `smartfarm/livestock`
