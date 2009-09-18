#!/bin/bash 
#------------------------------------------------
# The Virtual Monte Carlo examples
# Copyright (C) 2007, Ivana Hrivnacova
# All rights reserved.
#
# For the licensing terms see geant4_vmc/LICENSE.
# Contact: vmc@pcroot.cern.ch
#-------------------------------------------------

#
# Macro for preparing working directory for Fluka
# Adapted macro AliRoot/TFluka/scripts/runflukageo.sh 
# by Andreas Morsch

# Adapted to FairRoot by M. Al-Turany
# 
# Updated by <D.Bertini@gsi.de> 
#          Date: Sep 18/2009
#          Fluka version ---->   2008.3b1

echo " " 
echo "-I- TFluka: Preparing Fluka working directory ..." 

CURDIR=`pwd`
WORKDIR=$CURDIR"/fluka_out"

# Remove the previous temporary directory
rm -rf $WORKDIR

# Make a new temporary directory and move to it
mkdir $WORKDIR
cd $WORKDIR

# Link here some special Fluka files needed
ln -s $FLUPRO/neuxsc-ind_260.bin neuxsc.bin
ln -s $FLUPRO/sigmapi.bin sigmapi.bin
ln -s $FLUPRO/elasct.bin elasct.bin
ln -s $FLUPRO/nuclear.bin nuclear.bin

ln -s $FLUPRO/fluodt.dat fluodt.dat
ln -s $FLUPRO/random.dat random.dat
ln -s $FLUPRO/sidan.dat  sidan.dat
ln -s $FLUPRO/sidae.dat  sidae.dat
ln -s $FLUPRO/sidapi.dat sidapi.dat
ln -s $FLUPRO/sidap.dat  sidap.dat


# Copy the random seed
cp $FLUPRO/random.dat old.seed

# Give some meaningfull name to the output
ln -s fluka.out fort.11

#Link FlukaConfig.C as Config.C
#ln -fs ../run.C run.C
ln -fs $VMCWORKDIR/gconfig/flConfig.C flConfig.C
cp $FLUVMC/input/coreFlukaVmc.inp coreFlukaVmc.inp

echo "-I- TFluka: Preparing Fluka working directory ... finished" 
echo " " 
