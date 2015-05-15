// Wireless Sensor Network Routing Algorithms
// ==========================================
// Created By: Min Xu <xukmin@gmail.com>

#ifndef NETWORKING_ROUTING_BUILDERS_H_
#define NETWORKING_ROUTING_BUILDERS_H_

#include <string>

#include "sensor-network.h"

class ParentSelector;

class RoutingBuilder {
 public:
  // Takes ownership of selector.
  RoutingBuilder(const std::string& name,
                 const std::string& title,
                 ParentSelector* selector)
    : name_(name), title_(title), selector_(selector) {
  }

  virtual ~RoutingBuilder();

  const std::string& name() const { return name_; }

  const std::string& title() const { return title_; }

  void BuildRouting(SensorNetwork* network);

 private:
  const std::string name_;
  const std::string title_;
  ParentSelector* selector_;
};

class EarliestFirstRoutingBuilder : public RoutingBuilder {
 public:
  EarliestFirstRoutingBuilder();
};

class SecondEarliestFirstRoutingBuilder : public RoutingBuilder {
 public:
  SecondEarliestFirstRoutingBuilder();
};

class LatestFirstRoutingBuilder : public RoutingBuilder {
 public:
  LatestFirstRoutingBuilder();
};

class NearestFirstRoutingBuilder : public RoutingBuilder {
 public:
  NearestFirstRoutingBuilder();
};

class SecondNearestFirstRoutingBuilder : public RoutingBuilder {
 public:
  SecondNearestFirstRoutingBuilder();
};

class FarthestFirstRoutingBuilder : public RoutingBuilder {
 public:
  FarthestFirstRoutingBuilder();
};

class RandomizedRoutingBuilder : public RoutingBuilder {
 public:
  RandomizedRoutingBuilder();
};

class WeightedRandomizedRoutingBuilder : public RoutingBuilder {
 public:
  WeightedRandomizedRoutingBuilder();
};

#endif  // NETWORKING_ROUTING_BUILDERS_H_
