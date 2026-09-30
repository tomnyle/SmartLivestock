#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SHT31.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <HX711.h>

#include "pins.h"
#include "app_config.h"
#include "sensor_manager.h"

static Adafruit_SHT31 sht31 = Adafruit_SHT31();
static OneWire oneWire(PIN_ONEWIRE);
static DallasTemperature ds18b20(&oneWire);
static HX711 hx;
static bool shtAvailable = false;
static bool waterTemperatureAvailable = false;

static void updateReading(SensorReading& reading, float value, float minimum,
                          float maximum, unsigned long now, bool available = true) {
  reading.available = available;
  if (!available) {
    reading.valid = false;
    reading.fault = SensorFault::UNAVAILABLE;
    return;
  }
  if (isnan(value) || isinf(value)) {
    reading.valid = false;
    reading.fault = SensorFault::INVALID;
    return;
  }
  if (value < minimum || value > maximum) {
    reading.valid = false;
    reading.fault = SensorFault::OUT_OF_RANGE;
    return;
  }
  reading.value = value;
  reading.valid = true;
  reading.updatedAt = now;
  reading.fault = SensorFault::NONE;
}

void sensorInit() {
  Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
  shtAvailable = sht31.begin(0x44);
  ds18b20.begin();
  waterTemperatureAvailable = ds18b20.getDeviceCount() > 0;
  pinMode(PIN_WATER_LEVEL_LOW, INPUT);
  hx.begin(PIN_HX711_DOUT, PIN_HX711_SCK);
  hx.set_scale(420.0f);
  if (hx.is_ready()) hx.tare();
}

void sensorRead(SensorData& data) {
  const unsigned long now = millis();
  updateReading(data.temperature, shtAvailable ? sht31.readTemperature() : 0.0f,
                SENSOR_TEMP_MIN_C, SENSOR_TEMP_MAX_C, now, shtAvailable);
  updateReading(data.humidity, shtAvailable ? sht31.readHumidity() : 0.0f,
                SENSOR_HUMIDITY_MIN, SENSOR_HUMIDITY_MAX, now, shtAvailable);

  ds18b20.requestTemperatures();
  const float waterT = ds18b20.getTempCByIndex(0);
  updateReading(data.waterTemperature, waterT, SENSOR_WATER_TEMP_MIN_C,
                SENSOR_WATER_TEMP_MAX_C, now,
                waterTemperatureAvailable && waterT != DEVICE_DISCONNECTED_C);
  updateReading(data.waterLevel,
                (digitalRead(PIN_WATER_LEVEL_LOW) == HIGH) ? 100.0f : 20.0f,
                0.0f, 100.0f, now);

  if (hx.is_ready()) {
    updateReading(data.feedWeight, hx.get_units(5), 0.0f,
                  SENSOR_FEED_WEIGHT_MAX_KG, now);
  } else {
    updateReading(data.feedWeight, 0.0f, 0.0f,
                  SENSOR_FEED_WEIGHT_MAX_KG, now, false);
  }

  updateReading(data.nh3, 0.0f, 0.0f, 0.0f, now, false);
  updateReading(data.co2, 0.0f, 0.0f, 0.0f, now, false);
  updateReading(data.h2s, 0.0f, 0.0f, 0.0f, now, false);
}
