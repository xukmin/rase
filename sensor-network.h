// Wireless Sensor Network Routing Algorithms
// ==========================================
// Created By: Min Xu <xukmin@gmail.com>

#ifndef NETWORKING_SENSOR_NETWORK_H_
#define NETWORKING_SENSOR_NETWORK_H_

#include <map>
#include <set>
#include <vector>

#include "position.h"
#include "region.h"
#include "sensor.h"

class SensorNetwork {
 public:
  SensorNetwork() : communication_range_(0.0) {}

  int num_sensors() const {
    return sensors_.size();
  }

  const std::vector<Sensor>& sensors() const {
    return sensors_;
  }

  const Position& GetPosition(int sensor) const {
    return sensors_[sensor].position();
  }

  const std::set<int>& GetNeighbors(int sensor) const {
    return sensors_[sensor].neighbors();
  }

  double communication_range() const {
    return communication_range_;
  }

  // Checks if the sensor network is fully connected with routings.
  bool IsConnectedWithRoutings() const;

  // Returns whether the sensors are fully connected with channels.
  bool DeploySensors(const std::vector<Position>& positions,
                     double communication_range);

  // Returns whether there are sensors within range from the given position.
  bool FindSensorsWithinRange(const Position& position,
                              double range,
                              std::vector<int>* neighbors) const;

  // Returns whether there are sensors within range from the given sensor ID.
  bool FindSensorsWithinRange(int sensor,
                              double range,
                              std::vector<int>* neighbors) const;

  int GetLevel(int sensor) const {
    return sensors_[sensor].level();
  }

  void SetLevel(int sensor, int level) {
    sensors_[sensor].set_level(level);
  }

  int GetParent(int sensor) const {
    return sensors_[sensor].parent();
  }

  void SetParent(int sensor, int parent) {
    sensors_[sensor].set_parent(parent);
  }

  void RemoveParents();

  double GetDistance(int s, int t) const {
    return distances_[s][t];
  }

 private:
  void AddSensor(const Position& p);

  void AddSensors(const std::vector<Position>& positions);

  void RemoveSensors();

  // Connects the sensors with communication channels given a communication
  // range. The existing channels among sensors will be all cleared before
  // creating new ones.
  void CreateChannels(double range);

  // Removes all communication channels among sensors in the network.
  void RemoveChannels();

  // Checks if the sensor network is fully connected with communication channels.
  bool IsConnectedWithChannels() const;

  // Find sensors located within the (2 * range) by (2 * range) square area
  // centered by the given position.
  void FindNeighborCandidates(const Position& position,
                              double range,
                              std::vector<int>* candidates) const;

  typedef std::multimap<double, int> SensorMap;

  // all sensors with their index in the vector as their IDs.
  std::vector<Sensor> sensors_;

  // sensor ids sorted by their x position.
  SensorMap x_map_;

  // sensor ids sorted by their y position.
  SensorMap y_map_;

  // distance matrix for all sensors.
  std::vector<std::vector<double> > distances_;

  double communication_range_;
};

double CalculateMinimumCommunicationRange(const std::vector<Position>& positions);

#endif  // NETWORKING_SENSOR_NETWORK_H_
