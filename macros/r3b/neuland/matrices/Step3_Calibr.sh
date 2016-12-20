#!/bin/bash

# Set output directory from script arguments or use default
OUTDIR=${1:-output}
# Create a folder for root files, so this directory stays clean
mkdir -p ${OUTDIR}

DISTANCE=1400

# Kill background jobs if script is terminated
trap 'kill $(jobs -pr) 2>/dev/null' SIGINT SIGTERM EXIT

# Run all tasks in "parallel" in background
COUNT=0
for NDOUBLEPLANES in $(seq 4 50); do
	COMMAND="Step3_Calibr.C(\"./${OUTDIR}/${DISTANCE}cm_${NDOUBLEPLANES}dp\")"
	# Note: The root call is extemely sensitive to the usage of ' and "
	nice -n 19 root -l -q -b "${COMMAND}" &> "./${OUTDIR}/${DISTANCE}cm_${NDOUBLEPLANES}dp.calibr.log" &
	echo ${COMMAND}

	# Only spawn so many processes at once
	COUNT=$((${COUNT}+1))
	if (( ${COUNT} % 30 == 0 )); then
		wait
	fi
done

# Wait for all background jobs to finish
wait

# Convert all pdfs
cd ${OUTDIR} && mogrify -trim -density 200 -format png *.pdf
