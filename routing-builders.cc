// Wireless Sensor Network Routing Algorithms
// ==========================================
// Created By: Min Xu <xukmin@gmail.com>

#include "routing-builders.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iterator>
#include <map>
#include <queue>
#include <set>

#include "sensor-network.h"
#include "utils.h"

class ParentSelector {
 public:
  virtual ~ParentSelector() {}

  // Returns the selected parent.
  virtual int SelectParent(int sensor,
                           const std::vector<int>& candidates,
                           const SensorNetwork& network) = 0;
};

// We use a vector instead of a set to store parent candidates for each sensor
// because in some routing topology generating algorithms, the order in which a
// parent first reaches a sensor matters.
bool GenerateParentCandidates(SensorNetwork* network,
                              std::vector<std::vector<int> >* parents) {
  parents->clear();
  parents->resize(network->num_sensors());

  std::vector<bool> visited(network->num_sensors());
  int num_visited = 0;
  network->SetLevel(0, 0);
  std::queue<int> queue;
  visited[0] = true;
  num_visited++;
  queue.push(0);  // Start from the base station.
  while (!queue.empty()) {
    int current = queue.front();
    queue.pop();
    int level = network->GetLevel(current);
    const std::set<int>& neighbors = network->GetNeighbors(current);
    for (std::set<int>::const_iterator it = neighbors.begin();
         it != neighbors.end();
         ++it) {
      int neighbor = *it;
      int neighbor_level = network->GetLevel(neighbor);
      if (neighbor_level == -1 || neighbor_level > level) {
        if (neighbor_level == -1) {
          network->SetLevel(neighbor, level + 1);
        } else {
          assert(neighbor_level == level + 1);
        }
        std::vector<int>& neighbor_parents = (*parents)[neighbor];
        assert(std::find(neighbor_parents.begin(),
                         neighbor_parents.end(),
                         current) == neighbor_parents.end());
        neighbor_parents.push_back(current);
        if (!visited[neighbor]) {
          queue.push(neighbor);
          visited[neighbor] = true;
          num_visited++;
        }
      }
    }
  }

  return num_visited == network->num_sensors();
}

RoutingBuilder::~RoutingBuilder() {
  delete selector_;
}

void RoutingBuilder::BuildRouting(SensorNetwork* network) {
  network->RemoveParents();
  std::vector<std::vector<int> > candidates;
  bool succeeded = GenerateParentCandidates(network, &candidates);
  assert(succeeded);

  // Base station does not need to and cannot select parent.
  for (int i = 1; i < network->num_sensors(); i++) {
    network->SetParent(i, selector_->SelectParent(i, candidates[i], *network));
  }
  // Make sure the routing generator works as expected.
  assert(network->IsConnectedWithRoutings());
}

class EarliestFirstParentSelector : public ParentSelector {
 public:
  int SelectParent(int sensor,
                   const std::vector<int>& candidates,
                   const SensorNetwork& network);
};

int EarliestFirstParentSelector::SelectParent(int sensor,
                                              const std::vector<int>& candidates,
                                              const SensorNetwork& network) {
  assert(!candidates.empty());
  return candidates.front();
}

EarliestFirstRoutingBuilder::EarliestFirstRoutingBuilder()
    : RoutingBuilder("earliest-first",
                     "Earliest First Routing",
                     new EarliestFirstParentSelector()) {
}

class SecondEarliestFirstParentSelector : public ParentSelector {
 public:
  int SelectParent(int sensor,
                   const std::vector<int>& candidates,
                   const SensorNetwork& network);
};

int SecondEarliestFirstParentSelector::SelectParent(
    int sensor,
    const std::vector<int>& candidates,
    const SensorNetwork& network) {
  assert(!candidates.empty());
  if (candidates.size() == 1) {
    return candidates.front();
  } else {
    return candidates[1];
  }
}

SecondEarliestFirstRoutingBuilder::SecondEarliestFirstRoutingBuilder()
    : RoutingBuilder("second-earliest-first",
                     "Second Earliest First Routing",
                     new SecondEarliestFirstParentSelector()) {
}

class LatestFirstParentSelector : public ParentSelector {
 public:
  int SelectParent(int sensor,
                   const std::vector<int>& candidates,
                   const SensorNetwork& network);
};

int LatestFirstParentSelector::SelectParent(int sensor,
                                            const std::vector<int>& candidates,
                                            const SensorNetwork& network) {
  assert(!candidates.empty());
  return candidates.back();
}

LatestFirstRoutingBuilder::LatestFirstRoutingBuilder()
    : RoutingBuilder("latest-first",
                     "Latest First Routing",
                     new LatestFirstParentSelector()) {
}

class RandomizedParentSelector : public ParentSelector {
 public:
  int SelectParent(int sensor,
                   const std::vector<int>& candidates,
                   const SensorNetwork& network);
};

