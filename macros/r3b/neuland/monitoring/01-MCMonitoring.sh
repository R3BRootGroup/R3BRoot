#!/bin/bash

# Create a folder for root files, so this directory stays clean
mkdir -p output

# Kill background jobs if script is terminated
trap 'kill $(jobs -pr) 2>/dev/null' SIGINT SIGTERM EXIT

# Run all simulations in "parallel" in background
root -l -q -b 'MCMonitoring.c(10000, "600AMeV.1n.500keV.14m", "output")' &
root -l -q -b 'MCMonitoring.c(10000, "600AMeV.2n.500keV.14m", "output")' &
root -l -q -b 'MCMonitoring.c(10000, "600AMeV.3n.500keV.14m", "output")' &
root -l -q -b 'MCMonitoring.c(10000, "600AMeV.4n.500keV.14m", "output")' &

# Wait for all background jobs to finish
wait
