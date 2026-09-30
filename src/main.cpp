#include <Arduino.h>
#include <WiFi.h>
#include <esp_idf_version.h>
#include <esp_task_wdt.h>

#include "app_config.h"
#include "system_state.h"
#include "sensor_manager.h"
#include "control_manager.h"
#include "rule_engine.h"
#include "mqtt_service.h"
#include "safety_manager.h"

static SystemState gState;
static unsigned long tSensor = 0, tRule = 0, tPub = 0, tWiFiRetry = 0;
static OutputLockReason lastReportedReason = OutputLockReason::NONE;

static bool networkConfigured() {
  return WIFI_SSID[0] != '\0';
}

static void startWiFiConnection() {
  if (!networkConfigured()) return;
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  tWiFiRetry = millis();
}

static void updateNetworkState(unsigned long now) {
  gState.wifiConnected = WiFi.status() == WL_CONNECTED;
  gState.mqttConnected = mqttConnected();
  if (gState.wifiConnected) {
    gState.networkLostAt = 0;
  } else if (gState.networkLostAt == 0) {
    gState.networkLostAt = now == 0 ? 1 : now;
  }
}

static void initWatchdog() {
#if ESP_IDF_VERSION_MAJOR >= 5
  esp_task_wdt_config_t config = {};
  config.timeout_ms = WATCHDOG_TIMEOUT_SECONDS * 1000;
  config.idle_core_mask = (1U << portNUM_PROCESSORS) - 1U;
  config.trigger_panic = true;
  if (esp_task_wdt_reconfigure(&config) == ESP_ERR_INVALID_STATE) {
    esp_task_wdt_init(&config);
  }
#else
  esp_task_wdt_init(WATCHDOG_TIMEOUT_SECONDS, true);
#endif
  esp_task_wdt_add(nullptr);
}

void setup() {
  Serial.begin(115200);

  controlInit();
  clearOutputs(gState.outputs);
  applyOutputs(gState);

  initWatchdog();

  sensorInit();
  sensorRead(gState.sensors);
  evaluateSafety(gState, millis());
  applyOutputs(gState);

  startWiFiConnection();
  mqttInit();
}

void loop() {
  const unsigned long now = millis();
  if (networkConfigured() && WiFi.status() != WL_CONNECTED &&
      now - tWiFiRetry >= WIFI_RECONNECT_INTERVAL) {
    WiFi.disconnect();
    startWiFiConnection();
  }

  mqttLoop(gState);
  updateNetworkState(now);

  if (now - tSensor >= SENSOR_READ_INTERVAL) {
    tSensor = now;
    sensorRead(gState.sensors);
    evaluateSafety(gState, now);
  }

  if (now - tRule >= RULE_ENGINE_INTERVAL) {
    tRule = now;
    evaluateSafety(gState, now);
    applyAutoRules(gState);
    applyOutputs(gState);
    if (gState.lockReason != lastReportedReason) {
      Serial.printf("Safety=%s outputs_locked=%s reason=%s\n",
                    safetyStateName(gState.safety),
                    gState.outputsLocked ? "true" : "false",
                    outputLockReasonName(gState.lockReason));
      lastReportedReason = gState.lockReason;
    }
  }

  if (now - tPub >= MQTT_PUBLISH_INTERVAL) {
    tPub = now;
    mqttPublishState(gState);
  }
  esp_task_wdt_reset();
}
