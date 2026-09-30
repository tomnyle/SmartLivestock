#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "app_config.h"
#include "mqtt_service.h"
#include "livestock_profile.h"
#include "safety_manager.h"

static WiFiClient espClient;
static PubSubClient client(espClient);
static SystemState* gState = nullptr;
static unsigned long lastReconnect = 0;

static void handleSetBool(SystemState& state, bool& target, const String& payload) {
  const bool requested = payload == "ON" || payload == "1" || payload == "true";
  target = requested && !state.outputsLocked &&
           state.safety == SafetyState::RUNNING;
}

static void callback(char* topic, byte* payload, unsigned int length) {
  String t(topic), p;
  for (unsigned int i = 0; i < length; i++) p += (char)payload[i];

  if (!gState) return;

  if (t == MQTT_TOPIC_CMD_MODE) {
    p.toUpperCase();
    if (p == "AUTO") gState->mode = SystemMode::AUTO;
    else if (p == "MANUAL") gState->mode = SystemMode::MANUAL;
    else gState->mode = SystemMode::SAFE;
    if (gState->mode == SystemMode::SAFE) {
      gState->outputsLocked = true;
      gState->safety = SafetyState::SAFE;
      gState->lockReason = OutputLockReason::MANUAL_SAFE;
      clearOutputs(gState->outputs);
    }
  } else if (t == MQTT_TOPIC_CMD_PROFILE) {
    gState->profile = profileFromString(p);
  } else if (t == MQTT_TOPIC_CMD_FAN) handleSetBool(*gState, gState->outputs.fan, p);
  else if (t == MQTT_TOPIC_CMD_EXHAUST) handleSetBool(*gState, gState->outputs.exhaust, p);
  else if (t == MQTT_TOPIC_CMD_HEATER) handleSetBool(*gState, gState->outputs.heater, p);
  else if (t == MQTT_TOPIC_CMD_PUMP) handleSetBool(*gState, gState->outputs.pump, p);
  else if (t == MQTT_TOPIC_CMD_FEEDER) handleSetBool(*gState, gState->outputs.feeder, p);
  else if (t == MQTT_TOPIC_CMD_LIGHT) handleSetBool(*gState, gState->outputs.light, p);
  else if (t == MQTT_TOPIC_CMD_MIST) handleSetBool(*gState, gState->outputs.mist, p);
}

void mqttInit() {
  client.setServer(MQTT_BROKER, MQTT_PORT);
  client.setCallback(callback);
  client.setSocketTimeout(2);
}

static void subscribeTopics() {
  client.subscribe(MQTT_TOPIC_CMD_MODE);
  client.subscribe(MQTT_TOPIC_CMD_PROFILE);
  client.subscribe(MQTT_TOPIC_CMD_FAN);
  client.subscribe(MQTT_TOPIC_CMD_EXHAUST);
  client.subscribe(MQTT_TOPIC_CMD_HEATER);
  client.subscribe(MQTT_TOPIC_CMD_PUMP);
  client.subscribe(MQTT_TOPIC_CMD_FEEDER);
  client.subscribe(MQTT_TOPIC_CMD_LIGHT);
  client.subscribe(MQTT_TOPIC_CMD_MIST);
}

static void reconnectIfNeeded(SystemState& state) {
  if (WiFi.status() != WL_CONNECTED || MQTT_BROKER[0] == '\0') return;
  if (client.connected()) return;
  if (millis() - lastReconnect < MQTT_RECONNECT_INTERVAL) return;
  lastReconnect = millis();

  if (client.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD, MQTT_TOPIC_STATUS, 1, true, "offline")) {
    gState = &state;
    client.publish(MQTT_TOPIC_STATUS, "online", true);
    subscribeTopics();
  }
}

void mqttLoop(SystemState& state) {
  reconnectIfNeeded(state);
  if (client.connected()) client.loop();
}

bool mqttConnected() { return client.connected(); }

void mqttPublishState(const SystemState& s) {
  if (!client.connected()) return;

  const auto publishReading = [](const char* topic, const SensorReading& reading) {
    client.publish(topic, reading.valid ? String(reading.value, 2).c_str() : "unavailable", true);
  };
  publishReading(MQTT_TOPIC_TELEMETRY_TEMP, s.sensors.temperature);
  publishReading(MQTT_TOPIC_TELEMETRY_HUMIDITY, s.sensors.humidity);
  publishReading(MQTT_TOPIC_TELEMETRY_NH3, s.sensors.nh3);
  publishReading(MQTT_TOPIC_TELEMETRY_CO2, s.sensors.co2);
  publishReading(MQTT_TOPIC_TELEMETRY_H2S, s.sensors.h2s);
  publishReading(MQTT_TOPIC_TELEMETRY_WATER_TEMP, s.sensors.waterTemperature);
  publishReading(MQTT_TOPIC_TELEMETRY_WATER_LEVEL, s.sensors.waterLevel);
  publishReading(MQTT_TOPIC_TELEMETRY_FEED_WEIGHT, s.sensors.feedWeight);

  client.publish(MQTT_TOPIC_STATE_FAN, s.outputs.fan ? "ON":"OFF", true);
  client.publish(MQTT_TOPIC_STATE_EXHAUST, s.outputs.exhaust ? "ON":"OFF", true);
  client.publish(MQTT_TOPIC_STATE_HEATER, s.outputs.heater ? "ON":"OFF", true);
  client.publish(MQTT_TOPIC_STATE_PUMP, s.outputs.pump ? "ON":"OFF", true);
  client.publish(MQTT_TOPIC_STATE_FEEDER, s.outputs.feeder ? "ON":"OFF", true);
  client.publish(MQTT_TOPIC_STATE_LIGHT, s.outputs.light ? "ON":"OFF", true);
  client.publish(MQTT_TOPIC_STATE_MIST, s.outputs.mist ? "ON":"OFF", true);

  const char* mode = (s.mode == SystemMode::AUTO) ? "AUTO" : (s.mode == SystemMode::MANUAL ? "MANUAL" : "SAFE");
  client.publish(MQTT_TOPIC_MODE_STATE, mode, true);
  client.publish(MQTT_TOPIC_PROFILE_STATE, profileToString(s.profile), true);
  client.publish(MQTT_TOPIC_SAFETY_STATE, safetyStateName(s.safety), true);
  client.publish(MQTT_TOPIC_OUTPUT_LOCK, outputLockReasonName(s.lockReason), true);

  String faults;
  const auto appendFault = [&faults](const char* name, const SensorReading& reading) {
    if (reading.fault == SensorFault::NONE) return;
    if (faults.length()) faults += ",";
    faults += name;
    faults += ":";
    faults += sensorFaultName(reading.fault);
  };
  appendFault("temperature", s.sensors.temperature);
  appendFault("humidity", s.sensors.humidity);
  appendFault("water_temperature", s.sensors.waterTemperature);
  appendFault("water_level", s.sensors.waterLevel);
  appendFault("feed_weight", s.sensors.feedWeight);
  appendFault("nh3", s.sensors.nh3);
  appendFault("co2", s.sensors.co2);
  appendFault("h2s", s.sensors.h2s);
  client.publish(MQTT_TOPIC_SENSOR_FAULTS,
                 faults.length() ? faults.c_str() : "none", true);
}
