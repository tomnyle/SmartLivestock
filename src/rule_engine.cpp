#include "rule_engine.h"
#include "livestock_profile.h"
#include "app_config.h"
#include "safety_manager.h"

void applyAutoRules(SystemState& state) {
  if (state.mode != SystemMode::AUTO || state.outputsLocked ||
      state.safety != SafetyState::RUNNING) {
    enforceOutputSafety(state);
    return;
  }
  LivestockProfile p = getProfile(state.profile);

  state.outputs.fan = state.outputs.fan
      ? state.sensors.temperature.value > p.maxTemp - TEMP_HYSTERESIS_C
      : state.sensors.temperature.value > p.maxTemp;
  const bool humidityHigh = state.outputs.exhaust
      ? state.sensors.humidity.value > p.maxHumidity - HUMIDITY_HYSTERESIS
      : state.sensors.humidity.value > p.maxHumidity;
  const bool nh3High = state.sensors.nh3.valid &&
                       state.sensors.nh3.value > p.nh3Limit;
  state.outputs.exhaust = humidityHigh || nh3High;
  state.outputs.heater = state.outputs.heater
      ? state.sensors.temperature.value < p.minTemp + TEMP_HYSTERESIS_C
      : state.sensors.temperature.value < p.minTemp;
  state.outputs.pump = state.outputs.pump
      ? state.sensors.waterLevel.value < 30.0f + WATER_LEVEL_HYSTERESIS
      : state.sensors.waterLevel.value < 30.0f;
  state.outputs.light = true;
  state.outputs.mist = false;
  state.outputs.feeder = false;
}
