#!/bin/bash

# Set output directory from script arguments or use default
OUTDIR=${1:-output}
# Create a folder for root files, so this directory stays clean
mkdir -p ${OUTDIR}

# Number of batches
BATCHES=$(seq 1 100)
# Number of events per batch
EVENTS=1000
# Number of primary particles
PRIMARIES=$(seq 1 5)
# Kinetic energy in MeV
MIN_ENERGY=550
MAX_ENERGY=650
# Calculate momentum in GeV, p = √(K² + 2Kmc²)
MIN_MOMENTUM=$(echo "sqrt(($MIN_ENERGY^2)+(2*$MIN_ENERGY*939.))/1000." | bc -l)
MAX_MOMENTUM=$(echo "sqrt(($MAX_ENERGY^2)+(2*$MAX_ENERGY*939.))/1000." | bc -l)

# Kill background jobs if script is terminated
trap 'echo "Stopping..."; kill $(jobs -pr) 2>/dev/null; exit' SIGINT SIGTERM
# Remove Junk on exit
trap 'echo "Cleaning..."; rm -f calor.out; rm -f flukaerr.dat; rm -f gphysi.dat; exit' EXIT

# Run all simulations in "parallel" in background
COUNT=0
for PRIMARY in $PRIMARIES; do
	for BATCH in $BATCHES; do
		FILENAME=$(printf "2112-%01i-%04i" $PRIMARY $BATCH)
		echo $FILENAME
		nice -n 19 root -l -q -b 'Step1_Simulate.C(2112, '${PRIMARY}', '${MIN_MOMENTUM}', '${MAX_MOMENTUM}', '${EVENTS}', "'${OUTDIR}'", "'${FILENAME}'")' &>/dev/null &

		# Only spawn so many processes at once
		COUNT=$((${COUNT}+1))
		if (( ${COUNT} % 30 == 0 )); then
			wait
		fi
	done
done

# Wait for all background jobs to finish
wait
