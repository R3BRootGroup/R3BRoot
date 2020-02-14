#!/bin/bash

# Performs an entire simulation:

# root -l -q ChangeInputs.cpp

cd ../Geometry/
root -l -q NeuLAND_BuildGeometry.cpp
#root -l -q GLAD_BuildGeometry.cpp
cd ../Macros/

root -l -q MCtransport.cpp
root -l -q NeuLAND_Digitizer.cpp
root -l -q NeuLAND_Translation.cpp
root -l -q ClusterSignals.cpp
root -l -q TranslateClusters.cpp
root -l -q TradMed_ComputeCuts.cpp
root -l -q TradMed_Apply.cpp
