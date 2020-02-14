#!/bin/bash

# Performs all Analysis FairTasks. Test if the input is a positive integer:
if [ $1 -eq $1 2>/dev/null -o $1 -eq 0 2>/dev/null ] && [ $1 -gt "0" ]
then
root -l -q "Read_DNN_Multiplicity.cpp($1)"
root -l -q "BetaReconstruction.cpp($1)"
root -l -q "SingleReconstruction.cpp($1)"
root -l -q "GetMultMatrices.cpp($1)"
root -l -q "TradMed_Apply.cpp($1)"
root -l -q "NeuLAND_ScoreApplication.cpp($1)"
root -l -q "NeuLAND_DNNScoreApplication.cpp($1)"
root -l -q "ConvertReconstruction.cpp($1)"
root -l -q "NeuLAND_DetectionRate.cpp($1)"
root -l -q "ErelSpectra.cpp($1)"
root -l -q "PhysListPlots.cpp($1)"
root -l -q MakePlots.cpp
else
echo '### FATAL: You should supply a positive integer as input to this script!'
fi
