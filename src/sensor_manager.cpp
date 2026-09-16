#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SHT31.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <HX711.h>

#include "pins.h"
#include "sensor_manager.h"

static Adafruit_SHT31 sht31 = Adafruit_SHT31();
static OneWire oneWire(PIN_ONEWIRE);
static DallasTemperature ds18b20(&oneWire);
static HX711 hx;

void sensorInit() {
  Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
  sht31.begin(0x44);
  ds18b20.begin();
  pinMode(PIN_WATER_LEVEL_LOW, INPUT);
  hx.begin(PIN_HX711_DOUT, PIN_HX711_SCK);
  hx.set_scale(420.0f);
  hx.tare();
}

void sensorRead(SensorData& data) {
  float t = sht31.readTemperature();
  float h = sht31.readHumidity();
  if (!isnan(t)) data.temperature = t;
  if (!isnan(h)) data.humidity = h;

  ds18b20.requestTemperatures();
  float waterT = ds18b20.getTempCByIndex(0);
  (void)waterT;
  data.waterLevel = (digitalRead(PIN_WATER_LEVEL_LOW) == HIGH) ? 100.0f : 20.0f;

  if (hx.is_ready()) {
    data.feedWeight = hx.get_units(5);
    if (data.feedWeight < 0) data.feedWeight = 0;
  }

  data.nh3 = 5.0f;
  data.co2 = 400.0f;
  data.h2s = 1.0f;
}
