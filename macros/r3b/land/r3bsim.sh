#!/bin/bash

export LD_LIBRARY_PATH=/home/madalin/root/r3broot/build/lib:/home/madalin/root/fairsoft/jan10/tools/root/lib:/home/madalin/root/fairsoft/jan10/generators/lib:/home/madalin/root/fairsoft/jan10/generators/lib:/home/madalin/root/fairsoft/jan10/transport/geant3/lib/tgt_linux:/home/madalin/root/fairsoft/jan10/transport/geant4/lib/Linux-g++:/home/madalin/root/fairsoft/jan10/transport/geant4_vmc/lib/tgt_linux:/home/madalin/root/fairsoft/jan10/transport/vgm/lib:/home/madalin/root/fairsoft/jan10/cern/clhep/lib
export ROOTSYS=/home/madalin/root/fairsoft/jan10/tools/root
export VMCWORKDIR=/home/madalin/root/r3broot/trunk
export GEANT4VMC_MACRO_DIR=/home/madalin/root/fairsoft/jan10/transport/geant4_vmc/examples/macro/
export USE_VGM=1
. /home/madalin/root/fairsoft/jan10/transport/geant4/env.sh

cd /home/madalin/root/r3broot/build/macros
$ROOTSYS/bin/root.exe -q -b -l  /home/madalin/root/r3broot/build/macros/r3bsim.C
