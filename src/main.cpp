#include <Arduino.h>
#include <WiFi.h>

#include "app_config.h"
#include "system_state.h"
#include "sensor_manager.h"
#include "control_manager.h"
#include "rule_engine.h"
#include "mqtt_service.h"

static SystemState gState;
static unsigned long tSensor = 0, tRule = 0, tPub = 0;

static void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < WIFI_CONNECT_TIMEOUT) {
    delay(300);
  }
}

void setup() {
  Serial.begin(115200);
  delay(300);

  connectWiFi();
  sensorInit();
  controlInit();
  mqttInit();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) connectWiFi();

  mqttLoop(gState);

  if (millis() - tSensor >= SENSOR_READ_INTERVAL) {
    tSensor = millis();
    sensorRead(gState.sensors);
  }

  if (millis() - tRule >= RULE_ENGINE_INTERVAL) {
    tRule = millis();
    applyAutoRules(gState);
    applyOutputs(gState.outputs);
  }

  if (millis() - tPub >= MQTT_PUBLISH_INTERVAL) {
    tPub = millis();
    mqttPublishState(gState);
  }
}