int RandomizedParentSelector::SelectParent(int sensor,
                                           const std::vector<int>& candidates,
                                           const SensorNetwork& network) {
  assert(!candidates.empty());
  return candidates[std::rand() % candidates.size()];
}

RandomizedRoutingBuilder::RandomizedRoutingBuilder()
    : RoutingBuilder("randomized",
                     "Randomized Routing",
                     new RandomizedParentSelector()) {
}

class NearestFirstParentSelector : public ParentSelector {
 public:
  int SelectParent(int sensor,
                   const std::vector<int>& candidates,
                   const SensorNetwork& network);
};

int NearestFirstParentSelector::SelectParent(int sensor,
                                             const std::vector<int>& candidates,
                                             const SensorNetwork& network) {
  assert(!candidates.empty());
  int parent = candidates[0];
  double min_distance = network.GetDistance(sensor, parent);
  for (int i = 1; i < candidates.size(); i++) {
    double distance = network.GetDistance(sensor, candidates[i]);
    if (distance < min_distance) {
      parent = candidates[i];
      min_distance = distance;
    }
  }

  return parent;
}

NearestFirstRoutingBuilder::NearestFirstRoutingBuilder()
    : RoutingBuilder("nearest-first",
                     "Nearest First Routing",
                     new NearestFirstParentSelector()) {
}

class SecondNearestFirstParentSelector : public ParentSelector {
 public:
  int SelectParent(int sensor,
                   const std::vector<int>& candidates,
                   const SensorNetwork& network);
};

int SecondNearestFirstParentSelector::SelectParent(
    int sensor,
    const std::vector<int>& candidates,
    const SensorNetwork& network) {
  assert(!candidates.empty());
  if (candidates.size() == 1) {
    return candidates[0];
  }

  int min_sensor = candidates[0];
  double min_distance = network.GetDistance(sensor, min_sensor);
  int second_min_sensor = candidates[1];
  double second_min_distance = network.GetDistance(sensor, second_min_sensor);
  if (min_distance > second_min_sensor) {
    std::swap(min_sensor, second_min_sensor);
    std::swap(min_distance, second_min_distance);
  }
  for (int i = 2; i < candidates.size(); i++) {
    double distance = network.GetDistance(sensor, candidates[i]);
    if (distance < min_distance) {
      second_min_sensor = min_sensor;
      second_min_distance = min_distance;
      min_sensor = candidates[i];
      min_distance = distance;
    } else if (distance < second_min_distance) {
      second_min_sensor = candidates[i];
      second_min_distance = distance;
    }
  }

  return second_min_sensor;
}

SecondNearestFirstRoutingBuilder::SecondNearestFirstRoutingBuilder()
    : RoutingBuilder("second-nearest-first",
                     "Second Nearest First Routing",
                     new SecondNearestFirstParentSelector()) {
}

class FarthestFirstParentSelector : public ParentSelector {
 public:
  int SelectParent(int sensor,
                   const std::vector<int>& candidates,
                   const SensorNetwork& network);
};

int FarthestFirstParentSelector::SelectParent(int sensor,
                                              const std::vector<int>& candidates,
                                              const SensorNetwork& network) {
  assert(!candidates.empty());
  int parent = candidates[0];
  double max_distance = network.GetDistance(sensor, parent);
  for (int i = 1; i < candidates.size(); i++) {
    double distance = network.GetDistance(sensor, candidates[i]);
    if (distance > max_distance) {
      parent = candidates[i];
      max_distance = distance;
    }
  }

  return parent;
}

FarthestFirstRoutingBuilder::FarthestFirstRoutingBuilder()
    : RoutingBuilder("farthest-first",
                     "Farthest First Routing",
                     new FarthestFirstParentSelector()) {
}

class WeightedRandomizedParentSelector : public ParentSelector {
 public:
  int SelectParent(int sensor,
                   const std::vector<int>& candidates,
                   const SensorNetwork& network);
};

int WeightedRandomizedParentSelector::SelectParent(
    int sensor,
    const std::vector<int>& candidates,
    const SensorNetwork& network) {
  assert(!candidates.empty());
  std::vector<double> weights(candidates.size());
  double total_weights = 0.0;
  for (int i = 0; i < candidates.size(); i++) {
    int num_neighbors = network.GetNeighbors(i).size();
    assert(num_neighbors > 0);
    weights[i] = 1.0 / num_neighbors;
    total_weights += weights[i];
  }

  double r = DoubleRand(0, total_weights);
  for (int i = 0; i < weights.size(); i++) {
    if (r < weights[i]) {
      return candidates[i];
    }
    r -= weights[i];
  }

  // Should never get here.
  assert(false);
  exit(1);
}

WeightedRandomizedRoutingBuilder::WeightedRandomizedRoutingBuilder()
    : RoutingBuilder("weighted-randomized",
                     "Weighted Randomized Routing",
                     new WeightedRandomizedParentSelector()) {
}

