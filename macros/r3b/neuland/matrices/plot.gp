#!/bin/env gnuplot
reset
set encoding utf8

load '../gp/Paired.plt'
#load '../gp/Axes.plt'

set terminal pdf enhanced font "Helvetica,6"

set grid back


set xlabel "Number of Doubleplanes"
set xrange [0:50]
set xtics 5
set mxtics 5

#set yrange [0:1.1]
set ytics 0.1

set fit errorvariables
set fit quiet
set fit logfile '/dev/null'


do for [energy in "200 600 1000"] {
set output "plot-TotalDetectionEfficiency-".energy."MeV.pdf"
set ylabel "Total Detection Efficiency" offset 1

p = 0.1
f(x, y) = 1-(1-(1-(1-p)**x))**y
fit [0:50] f(x,y) sprintf("< awk '$1 == 1400 && $3 != 1 && $4 == %s' eval.dat | sort -nk 2", energy) using 2:3:6 via p

set label at graph 0.98,0.92 right sprintf("%s MeV", energy) font ",8"

set key at graph 0.99,0.87 right top

set style textbox opaque noborder
set label boxed at graph 0.98,0.65 right \
"NeuLAND + GLAD(Vacuum) in Vacuum\n\
Distances: 14m (dark lines), 35m (bright lines)\n\
Geant4 QGSP\\_BIC\\_HP, all Cuts 1MeV\n\
Sn input files (E_{rel} 500keV), 10k events each"

set label boxed at graph 0.98,0.47 right "Total Detection Efficiency:"
set label boxed at graph 0.98,0.42 right "P(nDP, nN) = 1-(1- (1-(1-p)^{nDP}) )^{nN}" font ",5"
set label boxed at graph 0.98,0.38 right sprintf("nDP: number of doubleplanes \nnN: number of incoming neutrons \np = %.4f(%.4f) (fitted)", p , p_err) font ",5"

plot \
	for [n = 1:4] f(x,n) notitle ls 2*n lw 1, \
	for [n = 1:4] sprintf("< awk '$1 == 1400 && $3 == %d && $4 == %s' eval.dat | sort -nk 2", n, energy) u ($2-0.5):6 ls 2*n   lw 1.5 w steps title sprintf("%dn incoming, any signal detected", n), \
	for [n = 1:4] sprintf("< awk '$1 == 3500 && $3 == %d && $4 == %s' eval.dat | sort -nk 2", n, energy) u ($2-0.5):6 ls 2*n-1 lw 1.5 w steps notitle#, \
	#for [n = 1:1] for [thresh = 8:12] sprintf("< awk '$1 == 1400 && $3 == %d && $4 == %s' eval.dat | sort -nk 2", n, energy) u ($2-0.5):thresh ls 2*n dt thresh-5  lw 1.5 w steps notitle
unset label
unset output
} # do energy
