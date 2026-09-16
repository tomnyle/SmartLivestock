#ifndef LIVESTOCK_PROFILE_H
#define LIVESTOCK_PROFILE_H

#include "system_state.h"

struct LivestockProfile {
  float minTemp;
  float maxTemp;
  float maxHumidity;
  float nh3Limit;
  float feedLowKg;
};

LivestockProfile getProfile(LivestockType type);
const char* profileToString(LivestockType type);
LivestockType profileFromString(const String& s);

#endif // LIVESTOCK_PROFILE_H
