RASE - Routing Algorithm Simulation & Evaluation framework for Wireless Sensor Networks
==========================================

Author: Min Xu <xukmin at gmail.com>

Overview
--------
RASE is a simulator and evaluator for Wireless Sensor Network routing algorithms.

8 localized routing algorithms, 5 network structure & routing performance analyzers, and a sensor network simulator are implemented.

Simulation & Evaluation Outputs
-------------------------------

### Routing Trees

Routing Tree with 100 Sensors

<img src="https://lh6.googleusercontent.com/-zEHEMgwhIgo/VVWyDRccUtI/AAAAAAAADzg/j41kVgHOSAY/w1896-h1010-no/routings-100.png" width="640">

Routing Tree with 200 Sensors

<img src="https://lh6.googleusercontent.com/-qt27OyVyhIU/VVWyDT88EMI/AAAAAAAADzg/AnkJdcynmGc/w1896-h1010-no/routings-200.png" width="640">

Routing Tree with 300 Sensors

<img src="https://lh6.googleusercontent.com/-1ueSJX8voS0/VVWyDfH7NbI/AAAAAAAADzg/NhHUwJKhSDc/w1896-h1010-no/routings-300.png" width="640">

Routing Tree with 400 Sensors

<img src="https://lh6.googleusercontent.com/-hpW1Z6IkxPU/VVWyDULkSTI/AAAAAAAADzg/C1j9gX_etLU/w1896-h1010-no/routings-400.png" width="640">

### Metrics

Node Degree Variance

<img src="https://lh4.googleusercontent.com/-X_zVxQKGMUw/VVWyDXEYtHI/AAAAAAAADzg/21LhC0AXAXg/w1280-h960-no/metrics-400-node-degree-variance.png" width="640">

Robustness

<img src="https://lh6.googleusercontent.com/-i9QK0NXlp2M/VVWyDWkTxaI/AAAAAAAADzg/DTtldC-IaDA/w1280-h960-no/metrics-400-robustness.png" width="640">

Channel Quality

<img src="https://lh4.googleusercontent.com/-z261-AioxG4/VVWyDXi9kPI/AAAAAAAADzg/-TLFvq4M0Fs/w1280-h960-no/metrics-400-channel-quality.png" width="640">

Data Aggregation

<img src="https://lh4.googleusercontent.com/-p2tL5vv4220/VVWyDVnrnfI/AAAAAAAADzg/sPKqR0pDtMQ/w1280-h960-no/metrics-400-data-aggregation.png" width="640">

Latency

<img src="https://lh4.googleusercontent.com/-2M9-OuBbNXA/VVWyDdEX_MI/AAAAAAAADzg/RabIQPVJ64k/w1280-h960-no/metrics-400-latency.png" width="640">

Usage
-----

Build:

    make

Run the simulation, generate data file and PNG diagrams:

    make run

Run the simulation with customized configurations:

    ./run.sh <num_sensors> <repetitions> <lower_range> <upper_range> <range_step>

For example:

    ./run.sh 100 20 25.0 50.0 1

All parameters are optional. The default values are

    <num_sensors> = 100
    <repetitions> = 20
    <lower_range> = 25.0
    <upper_range> = 50.0
    <range_step> = 0.1

The generated files are:

- metrics-<num_sensors>-*.dat Data files in plain text format.
- metrics-<num_sensors>-*.png PNG images of the routing metric diagrams.
- routings-<num_sensors>*.png PNG images of the sample routing networks.

Dependencies
------------

- gnuplot 5.0 or above is needed to correctly generate PNG images.

- imagemagick is needed for converting SVG to PNG.

File Structures
---------------
build-routings.cc
  Build sample routing networks.

calculate-routing-metrics.cc
  Main function of localized routing topology generation simulation.

position.h
position.cc
  Position struct.

region.h
  Region struct.

routing-builders.h
routing-builders.cc
  A set of localized routing topology generation algorithms.

routing-metric-calculators.h
routing-metric-calculators.cc
  A set of routing metric calculation algorithms.

sensor-network.h
sensor-network.cc
  The SensorNetwork class.

sensor-placers.h
sensor-placers.cc
  The SensorPlacer class and 2 subclasses.

sensor.h
sensor.cc
  The Sensor class.

svg-printer.h
svg-printer.cc
  The SvgPrinter class, to print a SensorNetwork to SVG graph.

utils.h
utils.cc
  Some utility functions.

Simulation Methodology
----------------------

Pseudocode for the simulation procedure:

    for communication_range in [lower_range, upper_range); do
      repeat N times; do
        Generate a random set of n sensor positions, ensure its
            min_communcation_range <= communication_range;
        Create network and add channels based on communication_range;
        foreach routing_algorithm; do
          Build routing_topology;
          foreach routing_metric; do
            Calculate metric value and save it into a map of 
                [communication_range, routing_algorithm, routing_metric] =>
                    value;
          done
        done
      done
    done

