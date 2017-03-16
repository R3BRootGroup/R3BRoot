#!/bin/bash

# Input Files
for EKIN in 300 600; do
	for EREL in 100 500; do
		for NNEUTRONS in 1 2 3 4; do
			root -l -q "Step0_GenerateInputFiles.C(${NNEUTRONS}, ${EKIN}, ${EREL}, \"${VMCWORKDIR}/input/4He_${NNEUTRONS}n_${EKIN}AMeV_${EREL}keV.dat\")"
		done
	done
done
