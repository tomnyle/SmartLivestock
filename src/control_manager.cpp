#include <Arduino.h>
#include "pins.h"
#include "control_manager.h"

static void setRelay(int pin, bool on) {
  digitalWrite(pin, on ? RELAY_ON : RELAY_OFF);
}

void controlInit() {
  int pins[] = {PIN_RELAY_FAN, PIN_RELAY_EXHAUST, PIN_RELAY_HEATER, PIN_RELAY_PUMP, PIN_RELAY_FEEDER, PIN_RELAY_LIGHT, PIN_RELAY_MIST};
  for (int p : pins) { pinMode(p, OUTPUT); digitalWrite(p, RELAY_OFF); }
}

void applyOutputs(const OutputState& out) {
  setRelay(PIN_RELAY_FAN, out.fan);
  setRelay(PIN_RELAY_EXHAUST, out.exhaust);
  setRelay(PIN_RELAY_HEATER, out.heater);
  setRelay(PIN_RELAY_PUMP, out.pump);
  setRelay(PIN_RELAY_FEEDER, out.feeder);
  setRelay(PIN_RELAY_LIGHT, out.light);
  setRelay(PIN_RELAY_MIST, out.mist);
}
