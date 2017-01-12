#!/bin/bash

# Set output directory from script arguments or use default
OUTDIR=${1:-output}

#python eval-sim.py ${OUTDIR}/*.sim.root > eval-sim.dat
python eval-digi.py ${OUTDIR}/*.mon.root > eval-digi.dat
