# Production-readiness checklist

This checklist is a deployment gate, not a certification. Record test dates,
equipment identifiers, results, failures, corrective actions, and responsible
reviewers. Do not introduce livestock until every applicable item passes.

## Configuration and security

- [ ] Rotate every Wi-Fi or MQTT credential previously committed to Git.
- [ ] Provision unique least-privilege credentials in the ignored
      `include/app_config_local.h`; confirm no secrets are tracked.
- [ ] Isolate plaintext MQTT on a controlled network or validate
      certificate-authenticated TLS.
- [ ] Review required-sensor, critical-threshold, hysteresis, and network-loss
      policy for this enclosure and species.
- [ ] Restrict physical and remote access to manual controls.

## Hardware and calibration

- [ ] Verify GPIO assignments, active relay level, fusing, isolation, grounding,
      flyback protection, and enclosure ingress/fire protection.
- [ ] Calibrate SHT31, DS18B20, water-level, and HX711 channels against traceable
      references over the intended operating range.
- [ ] Add and calibrate real supported gas sensors before using gas telemetry
      for safety decisions; confirm unavailable values are never interpreted as
      zero or normal.
- [ ] Test each relay first without a load, then with its rated real load.
- [ ] Confirm all outputs are OFF during boot, flashing, watchdog reset,
      brownout, power loss, and restoration.
- [ ] Independently verify emergency ventilation, water, fire, and temperature
      protections; do not rely on this controller as the sole life-safety layer.

## Fault and recovery tests

- [ ] Disconnect each sensor separately and in combination; verify `FAULT`,
      diagnostic publication, global output lock, and physical relay OFF.
- [ ] Inject invalid, out-of-range, frozen/stale, and critical readings.
- [ ] Interrupt and restore Wi-Fi and MQTT for longer than the configured
      timeout; verify the documented autonomous/network-lock policy.
- [ ] Exercise manual override and SAFE mode; verify no ON command bypasses the
      output lock.
- [ ] Exercise the physical emergency stop and verify restart requires the
      approved operating procedure.
- [ ] Confirm relay hysteresis prevents unacceptable chatter around thresholds.
- [ ] Verify recovery from sensor reconnection does not energize unexpected
      equipment.

## Staged validation

- [ ] Complete a continuous 24–72 hour no-livestock/empty-enclosure test with
      production wiring and loads.
- [ ] Review logs for resets, stale data, faults, reconnect behavior, output
      transitions, thermal conditions, and power quality.
- [ ] Perform power-loss/reboot tests at multiple points in every control cycle.
- [ ] Obtain electrical, mechanical, animal-welfare, and operational review and
      sign-off applicable to the deployment jurisdiction.
- [ ] Conduct a limited, continuously supervised rollout with an operator able
      to remove power and move animals immediately.
- [ ] Define inspection, calibration, credential rotation, incident response,
      rollback, and maintenance schedules.

Passing software tests alone does not satisfy this checklist or establish field
readiness.
