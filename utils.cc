// Wireless Sensor Network Routing Algorithms
// ==========================================
// Created By: Min Xu <xukmin@gmail.com>

#include "utils.h"

#include <cstdlib>
#include <sstream>

double DoubleRand(double min, double max) {
  return min + (((double) std::rand()) / RAND_MAX) * (max - min);
}

std::string IntToString(int n) {
  std::ostringstream os;
  os << n;
  return os.str();
}

