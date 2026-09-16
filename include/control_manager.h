#ifndef CONTROL_MANAGER_H
#define CONTROL_MANAGER_H

#include "system_state.h"

void controlInit();
void applyOutputs(const OutputState& out);

#endif
