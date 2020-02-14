#!/bin/bash

# Compile:
make clean
make

# Run:
./wsaw <./Inputs/7Li7Be.inp> ./Outputs/7Li7Be.out
cp ./LI7BE ./Outputs/LI7BE
rm -rf ./LI7BE
