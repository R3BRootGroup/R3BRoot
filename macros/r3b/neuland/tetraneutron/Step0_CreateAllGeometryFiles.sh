#!/bin/bash

# Input Files
root -l -q "Step0_GenerateInputFiles.C(0.0001, \"${VMCWORKDIR}/input/4He_4n_600AMeV_100keV.dat\")"
root -l -q "Step0_GenerateInputFiles.C(0.0005, \"${VMCWORKDIR}/input/4He_4n_600AMeV_500keV.dat\")"

# Geometry Files
for DISTANCE in 1400 3500; do
	for NDOUBLEPLANES in 10 12 14 30; do
		NPLANES=$((${NDOUBLEPLANES}*2));
		root -l -q ${VMCWORKDIR}'/macros/r3b/geo/create_neuland_demo_geo.C('${NPLANES}', '${DISTANCE}', "v2_'${DISTANCE}'cm_'${NDOUBLEPLANES}'dp")';
	done
done
