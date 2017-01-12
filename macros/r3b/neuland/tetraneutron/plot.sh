#!/bin/bash

mkdir -p plots

gnuplot plot-total.gp
gnuplot plot-nnpnips.gp
gnuplot plot-reco.gp
