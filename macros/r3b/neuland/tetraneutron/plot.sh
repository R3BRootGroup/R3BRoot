#!/bin/bash

# distance = int(sys.argv[1])
# nDoublePlanes = int(sys.argv[2])
# nNeutrons = int(sys.argv[3])
# energy = int(sys.argv[4])
# erel = int(sys.argv[5])
# directory = str(sys.argv[6])

for DISTANCE in 1400 3500; do
	for NDOUBLEPLANES in 12 50; do
		for ENERGY in 300 600; do
			for EREL in 100 500; do
				for NNEUTRONS in 1 2 3 4; do
					python plot_2DCalibr.py ${DISTANCE} ${NDOUBLEPLANES} ${NNEUTRONS} ${ENERGY} ${EREL}
					python plot_cluster.py ${DISTANCE} ${NDOUBLEPLANES} ${NNEUTRONS} ${ENERGY} ${EREL}
					python plot_npnip.py ${DISTANCE} ${NDOUBLEPLANES} ${NNEUTRONS} ${ENERGY} ${EREL}
					python plot_nRecoNPNIP.py ${DISTANCE} ${NDOUBLEPLANES} ${NNEUTRONS} ${ENERGY} ${EREL} NeulandRecoMon2DCalibr
					python plot_nReco.py ${DISTANCE} ${NDOUBLEPLANES} ${NNEUTRONS} ${ENERGY} ${EREL} NeulandRecoMon2DCalibr
					#python plot_nReco.py ${DISTANCE} ${NDOUBLEPLANES} NeulandRecoMonEGate
					#python plot_nReco.py ${DISTANCE} ${NDOUBLEPLANES} NeulandRecoMonNeuralNetworkMultiplicity
				done
			done
		done
	done
done
