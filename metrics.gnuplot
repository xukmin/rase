# Wireless Sensor Network Routing Algorithms
#
# Created By: Min Xu <xukmin@gmail.com>
#
# Required variables: BASENAME YLABEL

if (!exists("XLABEL")) XLABEL = "Communication Range"
if (!exists("INPUT")) INPUT = BASENAME . ".dat"
if (!exists("FORMAT")) FORMAT = "svg"
if (!exists("OUTPUT")) OUTPUT = BASENAME . "." . FORMAT

set terminal FORMAT
set output OUTPUT
set xlabel XLABEL
set ylabel YLABEL
set key center bmargin horizontal
plot INPUT using 1:2 with linespoints title "Earliest First", \
     INPUT using 1:3 with linespoints title "Second Earliest First", \
     INPUT using 1:4 with linespoints title "Latest First", \
     INPUT using 1:5 with linespoints title "Nearest First", \
     INPUT using 1:6 with linespoints title "Second Nearest First", \
     INPUT using 1:7 with linespoints title "Farthest First", \
     INPUT using 1:8 with linespoints title "Randomized", \
     INPUT using 1:9 with linespoints title "Weighted Randomized"
