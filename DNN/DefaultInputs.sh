#!/bin/bash

# This script (re)sets all inputs to default values:
cd ./InputFiles
rm -rf ./InputsFile.txt
rm -rf ./OutputPath.txt
cd ../Macros
root -l -q GenerateInputs.cpp
#root -l -q GenerateSAMURAIInputs.cpp
cd ../
