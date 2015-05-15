// Wireless Sensor Network Routing Algorithms
// ==========================================
// Created By: Min Xu <xukmin@gmail.com>

#include "sensor.h"

bool operator<(const Sensor& lhs, const Sensor& rhs) {
  return lhs.position() < rhs.position();
}

