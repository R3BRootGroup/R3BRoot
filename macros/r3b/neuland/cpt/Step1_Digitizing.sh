#!/bin/bash

# Kill background jobs if script is terminated
trap 'kill $(jobs -pr) 2>/dev/null' SIGINT SIGTERM EXIT

# Run all simulations in "parallel" in background
root -l -q -b 'Step1_Digitizing.c(10000, "neutron",  "output")' &
root -l -q -b 'Step1_Digitizing.c(10000, "proton",   "output")' &
root -l -q -b 'Step1_Digitizing.c(10000, "deuteron", "output")' &
root -l -q -b 'Step1_Digitizing.c(10000, "triton",   "output")' &
root -l -q -b 'Step1_Digitizing.c(10000, "He3",      "output")' &
root -l -q -b 'Step1_Digitizing.c(10000, "alpha",    "output")' &

# Wait for all background jobs to finish
wait
