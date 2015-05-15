// Wireless Sensor Network Routing Algorithms
// ==========================================
// Created By: Min Xu <xukmin@gmail.com>

#ifndef NETWORKING_SENSOR_H_
#define NETWORKING_SENSOR_H_

#include <map>
#include <set>
#include <vector>

#include "position.h"

class Sensor {
 public:
  explicit Sensor(const Position& c)
      : position_(c), level_(-1), parent_(-1) {
  }

  const Position& position() const {
    return position_;
  }

  const std::set<int>& neighbors() const {
    return neighbors_;
  }

  void AddNeighbor(int neighbor) {
    neighbors_.insert(neighbor);
  }

  void RemoveNeighbors() {
    neighbors_.clear();
  }

  int parent() const {
    return parent_;
  }

  void set_parent(int parent) {
    parent_ = parent;
  }

  void clear_parent() {
    parent_ = -1;
  }

  int level() const {
    return level_;
  }

  void set_level(int level) {
    level_ = level;
  }

 private:
  Position position_;

  std::set<int> neighbors_;

  int level_;

  // Parent sensor for routing.
  int parent_;
};

bool operator<(const Sensor&, const Sensor&);

#endif  // NETWORKING_SENSOR_H_
