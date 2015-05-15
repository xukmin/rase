// Wireless Sensor Network Routing Algorithms
// ==========================================
// Created By: Min Xu <xukmin@gmail.com>

#include "sensor-placers.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iterator>
#include <map>
#include <queue>
#include <set>

#include "sensor-network.h"
#include "utils.h"

void RandomizedSensorPlacer::GeneratePositions(std::vector<Position>* positions) {
  assert(positions != NULL);
  positions->clear();

  // Put the base station at the origin.
  positions->push_back(Position(0.0, 0.0));

  for (int i = 1; i < num_sensors_; i++) {
    positions->push_back(Position(DoubleRand(region_.min_x, region_.max_x),
                                  DoubleRand(region_.min_y, region_.max_y)));
  }
}

void RegularSensorPlacer::GeneratePositions(std::vector<Position>* positions) {
  assert(positions != NULL);
  positions->clear();

  double gap_x = (region_.max_x - region_.min_x) / num_cols_;
  double gap_y = (region_.max_y - region_.min_y) / num_rows_;

  for (int i = 0; i < num_cols_; i++) {
    for (int j = 0; j < num_rows_; j++) {
      positions->push_back(Position(region_.min_x + i * gap_x,
                                    region_.min_y + j * gap_y));
    }
  }
}

void GeneratePositionsThatCanBeConnected(double communication_range,
                                         SensorPlacer* placer,
                                         std::vector<Position>* positions) {
  double min_communication_range;
  int retries = 10;
  while (retries-- > 0) {
    placer->GeneratePositions(positions);
    if (communication_range >= CalculateMinimumCommunicationRange(*positions)) {
      return;
    }
  }
  fprintf(stderr, "Failed to generate sensors with given communication range!\n");
  exit(1);
}

