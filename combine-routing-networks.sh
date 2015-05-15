#!/bin/bash
#
# Wireless Sensor Network Routing Algorithms
# ==========================================
# Created By: Min Xu <xukmin@gmail.com>
#
# Combine multiple SVG routing graphs into one PNG image.
# Usage:
#     ./combine-routing-graphs.sh [<num_sensors>]
# <num_sensors> = 100 by default.

# convert "routings-${1:-100}-*.svg" +append "routings-${1:-100}.png"

convert \
    routings-${1:-100}-earliest-first.svg \
    routings-${1:-100}-second-earliest-first.svg \
    routings-${1:-100}-farthest-first.svg \
    routings-${1:-100}-randomized.svg \
    +append routings-${1:-100}-top.png

convert \
    routings-${1:-100}-nearest-first.svg \
    routings-${1:-100}-second-nearest-first.svg \
    routings-${1:-100}-farthest-first.svg \
    routings-${1:-100}-weighted-randomized.svg \
    +append routings-${1:-100}-bottom.png

convert \
    routings-${1:-100}-top.png \
    routings-${1:-100}-bottom.png \
    -append routings-${1:-100}.png
