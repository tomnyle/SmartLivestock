#ifndef SYSTEM_STATE_H
#define SYSTEM_STATE_H

#include <Arduino.h>

enum class SystemMode { AUTO, MANUAL, SAFE };
enum class LivestockType { CHICKEN, DUCK, QUAIL, PIG, COW, GOAT, RABBIT };

struct SensorData {
  float temperature = 0.0f;
  float humidity = 0.0f;
  float nh3 = 0.0f;
  float co2 = 0.0f;
  float h2s = 0.0f;
  float waterLevel = 0.0f;
  float feedWeight = 0.0f;
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
  LivestockType profile = LivestockType::CHICKEN;
  SensorData sensors;
  OutputState outputs;
};

#endif // SYSTEM_STATE_H
