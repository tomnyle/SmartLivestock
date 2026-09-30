#include <cassert>
#include <iostream>

#include "app_config.h"
#include "safety_manager.h"

static SensorReading validReading(float value, uint32_t now) {
  SensorReading reading;
  reading.value = value;
  reading.available = true;
  reading.valid = true;
  reading.updatedAt = now;
  reading.fault = SensorFault::NONE;
  return reading;
}

static SystemState validState(uint32_t now) {
  SystemState state;
  state.sensors.temperature = validReading(25.0f, now);
  state.sensors.humidity = validReading(60.0f, now);
  state.sensors.waterTemperature = validReading(20.0f, now);
  state.sensors.waterLevel = validReading(100.0f, now);
  state.sensors.feedWeight = validReading(10.0f, now);
  return state;
}

static void enableAllOutputs(OutputState& outputs) {
  outputs.fan = true;
  outputs.exhaust = true;
  outputs.heater = true;
  outputs.pump = true;
  outputs.feeder = true;
  outputs.light = true;
  outputs.mist = true;
}

static void assertAllOutputsOff(const OutputState& outputs) {
  assert(!outputs.fan);
  assert(!outputs.exhaust);
  assert(!outputs.heater);
  assert(!outputs.pump);
  assert(!outputs.feeder);
  assert(!outputs.light);
  assert(!outputs.mist);
}

void test_boot_state_is_locked_and_safe() {
  SystemState state;
  assert(state.outputsLocked);
  assert(state.safety == SafetyState::SAFE);
  assert(state.lockReason == OutputLockReason::BOOT);
  assertAllOutputsOff(state.outputs);
}

void test_valid_fresh_sensors_unlock_outputs() {
  const uint32_t now = 1000;
  SystemState state = validState(now);
  evaluateSafety(state, now);
  assert(!state.outputsLocked);
  assert(state.safety == SafetyState::RUNNING);
}

void test_unavailable_required_sensor_locks_and_clears_outputs() {
  const uint32_t now = 1000;
  SystemState state = validState(now);
  state.sensors.waterTemperature.available = false;
  state.sensors.waterTemperature.valid = false;
  enableAllOutputs(state.outputs);
  evaluateSafety(state, now);
  assert(state.outputsLocked);
  assert(state.lockReason == OutputLockReason::SENSOR_UNAVAILABLE);
  assertAllOutputsOff(state.outputs);
}

void test_invalid_required_sensor_locks_outputs() {
  const uint32_t now = 1000;
  SystemState state = validState(now);
  state.sensors.feedWeight.valid = false;
  state.sensors.feedWeight.fault = SensorFault::OUT_OF_RANGE;
  evaluateSafety(state, now);
  assert(state.lockReason == OutputLockReason::SENSOR_INVALID);
  assertAllOutputsOff(state.outputs);
}

void test_stale_sensor_is_marked_and_locks_outputs() {
  const uint32_t now = SENSOR_STALE_TIMEOUT + 100;
  SystemState state = validState(1);
  evaluateSafety(state, now);
  assert(state.lockReason == OutputLockReason::SENSOR_STALE);
  assert(state.sensors.temperature.fault == SensorFault::STALE);
  assertAllOutputsOff(state.outputs);
}

void test_critical_threshold_locks_and_clears_outputs() {
  const uint32_t now = 1000;
  SystemState state = validState(now);
  state.sensors.temperature.value = CRITICAL_TEMP_MAX_C;
  enableAllOutputs(state.outputs);
  evaluateSafety(state, now);
  assert(state.lockReason == OutputLockReason::CRITICAL_THRESHOLD);
  assertAllOutputsOff(state.outputs);
}

void test_manual_safe_mode_cannot_enable_outputs() {
  const uint32_t now = 1000;
  SystemState state = validState(now);
  state.mode = SystemMode::SAFE;
  enableAllOutputs(state.outputs);
  evaluateSafety(state, now);
  assert(state.safety == SafetyState::SAFE);
  assert(state.lockReason == OutputLockReason::MANUAL_SAFE);
  assertAllOutputsOff(state.outputs);
}

void test_fault_recovery_requires_fresh_valid_data() {
  const uint32_t now = 1000;
  SystemState state = validState(now);
  state.sensors.temperature.valid = false;
  evaluateSafety(state, now);
  assert(state.safety == SafetyState::FAULT);

  state.sensors.temperature = validReading(25.0f, now + 1);
  evaluateSafety(state, now + 1);
  assert(state.safety == SafetyState::RUNNING);
  assert(!state.outputsLocked);
}

void test_unsupported_optional_gases_do_not_block_autonomy() {
  const uint32_t now = 1000;
  SystemState state = validState(now);
  evaluateSafety(state, now);
  assert(state.sensors.nh3.fault == SensorFault::UNAVAILABLE);
  assert(state.sensors.co2.fault == SensorFault::UNAVAILABLE);
  assert(state.sensors.h2s.fault == SensorFault::UNAVAILABLE);
  assert(state.safety == SafetyState::RUNNING);
}

int main() {
  test_boot_state_is_locked_and_safe();
  test_valid_fresh_sensors_unlock_outputs();
  test_unavailable_required_sensor_locks_and_clears_outputs();
  test_invalid_required_sensor_locks_outputs();
  test_stale_sensor_is_marked_and_locks_outputs();
  test_critical_threshold_locks_and_clears_outputs();
  test_manual_safe_mode_cannot_enable_outputs();
  test_fault_recovery_requires_fresh_valid_data();
  test_unsupported_optional_gases_do_not_block_autonomy();
  std::cout << "All safety tests passed\n";
  return 0;
}
