#include "livestock_profile.h"

LivestockProfile getProfile(LivestockType type) {
  switch (type) {
    case LivestockType::CHICKEN: return {22, 30, 75, 25, 2.0f};
    case LivestockType::DUCK:    return {20, 29, 80, 25, 2.0f};
    case LivestockType::QUAIL:   return {22, 28, 75, 20, 1.0f};
    case LivestockType::PIG:     return {20, 28, 80, 20, 5.0f};
    case LivestockType::COW:     return {18, 27, 85, 20, 10.0f};
    case LivestockType::GOAT:    return {18, 30, 75, 20, 3.0f};
    case LivestockType::RABBIT:  return {18, 26, 70, 15, 1.0f};
    default:                     return {22, 30, 75, 25, 2.0f};
  }
}

const char* profileToString(LivestockType type) {
  switch (type) {
    case LivestockType::CHICKEN: return "chicken";
    case LivestockType::DUCK: return "duck";
    case LivestockType::QUAIL: return "quail";
    case LivestockType::PIG: return "pig";
    case LivestockType::COW: return "cow";
    case LivestockType::GOAT: return "goat";
    case LivestockType::RABBIT: return "rabbit";
    default: return "chicken";
  }
}

LivestockType profileFromString(const String& s) {
  String x = s; x.toLowerCase();
  if (x == "duck") return LivestockType::DUCK;
  if (x == "quail") return LivestockType::QUAIL;
  if (x == "pig") return LivestockType::PIG;
  if (x == "cow") return LivestockType::COW;
  if (x == "goat") return LivestockType::GOAT;
  if (x == "rabbit") return LivestockType::RABBIT;
  return LivestockType::CHICKEN;
}
