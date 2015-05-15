// Wireless Sensor Network Routing Algorithms
// ==========================================
// Created By: Min Xu <xukmin@gmail.com>

#include "routing-metric-calculators.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <functional>
#include <iostream>
#include <map>
#include <numeric>
#include <set>
#include <vector>

#include "position.h"
#include "region.h"
#include "sensor-network.h"

namespace {

double BitAccuracyRate(double distance,
                       double communication_range,
                       double noise) {
  // Early versions of GCC does not recognize std::erfc().
  return 1 - 0.5 * erfc(std::sqrt(
      1.0 / (pow(distance / communication_range, 4) * noise)));
}

}  // namespace

double NodeDegreeVarianceCalculator::CalculateMetric(
    const SensorNetwork& network) {
  assert(network.num_sensors() > 0);
  assert(network.IsConnectedWithRoutings());

  std::vector<int> degrees(network.num_sensors());
  int sum = 0;
  // Base station (sensor #0) does not have a parent, so we start at sensor #1.
  for (int i = 1; i < degrees.size(); i++) {
    int parent = network.GetParent(i);
    assert(parent >= 0);
    degrees[parent]++;
    sum++;
  }

  double average = sum / degrees.size();
  double variance = 0.0;
  for (int i = 0; i < degrees.size(); i++) {
    int difference = (degrees[i] - average);
    variance += difference * difference;
  }
  variance /= degrees.size();

  return variance;
}

struct SensorUsage {
  int sensor;
  int usage;
};

bool operator>(const SensorUsage& lhs, const SensorUsage& rhs) {
  return lhs.usage > rhs.usage;
}

// Sort sensors by their usage in descending order.
// NOTE that the base station (sensor #0) will always be the first in
// sorted_sensors. You may be interested in the second sensor in the list for
// many purposes.
void SortSensorsByUsage(const SensorNetwork& network,
                        std::vector<int>* sorted_sensors) {
  std::vector<SensorUsage> usages(network.num_sensors());
  for (int i = 0; i < usages.size(); i++) {
    usages[i].sensor = i;
    usages[i].usage = 1;  // Every sensor is used by itself.
  }

  for (int i = 0; i < network.num_sensors(); i++) {
    int parent = network.GetParent(i);
    while (parent >= 0) {
      usages[parent].usage++;
      parent = network.GetParent(parent);
    }
  }

  std::sort(usages.begin(), usages.end(), std::greater<SensorUsage>());

  sorted_sensors->resize(usages.size());
  for (int i = 0; i < usages.size(); i++) {
    (*sorted_sensors)[i] = usages[i].sensor;
  }
}

// Assume the most used sensor (except the base station) is failed and removed
// from the network, what is the percentage of the remaining sensors which are
// still connected to the base station.
double RobustnessCalculator::CalculateMetric(const SensorNetwork& network) {
  if (network.num_sensors() < 3) {
    return 0.0;
  }

  std::vector<int> sorted;
  SortSensorsByUsage(network, &sorted);

  // Assume the most frequently used sensors (except the base station) is failed.
  std::set<int> disconnected;
  int num_failed = 1;  // TODO: support more than one failed sensors.
  for (int i = 1; i <= num_failed; i++) {
    disconnected.insert(sorted[i]);
  }

  std::set<int> connected;
  connected.insert(0);  // The base station is always connected (with itself).

  for (int i = 1; i < network.num_sensors(); i++) {
    std::set<int> parents;
    int parent = i;
    while (disconnected.find(parent) == disconnected.end() &&
           connected.find(parent) == connected.end()) {
      parents.insert(parent);
      parent = network.GetParent(parent);
      assert(parent >= 0);
    }
    if (disconnected.find(parent) != disconnected.end()) {
      disconnected.insert(parents.begin(), parents.end());
    } else {
      connected.insert(parents.begin(), parents.end());
    }
  }
  return connected.size() * 1.0 / (network.num_sensors() - num_failed);
}

