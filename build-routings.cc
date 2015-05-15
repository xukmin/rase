// Wireless Sensor Network Routing Algorithms
// ==========================================
// Created By: Min Xu <xukmin@gmail.com>
//
// Build example routing networks.

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <string>

#include "region.h"
#include "routing-builders.h"
#include "routing-metric-calculators.h"
#include "sensor-network.h"
#include "sensor-placers.h"
#include "svg-printer.h"
#include "utils.h"

int main(int argc, char** argv) {
  std::srand(std::time(NULL));

  int num_sensors = 100;
  double communication_range = 20.0;
  double scale = 6.0;

  if (argc > 1) {
    num_sensors = std::atoi(argv[1]);
  }
  if (argc > 2) {
    communication_range = std::atof(argv[2]);
  }

  Region region;
  region.min_x = 0.0;
  region.min_y = 0.0;
  region.max_x = 100.0;
  region.max_y = 100.0;

  std::vector<RoutingBuilder*> builders;
  builders.push_back(new EarliestFirstRoutingBuilder());
  builders.push_back(new SecondEarliestFirstRoutingBuilder());
  builders.push_back(new LatestFirstRoutingBuilder());
  builders.push_back(new NearestFirstRoutingBuilder());
  builders.push_back(new SecondNearestFirstRoutingBuilder());
  builders.push_back(new FarthestFirstRoutingBuilder());
  builders.push_back(new RandomizedRoutingBuilder());
  builders.push_back(new WeightedRandomizedRoutingBuilder());

  RandomizedSensorPlacer placer(num_sensors, region);
  std::vector<Position> positions;
  GeneratePositionsThatCanBeConnected(communication_range, &placer, &positions);

  SensorNetwork network;
  network.DeploySensors(positions, communication_range);

  for (int i = 0; i < builders.size(); i++) {
    builders[i]->BuildRouting(&network);

    const std::string filename = "routings-" + IntToString(num_sensors) + "-" +
                                 builders[i]->name() + ".svg";
    SvgPrinter printer(filename, builders[i]->title(), region, scale);
    printer.PrintNetwork(network);
  }

  for (int i = 0; i < builders.size(); i++) {
    delete builders[i];
  }
}

