#!/bin/bash

# Set output directory from script arguments or use default
OUTDIR=${1:-output}

# Kill background jobs if script is terminated
trap 'kill $(jobs -pr) 2>/dev/null' SIGINT SIGTERM EXIT

# Run process on all simulation files in "parallel" in background
COUNT=0
for f in ${OUTDIR}/*.sim.root; do
	nice -n 19 root -l -q -b 'Step2_Digitizing.C('\"${f}\"')' &>/dev/null &

	# Only spawn so many processes at once
	COUNT=$((${COUNT}+1))
	if (( ${COUNT} % 30 == 0 )); then
		wait
	fi
done

# Wait for all background jobs to finish
wait
