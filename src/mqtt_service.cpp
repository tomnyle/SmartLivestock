#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "app_config.h"
#include "mqtt_service.h"
#include "livestock_profile.h"

static WiFiClient espClient;
static PubSubClient client(espClient);
static SystemState* gState = nullptr;
static unsigned long lastReconnect = 0;

static void handleSetBool(bool& target, const String& payload) {
  target = (payload == "ON" || payload == "1" || payload == "true");
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
  } else if (t == MQTT_TOPIC_CMD_PROFILE) {
    gState->profile = profileFromString(p);
  } else if (t == MQTT_TOPIC_CMD_FAN) handleSetBool(gState->outputs.fan, p);
  else if (t == MQTT_TOPIC_CMD_EXHAUST) handleSetBool(gState->outputs.exhaust, p);
  else if (t == MQTT_TOPIC_CMD_HEATER) handleSetBool(gState->outputs.heater, p);
  else if (t == MQTT_TOPIC_CMD_PUMP) handleSetBool(gState->outputs.pump, p);
  else if (t == MQTT_TOPIC_CMD_FEEDER) handleSetBool(gState->outputs.feeder, p);
  else if (t == MQTT_TOPIC_CMD_LIGHT) handleSetBool(gState->outputs.light, p);
  else if (t == MQTT_TOPIC_CMD_MIST) handleSetBool(gState->outputs.mist, p);
}

void mqttInit() {
  client.setServer(MQTT_BROKER, MQTT_PORT);
  client.setCallback(callback);
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

  client.publish(MQTT_TOPIC_TELEMETRY_TEMP, String(s.sensors.temperature, 2).c_str(), true);
  client.publish(MQTT_TOPIC_TELEMETRY_HUMIDITY, String(s.sensors.humidity, 2).c_str(), true);
  client.publish(MQTT_TOPIC_TELEMETRY_NH3, String(s.sensors.nh3, 2).c_str(), true);
  client.publish(MQTT_TOPIC_TELEMETRY_CO2, String(s.sensors.co2, 2).c_str(), true);
  client.publish(MQTT_TOPIC_TELEMETRY_H2S, String(s.sensors.h2s, 2).c_str(), true);
  client.publish(MQTT_TOPIC_TELEMETRY_WATER_LEVEL, String(s.sensors.waterLevel, 2).c_str(), true);
  client.publish(MQTT_TOPIC_TELEMETRY_FEED_WEIGHT, String(s.sensors.feedWeight, 2).c_str(), true);

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
}
