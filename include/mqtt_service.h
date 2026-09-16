#ifndef MQTT_SERVICE_H
#define MQTT_SERVICE_H

#include "system_state.h"

void mqttInit();
void mqttLoop(SystemState& state);
bool mqttConnected();
void mqttPublishState(const SystemState& state);

#endif