double ChannelQualityCalculator::CalculateMetric(const SensorNetwork& network) {
  // Suppose the Bit Error Rate at the communication range is 1e-3, i.e
  // 0.5 * erfc(sqrt(1 / noise)) = 1e-3
  static const double noise = 0.209434;
  static const double sensing_range = 15.0;

  std::vector<int> triggered;
  Position position(50.0, 50.0);  // FIXME: pass in region to the calculator.
  if (!network.FindSensorsWithinRange(position, sensing_range, &triggered)) {
    // No sensor will ever be triggered by the event.
    return 0.0;
  }

  // Link Error Rates
  std::vector<double> lers(triggered.size());
  for (int i = 0; i < triggered.size(); i++) {
    int sensor = triggered[i];
    double distance = Distance(position, network.GetPosition(sensor));
    // Link Accuracy Rate
    double lar = BitAccuracyRate(distance, network.communication_range(), noise);
    int parent = network.GetParent(sensor);
    while (parent >= 0) {
      double distance = Distance(network.GetPosition(sensor),
                                 network.GetPosition(parent));
      lar *= BitAccuracyRate(distance, network.communication_range(), noise);
      parent = network.GetParent(parent);
    }
    lers[i] = 1.0 - lar;
  }

  return std::accumulate(lers.begin(), lers.end(), 0.0) / lers.size();
}

double DataAggregationCalculator::CalculateMetric(const SensorNetwork& network) {
  // TODO: Make it an argument.
  static const double sensing_range = 15.0;
  std::vector<int> triggered;
  Position position(50.0, 50.0);  // FIXME: pass in region to the calculator.
  if (!network.FindSensorsWithinRange(position, sensing_range, &triggered)) {
    // No sensor will ever be triggered by the event.
    return 0.0;
  }

  std::set<int> visited;
  visited.insert(0);

  int num_transmissions = 1;
  for (int i = 0; i < triggered.size(); i++) {
    visited.insert(triggered[i]);
    num_transmissions++;
    int parent = network.GetParent(triggered[i]);
    while (visited.find(parent) == visited.end()) {
      visited.insert(parent);
      num_transmissions++;
      parent = network.GetParent(parent);
      assert(parent >= 0);
    }
  }

  return num_transmissions;
}

double LatencyCalculator::CalculateMetric(const SensorNetwork& network) {
  // TODO: Make sensing_range and position arguments.
  const double sensing_range = 15.0;
  const Position position(50.0, 50.0);

  std::vector<int> triggered;
  if (!network.FindSensorsWithinRange(position, sensing_range, &triggered)) {
    // No sensor will ever be triggered by the event.
    return 0.0;
  }

  std::vector<bool> active(network.num_sensors());
  for (int i = 0; i < triggered.size(); i++) {
    int parent = triggered[i];
    while (parent >= 0 && !active[parent]) {
      active[parent] = true;
      parent = network.GetParent(parent);
    }
  }

  std::vector<int> children(network.num_sensors());
  // Skip the base station which does not have a parent.
  for (int i = 1; i < network.num_sensors(); i++) {
    assert(network.GetParent(i) >= 0);
    if (active[i]) {
      children[network.GetParent(i)]++;
    }
  }

  std::vector<int> timestamps(network.num_sensors());
  std::vector<bool> visited(network.num_sensors());
  int num_to_visit = network.num_sensors();
  // Ignore the inactive sensors by marking them as visited.
  for (int i = 0; i < network.num_sensors(); i++) {
    visited[i] = !active[i];
    num_to_visit -= visited[i];
  }
  while (num_to_visit > 0) {
    std::vector<int> leaves;
    for (int i = 0; i < network.num_sensors(); i++) {
      if (!visited[i] && children[i] == 0) {
        int parent = network.GetParent(i);
        if (parent >= 0) {
          assert(active[parent]);
          timestamps[parent] = std::max(timestamps[parent], timestamps[i]) + 1;
        }
        leaves.push_back(i);
        visited[i] = true;
        num_to_visit--;
      }
    }
    for (int i = 0; i < leaves.size(); i++) {
      int parent = network.GetParent(leaves[i]);
      if (parent >= 0) {
        children[parent]--;
      }
    }
  }

  return timestamps[0];
}
