// Wireless Sensor Network Routing Algorithms
// ==========================================
// Created By: Min Xu <xukmin@gmail.com>

#ifndef NETWORKING_SVG_PRINTER_H_
#define NETWORKING_SVG_PRINTER_H_

#include <cstdio>
#include <string>

#include "region.h"
#include "sensor-network.h"

class SvgPrinter {
 public:
  SvgPrinter(const std::string& filename,
             const std::string& title,
             const Region& region,
             double scale = 1.0);

  ~SvgPrinter();

  void PrintNetwork(const SensorNetwork& network,
                    bool print_channels = true,
                    bool print_routings = true);

  void PrintChannels(const SensorNetwork& network);

  void PrintRoutings(const SensorNetwork& network);

 private:
  const std::string filename_;
  const std::string title_;
  const Region region_;
  const double scale_;

  FILE* file_;
};

#endif  // NETWORKING_SVG_PRINTER_H_
