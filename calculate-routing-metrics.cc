// Wireless Sensor Network Routing Algorithms
// ==========================================
// Created By: Min Xu <xukmin@gmail.com>
//
// Main function for running simulation and calcuate routing metrics.
// Usage:
//     ./calculate-routing-metrics \
//         [<num_sensors>] \
//         [<repetitions>] \
//         [<lower_communication_range>] \
//         [<upper_communication_range>] \
//         [<communcation_range_step>]
//
// All arguments are optional. Simply run without any arguments to perform
// simulation with default configurations.

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>

#include "region.h"
#include "routing-metric-calculators.h"
#include "routing-builders.h"
#include "sensor-network.h"
#include "sensor-placers.h"
#include "svg-printer.h"
#include "utils.h"

struct SimulationOptions {
  int num_sensors;
  int times;
  Region region;
  double lower_communication_range;
  double upper_communication_range;
  double communication_range_step;
  std::vector<RoutingBuilder*> builders;
  std::vector<RoutingMetricCalculator*> calculators;
};

void PrintSimulationOptions(const SimulationOptions& options) {
  printf("SimulationOptions:\n");
  printf("num_sensors = %d\n", options.num_sensors);
  printf("times = %d\n", options.times);
  printf("region.min_x = %f\n", options.region.min_x);
  printf("region.min_y = %f\n", options.region.min_y);
  printf("region.max_x = %f\n", options.region.max_x);
  printf("region.max_y = %f\n", options.region.max_y);
  printf("lower_communication_range = %f\n", options.lower_communication_range);
  printf("upper_communication_range = %f\n", options.upper_communication_range);
  printf("communication_range_step = %f\n", options.communication_range_step);
  printf("\n");
  printf("Routing Building Algorithms:\n");
  for (int i = 0; i < options.builders.size(); i++) {
    printf("%s\n", options.builders[i]->name().c_str());
  }
  printf("\n");
  printf("Routing Metrics:\n");
  for (int i = 0; i < options.calculators.size(); i++) {
    printf("%s\n", options.calculators[i]->name().c_str());
  }
}

void BuildExampleRoutingNetworks(const SimulationOptions& options) {
  double scale = 600.0 / (options.region.max_y - options.region.min_y);

  RandomizedSensorPlacer placer(options.num_sensors, options.region);
  std::vector<Position> positions;
  GeneratePositionsThatCanBeConnected(
      options.lower_communication_range, &placer, &positions);

  SensorNetwork network;
  network.DeploySensors(positions, options.lower_communication_range);

  for (int i = 0; i < options.builders.size(); i++) {
    options.builders[i]->BuildRouting(&network);

    const std::string filename =
        "routings-" + IntToString(options.num_sensors) + "-" +
        options.builders[i]->name() + ".svg";
    SvgPrinter printer(filename, options.builders[i]->title(),
                       options.region, scale);
    printer.PrintNetwork(network);
  }
}

class RoutingMetrics {
 public:
  explicit RoutingMetrics(const SimulationOptions& options) : options_(options) {
  }

  void AddRange(double range) {
    data_[range].resize(options_.builders.size());
    for (int i = 0; i < options_.builders.size(); i++) {
      data_[range][i].resize(options_.calculators.size());
    }
  }

  void AddData(double range, int builder, int calculator, double metric) {
    assert(data_.find(range) != data_.end());
    std::pair<double, int>& p = data_[range][builder][calculator];
    p.first += metric;
    p.second++;
  }

  double GetData(double range, int builder, int calculator) const {
    const std::pair<double, int>& p = data_[range][builder][calculator];
    if (p.second > 0) {
      return p.first / p.second;
    } else {
      return 0.0;
    }
  }

 private:
  const SimulationOptions options_;

  // range -> builder index -> metric index ->
  //   (sum of metric values, num of metric values)
  mutable std::map<double, std::vector<std::vector<std::pair<double, int> > > >
      data_;
};

