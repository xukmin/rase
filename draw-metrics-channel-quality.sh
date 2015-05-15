#!/bin/bash
#
# Wireless Sensor Network Routing Algorithms
#
# Created By: Min Xu <xukmin@gmail.com>

for format in "svg" "png"; do
  gnuplot \
      -e "BASENAME = 'metrics-${1:-100}-channel-quality'" \
      -e "YLABEL = 'Average Link Error Rate (%) for Sample Event'" \
      -e "FORMAT = '${format}'" \
      metrics.gnuplot
done
