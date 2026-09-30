#ifndef SAFETY_MANAGER_H
#define SAFETY_MANAGER_H

#include <stdint.h>
#include "system_state.h"

void clearOutputs(OutputState& outputs);
bool sensorReadingFresh(const SensorReading& reading, uint32_t now);
void evaluateSafety(SystemState& state, uint32_t now);
void enforceOutputSafety(SystemState& state);
const char* safetyStateName(SafetyState state);
const char* sensorFaultName(SensorFault fault);
const char* outputLockReasonName(OutputLockReason reason);

#endif
