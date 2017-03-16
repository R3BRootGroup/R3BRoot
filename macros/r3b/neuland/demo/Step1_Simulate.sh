#!/bin/bash

# Set output directory from script arguments or use default
OUTDIR=${1:-output}
# Create a folder for root files, so this directory stays clean
mkdir -p ${OUTDIR}

nEvents=1000

# Kill background jobs if script is terminated
trap 'kill $(jobs -pr) 2>/dev/null' SIGINT SIGTERM EXIT

# Run all simulations in "parallel" in background
root -l -q -b 'Step1_Simulate.C(2112, 1, 0.73, '${nEvents}', "'${OUTDIR}'", "neutron-X1-250MeV")' &
#root -l -q -b 'Step1_Simulate.C(2112, 1, 0.47, '${nEvents}', "'${OUTDIR}'", "neutron-X1-110MeV")' 1> /dev/null &  
#root -l -q -b 'Step1_Simulate.C(2112, 2, 0.73, '${nEvents}', "'${OUTDIR}'", "neutronX2")' 1> /dev/null &
#root -l -q -b 'Step1_Simulate.C(2112, 3, 0.73, '${nEvents}', "'${OUTDIR}'", "neutronX3")' 1> /dev/null &
#root -l -q -b 'Step1_Simulate.C(2112, 4, 0.73, '${nEvents}', "'${OUTDIR}'", "neutronX4")' 1> /dev/null &

# Wait for all background jobs to finish
wait

# Remove junk
rm -f calor.out
rm -f flukaerr.dat
rm -f gphysi.dat
