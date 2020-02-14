#!/bin/bash

# Compile:
make clean
make

# Run:
cp ../fold/Outputs/LI7IAS11 ./LI7IAS11
./dwhi <./Inputs/7Li7Be_IAS_110MeV.inp> ./Outputs/7Li7Be_IAS_110MeV.out 
cp ./Li7_IAS_110.plot ./Outputs/Li7_IAS_110.plot
rm -rf ./LI7IAS11
rm -rf ./Li7_IAS_110.plot

cp ../fold/Outputs/LI7IAS11 ./LI7IAS11
./dwhi <./Inputs/7Li7Be_IAS_110MeV_q0.inp> ./Outputs/7Li7Be_IAS_110MeV_q0.out 
cp ./Li7_IAS_1q0.plot ./Outputs/Li7_IAS_110_q0.plot
rm -rf ./LI7IAS11
rm -rf ./Li7_IAS_1q0.plot

cp ../fold/Outputs/LI7IAS25 ./LI7IAS25
./dwhi <./Inputs/7Li7Be_IAS_250MeV.inp> ./Outputs/7Li7Be_IAS_250MeV.out 
cp ./Li7_IAS_250.plot ./Outputs/Li7_IAS_250.plot
rm -rf ./LI7IAS25
rm -rf ./Li7_IAS_250.plot

cp ../fold/Outputs/LI7IAS25 ./LI7IAS25
./dwhi <./Inputs/7Li7Be_IAS_250MeV_q0.inp> ./Outputs/7Li7Be_IAS_250MeV_q0.out 
cp ./Li7_IAS_2q0.plot ./Outputs/Li7_IAS_250_q0.plot
rm -rf ./LI7IAS25
rm -rf ./Li7_IAS_2q0.plot
