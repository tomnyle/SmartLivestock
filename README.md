# SmartLivestock

ESP32 livestock-enclosure controller using PlatformIO, MQTT, and Home Assistant.

> **Status: production-readiness candidate, not certified production software.**
> Source changes and successful tests do not prove that the controller, sensors,
> relays, wiring, loads, enclosure, or operating procedures are safe for animals.
> Complete the documented hardware and supervised field validation before
> introducing livestock.

## Safety behavior

- Every relay is driven to its configured OFF level before sensor, Wi-Fi, or
  MQTT initialization.
- The controller boots with a global output lock in `SAFE`.
- Missing, invalid, out-of-range, or stale required sensors move the controller
  to `FAULT`, lock all outputs, and command every relay OFF.
- Critical air temperature, humidity, or water-temperature limits also lock all
  outputs OFF.
- Automatic rules run only in `RUNNING` with the output lock released and use
  hysteresis to reduce relay chatter.
- SHT31, DS18B20, water-level, and HX711 readings carry validity, availability,
  freshness, and fault state. DS18B20 water temperature is published.
- NH3, CO2, and H2S hardware is not implemented. These topics publish
  `unavailable`; no fabricated measurement is used by automation.
- Wi-Fi and MQTT reconnect attempts are bounded. Network loss is diagnostic by
  default so safe autonomous control continues. Set
  `NETWORK_LOSS_LOCKS_OUTPUTS` to `true` in the local configuration when the
  deployment safety case requires a network interlock.
- An ESP32 task watchdog resets the controller if the main task stops running.
  Reset returns all outputs to the boot-safe OFF state.

The controller publishes the existing MQTT topics unchanged. It additionally
publishes water temperature and retained diagnostics under
`smartfarm/livestock/safety/*`.

## Local configuration

Tracked files contain no deployment credentials. Copy the example locally:

```sh
cp include/app_config.example.h include/app_config_local.h
```

Edit `include/app_config_local.h`; it is ignored by Git. Empty defaults disable
network connection attempts. Never commit this local file. Credentials that
were ever committed to repository history must be rotated at the Wi-Fi access
point and MQTT broker; deleting them from the current source does not remove
them from Git history.

MQTT currently uses plaintext TCP. Deploy it only on an isolated, access-
controlled network, or add and validate certificate-authenticated TLS before
using an untrusted network.

## Build and test

```sh
g++ -std=c++11 -Iinclude src/safety_manager.cpp \
  test/test_safety/test_main.cpp -o /tmp/smartlivestock-safety-tests
/tmp/smartlivestock-safety-tests
pio run -e esp32dev
```

The native tests exercise boot safety, valid operation, required-sensor
failures, stale readings, critical thresholds, the global output lock, and safe
fallback without requiring hardware.

## Configuration policy

Required sensors and validated ranges are defined in `include/app_config.h`.
Defaults require SHT31, DS18B20, water-level, and HX711 telemetry. Gas sensors
remain optional because no supported driver exists; changing
`REQUIRE_GAS_SENSORS` to `true` intentionally holds the controller in `FAULT`
until real validated gas readings are implemented.

Thresholds are starting safeguards only. A qualified operator must set and
validate thresholds for the species, age, enclosure, climate, sensor placement,
and connected equipment.

## Mandatory deployment validation

See [Production-readiness checklist](docs/production-readiness-checklist.md).
Until every applicable item has documented evidence and sign-off, use this
firmware only for bench or empty-enclosure testing.
