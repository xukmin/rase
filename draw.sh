#!/bin/bash
#
# Wireless Sensor Network Routing Algorithms
# ==========================================
# Created By: Min Xu <xukmin@gmail.com>
#
# Draw routing networks and routing metrics diagrams.
# Usage:
#     ./draw.sh [<num_sensors>]
# <num_sensors> = 100 by default.

./combine-routing-networks.sh "${@}"

gnuplot_version="$(gnuplot --version)"

if [[ "${gnuplot_version}" < "gnuplot 5.0" ]]; then
  echo "Please install Gnuplot 5.0 or above to generate PNG images.";
  exit
fi

./draw-metrics-node-degree-variance.sh "${@}"
./draw-metrics-robustness.sh "${@}"
./draw-metrics-channel-quality.sh "${@}"
./draw-metrics-data-aggregation.sh "${@}"
./draw-metrics-latency.sh "${@}"

# Open all generated images if we are running on Mac.
if [[ $(uname) == "Darwin" ]]; then
  open routings-${1:-100}.png metrics-${1:-100}-*.png
fi
