#!/bin/bash

# Set output directory from script arguments or use default
OUTDIR=${1:-output}

# Kill background jobs if script is terminated
trap 'kill $(jobs -pr) 2>/dev/null' SIGINT SIGTERM EXIT

# Run porcess on all simualtion files in "parallel" in background
for f in ${OUTDIR}/*.sim.root; do
	root -l -q -b 'Step2_Digitizing.C('\"${f}\"')' &
done

# Wait for all background jobs to finish
wait
