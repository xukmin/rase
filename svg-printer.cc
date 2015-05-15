// Wireless Sensor Network Routing Algorithms
// ==========================================
// Created By: Min Xu <xukmin@gmail.com>

#include "svg-printer.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iterator>
#include <map>
#include <queue>
#include <set>

#include "position.h"
#include "region.h"
#include "sensor-network.h"

class PositionConverter {
 public:
  PositionConverter(double scale, const Position& origin)
      : scale_(scale), origin_(origin) {
  }

  double ConvertX(double x) {
    return x * scale_ + origin_.x;
  }

  double ConvertY(double y) {
    return y * scale_ + origin_.y;
  }

  Position Convert(const Position& p) {
    return Position(ConvertX(p.x), ConvertY(p.y));
  }

 private:
  const double scale_;
  const Position origin_;
};

SvgPrinter::SvgPrinter(const std::string& filename,
                       const std::string& title,
                       const Region& region,
                       double scale)
    : filename_(filename), title_(title), region_(region), scale_(scale) {
  file_ = std::fopen(filename_.c_str(), "w");
  if (!file_) {
    fprintf(stderr, "Failed to open file %s!\n", filename_.c_str());
    exit(1);
  }
}

SvgPrinter::~SvgPrinter() {
  std::fclose(file_);
}

void SvgPrinter::PrintNetwork(const SensorNetwork& network,
                              bool print_channels,
                              bool print_routings) {
  const static int RADIUS = 8;
  const static int STROKE_WIDTH = 1;
  const static int FONT_HEIGHT = 20;
  PositionConverter c(scale_, Position(RADIUS + STROKE_WIDTH,
                                       RADIUS + STROKE_WIDTH));

  int w = int((region_.max_x - region_.min_x) * scale_ +
              (RADIUS + STROKE_WIDTH) * 2);
  int h = int((region_.max_y - region_.min_y) * scale_ +
              (RADIUS + STROKE_WIDTH) * 2);
  fprintf(file_,
          "<svg width='%d' height='%d'"
          " xmlns='http://www.w3.org/2000/svg'"
          " xmlns:xlink='http://www.w3.org/1999/xlink'>\n"
          "<rect x='0' y='0' width='%d' height='%d'"
          " style='fill:white;stroke:black;stroke-width:1;' />\n",
          w, h + FONT_HEIGHT * 2, w, h);

  for (int i = 0; i < network.num_sensors(); i++) {
    const Position& p = network.GetPosition(i);
    if (print_channels) {
      const std::set<int>& neighbors = network.GetNeighbors(i);
      for (std::set<int>::const_iterator neighbor = neighbors.begin();
           neighbor != neighbors.end();
           ++neighbor) {
        if (i < *neighbor) {
          const Position& q = network.GetPosition(*neighbor);
          fprintf(file_,
                  "<path d='M %f %f L %f %f' stroke='gray' stroke-width='1'"
                   " stroke-dasharray='1,1' />\n",
                   c.ConvertX(p.x), c.ConvertY(p.y),
                   c.ConvertX(q.x), c.ConvertY(q.y));
        }
      }
    }
    if (print_routings) {
      int parent = network.GetParent(i);
      if (parent != -1) {
        const Position& q = network.GetPosition(parent);
        fprintf(file_,
                "<line x1='%f' y1='%f' x2='%f' y2='%f' "
                " style='stroke:royalblue;stroke-width:2' />\n",
                c.ConvertX(p.x), c.ConvertY(p.y),
                c.ConvertX(q.x), c.ConvertY(q.y));
      }
    }
  }

  if (network.num_sensors() > 0) {
    fprintf(file_,
            "<circle cx='%f' cy='%f' r='%d' stroke='yellow' stroke-width='%d'"
            " fill='red' />\n",
            c.ConvertX(network.GetPosition(0).x),
            c.ConvertY(network.GetPosition(0).y),
            RADIUS, STROKE_WIDTH);
  }
  for (int i = 1; i < network.num_sensors(); i++) {
    const Position& p = network.GetPosition(i);
    fprintf(file_,
            "<circle cx='%f' cy='%f' r='%d' stroke='green' stroke-width='%d'"
            " fill='yellow' />\n",
            c.ConvertX(p.x), c.ConvertY(p.y), RADIUS, STROKE_WIDTH);
  }

  fprintf(file_,
          "<text x='%d' y='%d' fill='royalblue' text-anchor='middle' "
          "alignment-baseline='middle' font-size='%dpx'>%s</text>\n",
          w / 2, h + FONT_HEIGHT, FONT_HEIGHT, title_.c_str());
  fprintf(file_, "</svg>\n");
}

void SvgPrinter::PrintChannels(const SensorNetwork& network) {
  PrintNetwork(network, true, false);
}

void SvgPrinter::PrintRoutings(const SensorNetwork& network) {
  PrintNetwork(network, false, true);
}

