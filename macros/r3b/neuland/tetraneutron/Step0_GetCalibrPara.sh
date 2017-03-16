#!/bin/bash

# Set output directory from script arguments or use default
OUTDIR=${1:-output}
# Create a folder for root files, so this directory stays clean
mkdir -p ${OUTDIR}


# Copy over 2DCalibr calibration files
COUNT=0
for DISTANCE in 1400 3500; do
	for NDOUBLEPLANES in 12 30 50; do
		cp ../matrices/output/${DISTANCE}cm_${NDOUBLEPLANES}dp_600AMeV_500keV.neutroncuts.para.root ${OUTDIR}/
	done
done

# Create Fake Calibr File for testing
root -l -q "Step0_FakeCalibr.C(\"${OUTDIR}/fake.root\")"
