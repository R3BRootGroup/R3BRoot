#!/bin/bash

# Compile:
make clean
make

# Run:
./PlotTheFile Li7_IAS_110.plot 2
./PlotTheFile Li7_IAS_250.plot 2
./PlotTheFile Li7_IAS_110_q0.plot 2
./PlotTheFile Li7_IAS_250_q0.plot 2
