#!/bin/bash

# Kill background jobs if script is terminated
trap 'kill $(jobs -pr) 2>/dev/null' SIGINT SIGTERM EXIT

# Run all simulations in "parallel" in background
root -l -q -b 'Step2_Monitoring.c(1000, "neutron",  "output")' &
root -l -q -b 'Step2_Monitoring.c(1000, "proton",   "output")' &
root -l -q -b 'Step2_Monitoring.c(1000, "deuteron", "output")' &
root -l -q -b 'Step2_Monitoring.c(1000, "triton",   "output")' &
root -l -q -b 'Step2_Monitoring.c(1000, "He3",      "output")' &
root -l -q -b 'Step2_Monitoring.c(1000, "alpha",    "output")' &

# Wait for all background jobs to finish
wait
