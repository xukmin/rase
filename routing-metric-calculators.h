// Wireless Sensor Network Routing Algorithms
// ==========================================
// Created By: Min Xu <xukmin@gmail.com>

#ifndef NETWORKING_ROUTING_METRIC_CALCULATORS_H_
#define NETWORKING_ROUTING_METRIC_CALCULATORS_H_

#include <string>

#include "sensor-network.h"

class RoutingMetricCalculator {
 public:
  explicit RoutingMetricCalculator(const std::string& name) : name_(name) {}
  virtual ~RoutingMetricCalculator() {}

  const std::string& name() const { return name_; }

  virtual double CalculateMetric(const SensorNetwork& network) = 0;

 private:
  const std::string name_;
};

class NodeDegreeVarianceCalculator : public RoutingMetricCalculator {
 public:
  NodeDegreeVarianceCalculator()
      : RoutingMetricCalculator("node-degree-variance") {}

  double CalculateMetric(const SensorNetwork& network);
};

class RobustnessCalculator : public RoutingMetricCalculator {
 public:
  RobustnessCalculator() : RoutingMetricCalculator("robustness") {}

  double CalculateMetric(const SensorNetwork& network);
};

class ChannelQualityCalculator : public RoutingMetricCalculator {
 public:
  ChannelQualityCalculator() : RoutingMetricCalculator("channel-quality") {}

  double CalculateMetric(const SensorNetwork& network);
};

class DataAggregationCalculator : public RoutingMetricCalculator {
 public:
  DataAggregationCalculator() : RoutingMetricCalculator("data-aggregation") {}

  double CalculateMetric(const SensorNetwork& network);
};

class LatencyCalculator : public RoutingMetricCalculator {
 public:
  LatencyCalculator() : RoutingMetricCalculator("latency") {}

  double CalculateMetric(const SensorNetwork& network);
};

#endif  // NETWORKING_ROUTING_METRIC_CALCULATORS_H_
