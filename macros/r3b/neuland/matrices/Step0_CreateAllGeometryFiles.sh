#!/bin/bash

DISTANCE=1400

for NDOUBLEPLANES in $(seq 4 50); do
	NPLANES=$((${NDOUBLEPLANES}*2));
	root -l -q ${VMCWORKDIR}'/macros/r3b/geo/create_neuland_demo_geo.C('${NPLANES}', '${DISTANCE}', "v2_'${DISTANCE}'cm_'${NDOUBLEPLANES}'dp")';
done
