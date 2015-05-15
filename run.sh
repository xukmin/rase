#!/bin/bash
#
# Wireless Sensor Network Routing Algorithms
# ==========================================
# Created By: Min Xu <xukmin@gmail.com>
#
# Run everything for routing topology generation and metrics calculation.
# Draw diagrams from generated data.

make all
./calculate-routing-metrics "${@}"
./draw.sh "${1}"

