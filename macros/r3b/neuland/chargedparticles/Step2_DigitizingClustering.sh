#!/bin/bash

# Set output directory from script arguments or use default
OUTDIR=${1:-output}
FILES=(${OUTDIR}/*.sim.root)

# Kill background jobs if script is terminated
trap 'kill $(jobs -pr) 2>/dev/null' SIGINT SIGTERM EXIT

# Run process on all simulation files in "parallel" in background
COUNT=0
for f in "${FILES[@]}"
do
	nice -n 19 root -l -q -b 'Step2_DigitizingClustering.C('\"${f}\"')' &>"${f}.log" &

	# Only spawn so many processes at once
	COUNT=$((${COUNT}+1))
	if (( ${COUNT} % 30 == 0 )); then
		wait
		echo "${COUNT}/${#FILES[@]}"
	fi
done

# Wait for all background jobs to finish
wait
echo "${COUNT}/${#FILES[@]}"
