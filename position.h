// Wireless Sensor Network Routing Algorithms
// ==========================================
// Created By: Min Xu <xukmin@gmail.com>

#ifndef NETWORKING_POSITION_H_
#define NETWORKING_POSITION_H_

struct Position {
  Position(double x, double y) : x(x), y(y) {}

  double x;
  double y;
};

double Distance(const Position&, const Position&);

bool operator<(const Position&, const Position&);

#endif  // NETWORKING_POSITION_H_
