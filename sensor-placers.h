// Wireless Sensor Network Routing Algorithms
// ==========================================
// Created By: Min Xu <xukmin@gmail.com>

#ifndef NETWORKING_SENSOR_PLACERS_H_
#define NETWORKING_SENSOR_PLACERS_H_

#include <cassert>
#include <map>
#include <set>
#include <vector>

#include "position.h"
#include "region.h"

class SensorPlacer {
 public:
  virtual ~SensorPlacer() {}
  virtual void GeneratePositions(std::vector<Position>* positions) = 0;
};

class RandomizedSensorPlacer : public SensorPlacer {
 public:
  RandomizedSensorPlacer(int num_sensors, const Region& region)
      : num_sensors_(num_sensors), region_(region) {
    assert(num_sensors > 0);
  }

  void GeneratePositions(std::vector<Position>* positions);

 private:
  const int num_sensors_;
  const Region region_;
};

class RegularSensorPlacer : public SensorPlacer {
 public:
  RegularSensorPlacer(int num_cols, int num_rows, const Region& region)
      : num_cols_(num_cols), num_rows_(num_rows), region_(region) {
    assert(num_cols > 0);
    assert(num_rows > 0);
  }

  void GeneratePositions(std::vector<Position>* positions);

 private:
  const int num_cols_;
  const int num_rows_;
  const Region region_;
};

void GeneratePositionsThatCanBeConnected(double communication_range,
                                         SensorPlacer* placer,
                                         std::vector<Position>* positions);

#endif  // NETWORKING_SENSOR_PLACERS_H_
