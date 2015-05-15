// Wireless Sensor Network Routing Algorithms
// ==========================================
// Created By: Min Xu <xukmin@gmail.com>

#include "sensor-network.h"

#include <algorithm>
#include <cassert>
#include <cfloat>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <iterator>
#include <map>
#include <queue>
#include <set>

#include "svg-printer.h"
#include "utils.h"

namespace {

void CalculateDistances(const std::vector<Position>& positions,
                        std::vector<std::vector<double> >* distances) {
  distances->clear();
  distances->resize(positions.size());
  for (int i = 0; i < distances->size(); i++) {
    (*distances)[i].resize(positions.size());
  }

  for (int i = 0; i < positions.size(); i++) {
    for (int j = i + 1; j < positions.size(); j++) {
      (*distances)[i][j] = (*distances)[j][i] =
          Distance(positions[i], positions[j]);
    }
  }
}

}  // namespace

void SensorNetwork::AddSensor(const Position& p) {
  x_map_.insert(std::pair<double, int>(p.x, sensors_.size()));
  y_map_.insert(std::pair<double, int>(p.y, sensors_.size()));
  sensors_.push_back(Sensor(p));
}

void SensorNetwork::RemoveChannels() {
  for (int i = 0; i < sensors_.size(); i++) {
    sensors_[i].RemoveNeighbors();
  }
}

void SensorNetwork::FindNeighborCandidates(const Position& position,
                                           double range,
                                           std::vector<int>* candidates) const {
  assert(candidates != NULL);
  candidates->clear();

  std::set<int> x_neighbors;
  {
    SensorMap::const_iterator lower = x_map_.lower_bound(position.x - range);
    SensorMap::const_iterator upper = x_map_.upper_bound(position.x + range);
    for (SensorMap::const_iterator it = lower; it != upper; ++it) {
      x_neighbors.insert(it->second);
    }
  }

  std::set<int> y_neighbors;
  {
    SensorMap::const_iterator lower = y_map_.lower_bound(position.y - range);
    SensorMap::const_iterator upper = y_map_.upper_bound(position.y + range);
    for (SensorMap::const_iterator it = lower; it != upper; ++it) {
      y_neighbors.insert(it->second);
    }
  }

  std::set_intersection(x_neighbors.begin(), x_neighbors.end(),
                        y_neighbors.begin(), y_neighbors.end(),
                        std::back_inserter(*candidates));
}

bool SensorNetwork::FindSensorsWithinRange(const Position& position,
                                           double range,
                                           std::vector<int>* neighbors) const {
  assert(neighbors != NULL);
  neighbors->clear();

  std::vector<int> candidates;
  FindNeighborCandidates(position, range, &candidates);

  for (int i = 0; i < candidates.size(); i++) {
    int c = candidates[i];
    if (Distance(position, sensors_[c].position()) <= range) {
      neighbors->push_back(c);
    }
  }

  return !neighbors->empty();
}

bool SensorNetwork::FindSensorsWithinRange(int sensor,
                                           double range,
                                           std::vector<int>* neighbors) const {
  assert(neighbors != NULL);
  neighbors->clear();

  std::vector<int> candidates;
  FindNeighborCandidates(sensors_[sensor].position(), range, &candidates);

  for (int i = 0; i < candidates.size(); i++) {
    int c = candidates[i];
    if (distances_[sensor][c] <= range) {
      neighbors->push_back(c);
    }
  }

  return !neighbors->empty();
}

void SensorNetwork::CreateChannels(double communication_range) {
  RemoveChannels();

  communication_range_ = communication_range;
  for (int i = 0; i < sensors_.size(); i++) {
    Sensor& sensor = sensors_[i];
    const Position& p = sensor.position();

    std::vector<int> neighbors;
    FindSensorsWithinRange(i, communication_range, &neighbors);
    for (int j = 0; j < neighbors.size(); j++) {
      if (neighbors[j] != i) {
        sensor.AddNeighbor(neighbors[j]);
      }
    }
  }
}

void SensorNetwork::AddSensors(const std::vector<Position>& positions) {
  RemoveSensors();
  for (int i = 0; i < positions.size(); i++) {
    AddSensor(positions[i]);
  }

  CalculateDistances(positions, &distances_);
}

void SensorNetwork::RemoveSensors() {
  sensors_.clear();
  x_map_.clear();
  y_map_.clear();
}

bool SensorNetwork::DeploySensors(const std::vector<Position>& positions,
                                  double communication_range) {
  AddSensors(positions);
  CreateChannels(communication_range);
  return IsConnectedWithChannels();
}

void SensorNetwork::RemoveParents() {
  for (int i = 0; i < sensors_.size(); i++) {
    sensors_[i].clear_parent();
  }
}

bool SensorNetwork::IsConnectedWithChannels() const {
  assert(num_sensors() > 0);

  std::vector<bool> visited(num_sensors());
  int num_visited = 0;

  std::queue<int> queue;
  queue.push(0);  // Start from the base station.
  while (!queue.empty()) {
    int current = queue.front();
    queue.pop();
    const std::set<int>& neighbors = GetNeighbors(current);
    for (std::set<int>::const_iterator neighbor = neighbors.begin();
         neighbor != neighbors.end();
         ++neighbor) {
      if (!visited[*neighbor]) {
        queue.push(*neighbor);
        visited[*neighbor] = true;
        num_visited++;
      }
    }
  }

  return num_visited == num_sensors();
}

// Checks if the sensor network is fully connected with routings.
// It is done by checking if the base station can be reached from every sensor.
bool SensorNetwork::IsConnectedWithRoutings() const {
  assert(num_sensors() > 0);

  std::vector<bool> visited(num_sensors());
  int num_connected = 0;

  visited[0] = true;
  num_connected++;

  for (int i = 1; i < num_sensors(); i++) {
    int parent = GetParent(i);
    while (parent > 0 && !visited[parent]) {
      visited[parent] = true;
      parent = GetParent(parent);
    }
    if (parent < 0) {
      return false;
    }
    num_connected++;
  }

  return num_connected == num_sensors();
}

double CalculateMinimumCommunicationRange(
    const std::vector<Position>& positions) {
  if (positions.size() <= 1) {
    return 0.0;
  }

  // Matrix of distances between sensors.
  std::vector<std::vector<double> > distances;
  CalculateDistances(positions, &distances);

  double min_range = 0.0;

  std::set<int> connected;
  connected.insert(0);

  std::set<int> unconnected;
  for (int i = 1; i < positions.size(); i++) {
    unconnected.insert(i);
  }

  for (int i = 1; i < positions.size(); i++) {
    int nearest = -1;
    double min_distance = DBL_MAX;
    for (std::set<int>::const_iterator c = connected.begin();
         c != connected.end(); ++c) {
      for (std::set<int>::const_iterator u = unconnected.begin();
           u != unconnected.end(); ++u) {
        if (distances[*c][*u] < min_distance) {
          min_distance = distances[*c][*u];
          nearest = *u;
        }
      }
    }
    assert(nearest > 0);
    unconnected.erase(nearest);
    connected.insert(nearest);
    min_range = std::max(min_range, min_distance);
  }

  return min_range;
}

