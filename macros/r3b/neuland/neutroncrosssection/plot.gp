#!/bin/env gnuplot

reset

load '../gp/Paired.plt'
#load '../gp/Axes.plt'

set grid

plists = 'FTFP\_BERT FTFP\_BERT\_HP FTFP\_INCLXX FTFP\_INCLXX\_HP FTF\_BIC QBBC QGSP\_BERT QGSP\_BERT\_HP QGSP\_BIC QGSP\_BIC\_HP QGSP\_FTFP\_BERT QGSP\_INCLXX QGSP\_INCLXX\_HP'

do for [term in "pdf svg"]{
if (term eq "svg"){
	set terminal term enhanced font "Helvetica" dynamic
	set key outside right Left reverse
} else {
	set terminal term enhanced font "Helvetica"
	set key at graph 0.95,0.98 Left reverse
}

set xlabel "Neutron Energy [MeV]"
set logscale x
set mxtics 10
set xrange [2e0:1e4]

set yrange [0:0.7]

set output "plot-mctracks.".term
set ylabel "Events with #MCTracks > 1 / All Events"
plot \
	for [plist in plists] \
	sprintf("< awk '$1 == \"%s\"' eval-sim.dat 2>/dev/null | sort -k 2 -g", plist) using 2:3 w steps lw 2 title plist
unset output

set output "plot-neulandpoints.".term
set ylabel "Events with #NeulandPoints > 0 / All Events"
plot \
	for [plist in plists] \
	sprintf("< awk '$1 == \"%s\"' eval-sim.dat 2>/dev/null | sort -k 2 -g", plist) using 2:4 w steps lw 2 title plist
unset output

set output "plot-neulanddigis.".term
set ylabel "Events with #NeulandDigis > 0 / All Events"
plot \
	for [plist in plists] \
	sprintf("< awk '$1 == \"%s\"' eval-digi.dat 2>/dev/null | sort -k 2 -g", plist) using 2:3 w steps lw 2 title plist
unset output


unset logscale x
set mxtics 10
set xrange [100:1000]

set yrange [0:0.2]

set output "plot-mctracks2.".term
set ylabel "Events with #MCTracks > 1 / All Events"
plot \
	for [plist in plists] \
	sprintf("< awk '$1 == \"%s\"' eval-sim.dat 2>/dev/null | sort -k 2 -g", plist) using 2:3 w steps lw 2 title plist
unset output

set output "plot-neulandpoints2.".term
set ylabel "Events with #NeulandPoints > 0 / All Events"
plot \
	for [plist in plists] \
	sprintf("< awk '$1 == \"%s\"' eval-sim.dat 2>/dev/null | sort -k 2 -g", plist) using 2:4 w steps lw 2 title plist
unset output

set output "plot-neulanddigis2.".term
set ylabel "Events with #NeulandDigis > 0 / All Events"
plot \
	for [plist in plists] \
	sprintf("< awk '$1 == \"%s\"' eval-digi.dat 2>/dev/null | sort -k 2 -g", plist) using 2:3 w steps lw 2 title plist
unset output

} # do term
