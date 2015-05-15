#!/bin/bash
#
# Wireless Sensor Network Routing Algorithms
#
# Created By: Min Xu <xukmin@gmail.com>

for format in "svg" "png"; do
  gnuplot \
      -e "BASENAME = 'metrics-${1:-100}-robustness'" \
      -e "YLABEL = 'Connectivity (%) after 1 Failed Sensor'" \
      -e "FORMAT = '${format}'" \
      metrics.gnuplot
done
