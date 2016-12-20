#!/bin/bash

# Set output directory from script arguments or use default
OUTDIR=${1:-output}
# Create a folder for root files, so this directory stays clean
mkdir -p ${OUTDIR}

# Kill background jobs if script is terminated
trap 'kill $(jobs -pr) 2>/dev/null' SIGINT SIGTERM EXIT

# Run all simulations in "parallel" in background
COUNT=0
for DISTANCE in 1400 3500; do
	for NDOUBLEPLANES in 10 12 14 30; do
		for NNEUTRONS in 4; do
			CLUSTER="${OUTDIR}/${DISTANCE}cm_${NDOUBLEPLANES}dp_${NNEUTRONS}n.digi.root"

			#CALIBR="${OUTDIR}/fake.root"
			#CALIBR="${OUTDIR}/${DISTANCE}cm_${NDOUBLEPLANES}dp.neutroncuts.par.root"
			CALIBR="${OUTDIR}/1400cm_${NDOUBLEPLANES}dp.neutroncuts.par.root"

			# Note: The root call is extremely sensitive to the usage of ' and "
			COMMAND="Step4_Reconstruction.C(\"${CLUSTER}\", \"${CALIBR}\")"
			echo ${COMMAND}
			nice -n 19 root -l -q -b "${COMMAND}" &> "${OUTDIR}/${DISTANCE}cm_${NDOUBLEPLANES}dp_${NNEUTRONS}n.reco.log" #&

			# Only spawn so many processes at once
			COUNT=$((${COUNT}+1))
			if (( ${COUNT} % 30 == 0 )); then
				wait
			fi
		done
	done
done

# Wait for all background jobs to finish
wait
