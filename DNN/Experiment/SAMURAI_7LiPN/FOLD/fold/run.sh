#!/bin/bash

# Compile:
make clean
make

# Run:
cp ../wsaw/Outputs/LI7BE ./LI7BE
./fold <./Inputs/7Li7Be_IAS_110MeV.inp> ./Outputs/7Li7Be_IAS_110MeV.out 
cp ./LI7IAS11 ./Outputs/LI7IAS11
rm -rf ./LI7IAS11
rm -rf ./LI7BE

cp ../wsaw/Outputs/LI7BE ./LI7BE
./fold <./Inputs/7Li7Be_IAS_250MeV.inp> ./Outputs/7Li7Be_IAS_250MeV.out 
cp ./LI7IAS25 ./Outputs/LI7IAS25
rm -rf ./LI7IAS25
rm -rf ./LI7BE
