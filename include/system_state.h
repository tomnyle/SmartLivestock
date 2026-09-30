#ifndef SYSTEM_STATE_H
#define SYSTEM_STATE_H

#include <stdint.h>

enum class SystemMode { AUTO, MANUAL, SAFE };
enum class SafetyState { SAFE, RUNNING, FAULT };
enum class SensorFault { NONE, UNAVAILABLE, INVALID, OUT_OF_RANGE, STALE };
enum class OutputLockReason {
  BOOT,
  MANUAL_SAFE,
  SENSOR_UNAVAILABLE,
  SENSOR_INVALID,
  SENSOR_STALE,
  CRITICAL_THRESHOLD,
  NETWORK_LOSS,
  NONE
};
enum class LivestockType { CHICKEN, DUCK, QUAIL, PIG, COW, GOAT, RABBIT };

struct SensorReading {
  float value = 0.0f;
  bool available = false;
  bool valid = false;
  uint32_t updatedAt = 0;
  SensorFault fault = SensorFault::UNAVAILABLE;
};

struct SensorData {
  SensorReading temperature;
  SensorReading humidity;
  SensorReading waterTemperature;
  SensorReading nh3;
  SensorReading co2;
  SensorReading h2s;
  SensorReading waterLevel;
  SensorReading feedWeight;
};

struct OutputState {
  bool fan = false;
  bool exhaust = false;
  bool heater = false;
  bool pump = false;
  bool feeder = false;
  bool light = false;
  bool mist = false;
};

struct SystemState {
  SystemMode mode = SystemMode::AUTO;
  SafetyState safety = SafetyState::SAFE;
  bool outputsLocked = true;
  OutputLockReason lockReason = OutputLockReason::BOOT;
  bool wifiConnected = false;
  bool mqttConnected = false;
  uint32_t networkLostAt = 0;
  LivestockType profile = LivestockType::CHICKEN;
  SensorData sensors;
  OutputState outputs;
};

#endif // SYSTEM_STATE_H
