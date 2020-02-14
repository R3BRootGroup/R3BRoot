#!/bin/bash

# Perform the full simulation:
root -l -q MCtransport.cpp
root -l -q NeuLAND_Digitizer.cpp
root -l -q NeuLAND_JanClusters.cpp
root -l -q NeuLAND_Translation.cpp
root -l -q ClusterSignals.cpp
root -l -q NeuLAND_ScoreCollection.cpp
root -l -q TranslateClusters.cpp
root -l -q TradMed_ComputeCuts.cpp
