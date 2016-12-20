#!/bin/bash

# Set output directory from script arguments or use default
if [ -n "$1" ]
then
    OUTDIR=$1
else
    OUTDIR=output
fi
# Create a folder for root files, so this directory stays clean
mkdir -p ${OUTDIR}

# Kill background jobs if script is terminated
trap 'kill $(jobs -pr) 2>/dev/null' SIGINT SIGTERM EXIT

# Run all simulations in "parallel" in background
#root -l -q -b 'Step0_R3BNeulandSim.c(2112,       1000, 1.22, "neutron",  "output")' 1> /dev/null &
#root -l -q -b 'Step0_R3BNeulandSim.c(2212,       1000, 1.22, "proton",   "output")' &
#root -l -q -b 'Step0_R3BNeulandSim.c(1000010020, 1000, 1.61, "deuteron", "output")' 1> /dev/null &
#root -l -q -b 'Step0_R3BNeulandSim.c(1000010030, 1000, 1.93, "triton",   "output")' 1> /dev/null &
#root -l -q -b 'Step0_R3BNeulandSim.c(1000020030, 1000, 1.93, "He3",      "output")' 1> /dev/null &
#root -l -q -b 'Step0_R3BNeulandSim.c(1000020040, 1000, 2.20, "alpha",    "output")' 1> /dev/null &

root -l -q -b 'Step0_R3BNeulandSim.c(2112,       10000, 1*1.22, "neutron",  "'${OUTDIR}'")' 1> /dev/null &
root -l -q -b 'Step0_R3BNeulandSim.c(2212,       10000, 1*1.22, "proton",   "'${OUTDIR}'")' 1> /dev/null &
root -l -q -b 'Step0_R3BNeulandSim.c(1000010020, 10000, 2*1.22, "deuteron", "'${OUTDIR}'")' 1> /dev/null &
root -l -q -b 'Step0_R3BNeulandSim.c(1000010030, 10000, 3*1.22, "triton",   "'${OUTDIR}'")' 1> /dev/null &
root -l -q -b 'Step0_R3BNeulandSim.c(1000020030, 10000, 3*1.22, "He3",      "'${OUTDIR}'")' 1> /dev/null &
root -l -q -b 'Step0_R3BNeulandSim.c(1000020040, 10000, 4*1.22, "alpha",    "'${OUTDIR}'")' 1> /dev/null &

# Wait for all background jobs to finish
wait

# Remove junk
rm -f calor.out
rm -f flukaerr.dat
rm -f gphysi.dat
