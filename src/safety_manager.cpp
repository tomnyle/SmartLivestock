#include "app_config.h"
#include "safety_manager.h"

void clearOutputs(OutputState& outputs) {
  outputs = OutputState{};
}

bool sensorReadingFresh(const SensorReading& reading, uint32_t now) {
  return reading.available && reading.valid &&
         static_cast<uint32_t>(now - reading.updatedAt) <= SENSOR_STALE_TIMEOUT;
}

static bool requiredSensorFault(SensorReading& reading, bool required,
                                uint32_t now, OutputLockReason& reason) {
  if (!required) return false;
  if (!reading.available) {
    reason = OutputLockReason::SENSOR_UNAVAILABLE;
    return true;
  }
  if (reading.fault == SensorFault::STALE) {
    reason = OutputLockReason::SENSOR_STALE;
    return true;
  }
  if (!reading.valid) {
    reason = OutputLockReason::SENSOR_INVALID;
    return true;
  }
  if (!sensorReadingFresh(reading, now)) {
    reading.valid = false;
    reading.fault = SensorFault::STALE;
    reason = OutputLockReason::SENSOR_STALE;
    return true;
  }
  return false;
}

static bool criticalThresholdReached(const SensorData& sensors) {
  return (sensors.temperature.valid &&
          (sensors.temperature.value <= CRITICAL_TEMP_MIN_C ||
           sensors.temperature.value >= CRITICAL_TEMP_MAX_C)) ||
         (sensors.humidity.valid &&
          sensors.humidity.value >= CRITICAL_HUMIDITY_MAX) ||
         (sensors.waterTemperature.valid &&
          (sensors.waterTemperature.value <= CRITICAL_WATER_TEMP_MIN_C ||
           sensors.waterTemperature.value >= CRITICAL_WATER_TEMP_MAX_C));
}

void evaluateSafety(SystemState& state, uint32_t now) {
  OutputLockReason reason = OutputLockReason::NONE;

  if (state.mode == SystemMode::SAFE) {
    reason = OutputLockReason::MANUAL_SAFE;
  } else if (requiredSensorFault(state.sensors.temperature, REQUIRE_SHT31, now, reason) ||
             requiredSensorFault(state.sensors.humidity, REQUIRE_SHT31, now, reason) ||
             requiredSensorFault(state.sensors.waterTemperature, REQUIRE_DS18B20, now, reason) ||
             requiredSensorFault(state.sensors.waterLevel, REQUIRE_WATER_LEVEL, now, reason) ||
             requiredSensorFault(state.sensors.feedWeight, REQUIRE_HX711, now, reason) ||
             requiredSensorFault(state.sensors.nh3, REQUIRE_GAS_SENSORS, now, reason) ||
             requiredSensorFault(state.sensors.co2, REQUIRE_GAS_SENSORS, now, reason) ||
             requiredSensorFault(state.sensors.h2s, REQUIRE_GAS_SENSORS, now, reason)) {
  } else if (criticalThresholdReached(state.sensors)) {
    reason = OutputLockReason::CRITICAL_THRESHOLD;
  } else if (NETWORK_LOSS_LOCKS_OUTPUTS && state.networkLostAt != 0 &&
             static_cast<uint32_t>(now - state.networkLostAt) >= NETWORK_FAILURE_TIMEOUT) {
    reason = OutputLockReason::NETWORK_LOSS;
  }

  state.outputsLocked = reason != OutputLockReason::NONE;
  state.lockReason = reason;
  state.safety = state.outputsLocked ? SafetyState::FAULT : SafetyState::RUNNING;
  if (state.mode == SystemMode::SAFE) state.safety = SafetyState::SAFE;
  enforceOutputSafety(state);
}

void enforceOutputSafety(SystemState& state) {
  if (state.outputsLocked || state.safety != SafetyState::RUNNING) {
    clearOutputs(state.outputs);
  }
}

const char* safetyStateName(SafetyState state) {
  switch (state) {
    case SafetyState::RUNNING: return "RUNNING";
    case SafetyState::FAULT: return "FAULT";
    default: return "SAFE";
  }
}

const char* sensorFaultName(SensorFault fault) {
  switch (fault) {
    case SensorFault::INVALID: return "invalid";
    case SensorFault::OUT_OF_RANGE: return "out_of_range";
    case SensorFault::STALE: return "stale";
    case SensorFault::NONE: return "none";
    default: return "unavailable";
  }
}

const char* outputLockReasonName(OutputLockReason reason) {
  switch (reason) {
    case OutputLockReason::MANUAL_SAFE: return "manual_safe";
    case OutputLockReason::SENSOR_UNAVAILABLE: return "sensor_unavailable";
    case OutputLockReason::SENSOR_INVALID: return "sensor_invalid";
    case OutputLockReason::SENSOR_STALE: return "sensor_stale";
    case OutputLockReason::CRITICAL_THRESHOLD: return "critical_threshold";
    case OutputLockReason::NETWORK_LOSS: return "network_loss";
    case OutputLockReason::NONE: return "none";
    default: return "boot";
  }
}
