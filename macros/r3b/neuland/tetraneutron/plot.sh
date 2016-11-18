#!/bin/bash

for DISTANCE in 1400 3500; do
	for NDOUBLEPLANES in 10 12 14 30; do
		python plot_2DCalibr.py ${DISTANCE} ${NDOUBLEPLANES}
		python plot_cluster.py ${DISTANCE} ${NDOUBLEPLANES}
		python plot_npnip.py  ${DISTANCE} ${NDOUBLEPLANES}
		python plot_nRecoNPNIP.py ${DISTANCE} ${NDOUBLEPLANES} NeulandRecoMon2DCalibr
		python plot_nReco.py ${DISTANCE} ${NDOUBLEPLANES} NeulandRecoMon2DCalibr
		python plot_nReco.py ${DISTANCE} ${NDOUBLEPLANES} NeulandRecoMonEGate
		python plot_nReco.py ${DISTANCE} ${NDOUBLEPLANES} NeulandRecoMonNeuralNetworkMultiplicity
	done
done

for i in *.pdf; do
	convert -density 300 -trim ${i} "${i%.*}.png"
done
