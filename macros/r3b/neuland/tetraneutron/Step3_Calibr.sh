#!/bin/bash

# Set output directory from script arguments or use default
OUTDIR=${1:-output}
# Create a folder for root files, so this directory stays clean
mkdir -p ${OUTDIR}

NEVENTS=10000
DISTANCE=1400

# Copy over 2DCalibr calibration files
COUNT=0
for NDOUBLEPLANES in 10 12 14 30; do
	for NNEUTRONS in 4; do
		cp ../matrices/output-20161121/${DISTANCE}cm_${NDOUBLEPLANES}dp.neutroncuts.par.root ${OUTDIR}
	done
done

# Create Fake Calibr File for testing
root -l -q "Step3_FakeCalibr.C(\"${OUTDIR}/fake.root\")"