void CalculateMetrics(const SimulationOptions& options, RoutingMetrics* metrics) {
  RandomizedSensorPlacer placer(options.num_sensors, options.region);

  for (double range = options.lower_communication_range;
       range < options.upper_communication_range;
       range += options.communication_range_step) {
    if (int(range * 10) % 10 == 0) {
      printf("\nrange = %.1f", range);
    } else {
      printf(" %.1f", range);
    }

    metrics->AddRange(range);
    for (int i = 0; i < options.times; i++) {
      std::vector<Position> positions;
      GeneratePositionsThatCanBeConnected(range, &placer, &positions);

      SensorNetwork network;
      network.DeploySensors(positions, range);

      for (int b = 0; b < options.builders.size(); b++) {
        options.builders[b]->BuildRouting(&network);
        for (int c = 0; c < options.calculators.size(); c++) {
          double metric = options.calculators[c]->CalculateMetric(network);
          if (metric > 0.0) {
            metrics->AddData(range, b, c, metric);
          }
        }
      }
    }
  }
  printf("\n");
}

void SaveMetrics(const SimulationOptions& options,
                 const RoutingMetrics& metrics) {
  for (int c = 0; c < options.calculators.size(); c++) {
    const std::string filename =
        "metrics-" + IntToString(options.num_sensors) + "-" +
        options.calculators[c]->name() + ".dat";

    printf("Writing %s metrics to %s ...\n",
           options.calculators[c]->name().c_str(), filename.c_str());

    std::ofstream fs(filename.c_str());

    for (double range = options.lower_communication_range;
         range < options.upper_communication_range;
         range += options.communication_range_step) {
      fs << range;
      for (int b = 0; b < options.builders.size(); b++) {
        fs << " " << metrics.GetData(range, b, c);
      }
      fs << std::endl;
    }
  }
}

int main(int argc, char** argv) {
  std::srand(std::time(NULL));

  // Disable buffering of stdout.
  std::setbuf(stdout, NULL);

  SimulationOptions options;

  options.num_sensors = 100;
  options.times = 20;

  options.lower_communication_range = 25.0;
  options.upper_communication_range = 50.0;
  options.communication_range_step = 0.1;

  options.region.min_x = 0.0;
  options.region.min_y = 0.0;
  options.region.max_x = 100.0;
  options.region.max_y = 100.0;

  options.builders.push_back(new EarliestFirstRoutingBuilder());
  options.builders.push_back(new SecondEarliestFirstRoutingBuilder());
  options.builders.push_back(new LatestFirstRoutingBuilder());
  options.builders.push_back(new NearestFirstRoutingBuilder());
  options.builders.push_back(new SecondNearestFirstRoutingBuilder());
  options.builders.push_back(new FarthestFirstRoutingBuilder());
  options.builders.push_back(new RandomizedRoutingBuilder());
  options.builders.push_back(new WeightedRandomizedRoutingBuilder());

  options.calculators.push_back(new NodeDegreeVarianceCalculator());
  options.calculators.push_back(new RobustnessCalculator());
  options.calculators.push_back(new ChannelQualityCalculator());
  options.calculators.push_back(new DataAggregationCalculator());
  options.calculators.push_back(new LatencyCalculator());

  if (argc > 1) {
    options.num_sensors = std::atoi(argv[1]);
  }
  if (argc > 2) {
    options.times = std::atoi(argv[2]);
  }
  if (argc > 4) {
    options.lower_communication_range = std::atof(argv[3]);
    options.upper_communication_range = std::atof(argv[4]);
  }
  if (argc > 5) {
    options.communication_range_step = std::atof(argv[5]);
  }

  PrintSimulationOptions(options);

  // Build example routing networks and save as SVG images. These example
  // routing networks are only for demonstration purposes but not for the
  // following simulations.
  BuildExampleRoutingNetworks(options);

  RoutingMetrics metrics(options);
  CalculateMetrics(options, &metrics);
  SaveMetrics(options, metrics);

  for (int i = 0; i < options.calculators.size(); i++) {
    delete options.calculators[i];
  }

  for (int i = 0; i < options.builders.size(); i++) {
    delete options.builders[i];
  }
}

