#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include <Arduino.h>

#define FW_VERSION "0.1.0"
#define FW_BUILD_DATE __DATE__
#define FW_DEVICE_ID "ESP32_LIVESTOCK_001"

#define WIFI_SSID "Le Danh"
#define WIFI_PASSWORD "123456789"
#define WIFI_CONNECT_TIMEOUT 30000

#define MQTT_BROKER "192.168.100.168"
#define MQTT_PORT 1883
#define MQTT_USER "homer"
#define MQTT_PASSWORD "Danh@@@1992"
#define MQTT_CLIENT_ID "ESP32_LIVESTOCK"
#define MQTT_RECONNECT_INTERVAL 5000

#define HA_DISCOVERY_PREFIX "homeassistant"
#define HA_DISCOVERY_ENABLED true

#define MQTT_BASE "smartfarm/livestock"

#define MQTT_TOPIC_TELEMETRY_TEMP        MQTT_BASE "/sensor/temp"
#define MQTT_TOPIC_TELEMETRY_HUMIDITY    MQTT_BASE "/sensor/humidity"
#define MQTT_TOPIC_TELEMETRY_NH3         MQTT_BASE "/sensor/nh3"
#define MQTT_TOPIC_TELEMETRY_CO2         MQTT_BASE "/sensor/co2"
#define MQTT_TOPIC_TELEMETRY_H2S         MQTT_BASE "/sensor/h2s"
#define MQTT_TOPIC_TELEMETRY_WATER_LEVEL MQTT_BASE "/sensor/water_level"
#define MQTT_TOPIC_TELEMETRY_FEED_WEIGHT MQTT_BASE "/sensor/feed_weight"

#define MQTT_TOPIC_STATE_FAN             MQTT_BASE "/output/fan"
#define MQTT_TOPIC_STATE_EXHAUST         MQTT_BASE "/output/exhaust"
#define MQTT_TOPIC_STATE_HEATER          MQTT_BASE "/output/heater"
#define MQTT_TOPIC_STATE_PUMP            MQTT_BASE "/output/pump"
#define MQTT_TOPIC_STATE_FEEDER          MQTT_BASE "/output/feeder"
#define MQTT_TOPIC_STATE_LIGHT           MQTT_BASE "/output/light"
#define MQTT_TOPIC_STATE_MIST            MQTT_BASE "/output/mist"

#define MQTT_TOPIC_CMD_FAN               MQTT_BASE "/control/fan/set"
#define MQTT_TOPIC_CMD_EXHAUST           MQTT_BASE "/control/exhaust/set"
#define MQTT_TOPIC_CMD_HEATER            MQTT_BASE "/control/heater/set"
#define MQTT_TOPIC_CMD_PUMP              MQTT_BASE "/control/pump/set"
#define MQTT_TOPIC_CMD_FEEDER            MQTT_BASE "/control/feeder/set"
#define MQTT_TOPIC_CMD_LIGHT             MQTT_BASE "/control/light/set"
#define MQTT_TOPIC_CMD_MIST              MQTT_BASE "/control/mist/set"
#define MQTT_TOPIC_CMD_MODE              MQTT_BASE "/config/mode/set"
#define MQTT_TOPIC_CMD_PROFILE           MQTT_BASE "/config/profile/set"

#define MQTT_TOPIC_MODE_STATE            MQTT_BASE "/config/mode/state"
#define MQTT_TOPIC_PROFILE_STATE         MQTT_BASE "/config/profile/state"

#define MQTT_TOPIC_STATUS                MQTT_BASE "/status"
#define MQTT_TOPIC_CONTROLLER_STATE      MQTT_BASE "/controller/state"
#define MQTT_TOPIC_ALARM                 MQTT_BASE "/alarm"

#define DEVICE_NAME "Livestock-Controller-001"
#define DEVICE_LOCATION "Home Farm"

#define SENSOR_READ_INTERVAL 5000
#define MQTT_PUBLISH_INTERVAL 10000
#define RULE_ENGINE_INTERVAL 3000

#define DEFAULT_MODE "AUTO"

#define TEMP_ALERT_HIGH 32.0f
#define TEMP_ALERT_LOW  22.0f
#define HUMIDITY_ALERT_HIGH 80.0f
#define NH3_ALERT_HIGH 25.0f
#define FEED_ALERT_LOW_KG 2.0f

#endif // APP_CONFIG_H
