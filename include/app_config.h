#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#define FW_VERSION "0.1.0"
#define FW_BUILD_DATE __DATE__
#define FW_DEVICE_ID "ESP32_LIVESTOCK_001"

#if __has_include("app_config_local.h")
#include "app_config_local.h"
#endif

#ifndef WIFI_SSID
#define WIFI_SSID ""
#endif
#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD ""
#endif
#define WIFI_CONNECT_TIMEOUT 30000

#ifndef MQTT_BROKER
#define MQTT_BROKER ""
#endif
#define MQTT_PORT 1883
#ifndef MQTT_USER
#define MQTT_USER ""
#endif
#ifndef MQTT_PASSWORD
#define MQTT_PASSWORD ""
#endif
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
#define MQTT_TOPIC_TELEMETRY_WATER_TEMP  MQTT_BASE "/sensor/water_temperature"
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
#define MQTT_TOPIC_SAFETY_STATE          MQTT_BASE "/safety/state"
#define MQTT_TOPIC_OUTPUT_LOCK           MQTT_BASE "/safety/output_lock"
#define MQTT_TOPIC_SENSOR_FAULTS         MQTT_BASE "/safety/sensor_faults"

#define DEVICE_NAME "Livestock-Controller-001"
#define DEVICE_LOCATION "Home Farm"

#define SENSOR_READ_INTERVAL 5000
#define MQTT_PUBLISH_INTERVAL 10000
#define RULE_ENGINE_INTERVAL 3000
#define SENSOR_STALE_TIMEOUT 15000
#define WIFI_RECONNECT_INTERVAL 30000
#define NETWORK_FAILURE_TIMEOUT 120000
#define WATCHDOG_TIMEOUT_SECONDS 8

#ifndef NETWORK_LOSS_LOCKS_OUTPUTS
#define NETWORK_LOSS_LOCKS_OUTPUTS false
#endif
#ifndef REQUIRE_SHT31
#define REQUIRE_SHT31 true
#endif
#ifndef REQUIRE_DS18B20
#define REQUIRE_DS18B20 true
#endif
#ifndef REQUIRE_WATER_LEVEL
#define REQUIRE_WATER_LEVEL true
#endif
#ifndef REQUIRE_HX711
#define REQUIRE_HX711 true
#endif
#ifndef REQUIRE_GAS_SENSORS
#define REQUIRE_GAS_SENSORS false
#endif

#define SENSOR_TEMP_MIN_C -20.0f
#define SENSOR_TEMP_MAX_C 70.0f
#define SENSOR_HUMIDITY_MIN 0.0f
#define SENSOR_HUMIDITY_MAX 100.0f
#define SENSOR_WATER_TEMP_MIN_C -10.0f
#define SENSOR_WATER_TEMP_MAX_C 60.0f
#define SENSOR_FEED_WEIGHT_MAX_KG 1000.0f

#define CRITICAL_TEMP_MIN_C 5.0f
#define CRITICAL_TEMP_MAX_C 40.0f
#define CRITICAL_HUMIDITY_MAX 95.0f
#define CRITICAL_WATER_TEMP_MIN_C 2.0f
#define CRITICAL_WATER_TEMP_MAX_C 40.0f

#define TEMP_HYSTERESIS_C 1.0f
#define HUMIDITY_HYSTERESIS 3.0f
#define WATER_LEVEL_HYSTERESIS 10.0f

#define DEFAULT_MODE "AUTO"

#define TEMP_ALERT_HIGH 32.0f
#define TEMP_ALERT_LOW  22.0f
#define HUMIDITY_ALERT_HIGH 80.0f
#define NH3_ALERT_HIGH 25.0f
#define FEED_ALERT_LOW_KG 2.0f

#endif // APP_CONFIG_H
