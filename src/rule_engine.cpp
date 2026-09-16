#include "rule_engine.h"
#include "livestock_profile.h"

void applyAutoRules(SystemState& state) {
  if (state.mode != SystemMode::AUTO) return;
  LivestockProfile p = getProfile(state.profile);

  state.outputs.fan = (state.sensors.temperature > p.maxTemp);
  state.outputs.exhaust = (state.sensors.humidity > p.maxHumidity) || (state.sensors.nh3 > p.nh3Limit);
  state.outputs.heater = (state.sensors.temperature < p.minTemp);
  state.outputs.pump = (state.sensors.waterLevel < 30.0f);
  state.outputs.light = true;
  state.outputs.mist = false;
  state.outputs.feeder = false;
}
