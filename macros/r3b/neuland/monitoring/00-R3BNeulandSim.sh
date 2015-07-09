#!/bin/bash

# Create a folder for root files, so this directory stays clean
mkdir -p output

# Kill background jobs if script is terminated
trap 'kill $(jobs -pr) 2>/dev/null' SIGINT SIGTERM EXIT

# Run all simulations in "parallel" in background
root -l -q -b 'R3BNeulandSim.c(1, 10000, "131Sn_1n_600AMeV_500keV.dat", "600AMeV.1n.500keV.14m", "output")' 1> /dev/null &
root -l -q -b 'R3BNeulandSim.c(2, 10000, "130Sn_2n_600AMeV_500keV.dat", "600AMeV.2n.500keV.14m", "output")' 1> /dev/null &
root -l -q -b 'R3BNeulandSim.c(3, 10000, "129Sn_3n_600AMeV_500keV.dat", "600AMeV.3n.500keV.14m", "output")' 1> /dev/null &
root -l -q -b 'R3BNeulandSim.c(4, 10000, "128Sn_4n_600AMeV_500keV.dat", "600AMeV.4n.500keV.14m", "output")' 1> /dev/null &

# Wait for all background jobs to finish
wait

# Remove junk
rm -f calor.out
rm -f flukaerr.dat
rm -f gphysi.dat
