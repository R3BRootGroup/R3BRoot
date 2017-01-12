#!/bin/bash

# Set output directory from script arguments or use default
OUTDIR=${1:-output}

#python eval.py ${OUTDIR}/*.digi.root > eval.dat
#python eval-thresh.py ${OUTDIR}/1400cm_*.digi.root > eval-thresh.dat
#python eval-nnpnips.py ${OUTDIR}/*.digi.root > eval-nnpnips.dat
python eval-nreacnreco.py NeulandRecoMon2DCalibr ${OUTDIR}/*.eval.root > eval-nreacnreco-2D.dat
python eval-nreacnreco.py NeulandRecoMonEGate ${OUTDIR}/*.eval.root > eval-nreacnreco-EG.dat