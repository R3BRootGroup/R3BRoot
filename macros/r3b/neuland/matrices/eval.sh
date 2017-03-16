#!/bin/bash

# Set output directory from script arguments or use default
OUTDIR=${1:-output}

python eval.py ${OUTDIR}/*.digi.root > eval.dat
python eval-nnpnips.py ${OUTDIR}/*.digi.root > eval-nnpnips.dat
