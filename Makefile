# Wireless Sensor Network Routing Algorithms
#
# Created By: Min Xu <xukmin@gmail.com>

CXX = g++

CXXFLAGS =

BINS = build-routings calculate-routing-metrics
OBJS = *.o
DATA = *.dat
SVGS = *.svg
PNGS = *.png

all: $(BINS)

clean:
	$(RM) $(BINS) $(OBJS) $(DATA) $(SVGS) $(PNGS)

run: run-calculate-routing-metrics

run-build-routings: build-routings
	./build-routings
	./combine-routing-graphs.sh

run-calculate-routing-metrics: calculate-routing-metrics
	./calculate-routing-metrics
	./draw.sh

build-routings: build-routings.o sensor-placers.o sensor-network.o \
    position.o routing-builders.o sensor.o svg-printer.o utils.o
	$(CXX) -o $@ $^ $(CXXFLAGS)

build-routings.o: build-routings.cc region.h routing-builders.h \
    sensor-network.h svg-printer.h
	$(CXX) -c $< $(CXXFLAGS)

calculate-routing-metrics: calculate-routing-metrics.o sensor-placers.o \
    sensor-network.o position.o routing-builders.o svg-printer.o utils.o \
    routing-metric-calculators.o sensor.o utils.o
	$(CXX) -o $@ $^ $(CXXFLAGS)

calculate-routing-metrics.o: calculate-routing-metrics.cc region.h \
    routing-builders.h sensor-network.h svg-printer.h utils.h
	$(CXX) -c $< $(CXXFLAGS)

position.o: position.cc position.h
	$(CXX) -c $< $(CXXFLAGS)

routing-builders.o: routing-builders.cc routing-builders.h utils.h
	$(CXX) -c $< $(CXXFLAGS)

routing-metric-calculators.o: routing-metric-calculators.cc \
    routing-metric-calculators.h position.h region.h sensor-network.h
	$(CXX) -c $< $(CXXFLAGS)

sensor.o: sensor.cc sensor.h position.h
	$(CXX) -c $< $(CXXFLAGS)

sensor-placers.o: sensor-placers.cc sensor-placers.h position.h region.h \
    sensor-network.h utils.h
	$(CXX) -c $< $(CXXFLAGS)

svg-printer.o: svg-printer.cc svg-printer.h position.h
	$(CXX) -c $< $(CXXFLAGS)

sensor-network.o: sensor-network.cc sensor-network.h position.h region.h \
    sensor.h utils.h
	$(CXX) -c $< $(CXXFLAGS)

utils.o: utils.cc utils.h
	$(CXX) -c $< $(CXXFLAGS)

.PHONY: all clean run run-build-routings run-calculate-routing-metrics

