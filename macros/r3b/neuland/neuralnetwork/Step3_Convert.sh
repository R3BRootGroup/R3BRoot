#!/bin/bash

# Set output directory from script arguments or use default
OUTDIR=${1:-output}

# Build python protobuf include TODO: Replace with cmake once fairroot is fixed
protoc --python_out=. --proto_path=${VMCWORKDIR}/neuland/reconstruction/ ${VMCWORKDIR}/neuland/reconstruction/neuland.proto

# Convert ROOT Data to protobuf stream file for faster I/O
python Step3_Convert.py ${OUTDIR}/train.gam ${OUTDIR}*/1400cm*600AMeV*.digi.root
