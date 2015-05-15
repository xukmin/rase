#!/bin/bash
#
# Wireless Sensor Network Routing Algorithms
#
# Created By: Min Xu <xukmin@gmail.com>

for format in "svg" "png"; do
  gnuplot \
      -e "BASENAME = 'metrics-${1:-100}-latency'" \
      -e "YLABEL = 'Latency'" \
      -e "FORMAT = '${format}'" \
      metrics.gnuplot
done
