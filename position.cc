// Wireless Sensor Network Routing Algorithms
// ==========================================
// Created By: Min Xu <xukmin@gmail.com>

#include "position.h"

#include <cmath>

bool operator<(const Position& lhs, const Position& rhs) {
  if (lhs.y != rhs.y) {
    return lhs.y < rhs.y;
  } else {
    return lhs.x < rhs.x;
  }
}

double Distance(const Position& lhs, const Position& rhs) {
  double dx = lhs.x - rhs.x;
  double dy = lhs.y - rhs.y;
  return std::sqrt(dx * dx + dy * dy);
}

