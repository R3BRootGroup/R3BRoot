#!gnuplot

if (!exists('dir')) dir='output'

load '../gp/Set1.plt'
#load '../gp/Axes.plt'

set terminal pdf enhanced font 'Open Sans'

set key top right


set mxtics 2

set ylabel 'counts' offset 1
set ytics auto offset 1
set mytics 2

#set label '600 MeV/u' at graph 0.97,0.9 right
set label dir at graph 0.05,0.95 left


set output dir.'/depth.pdf'
set xlabel 'Maximal depth [cm]'
plot \
	dir.'/1400cm_30dp_proton.hDepth.dat'   u 1:2 w steps ls 3 title 'p', \
	dir.'/1400cm_30dp_deuteron.hDepth.dat' u 1:2 w steps ls 4 title 'd', \
	dir.'/1400cm_30dp_triton.hDepth.dat'   u 1:2 w steps ls 5 title 't', \
	dir.'/1400cm_30dp_He3.hDepth.dat'      u 1:2 w steps ls 2 title 'He-3', \
	dir.'/1400cm_30dp_alpha.hDepth.dat'    u 1:2 w steps ls 1 title 'α', \

unset output


set output dir.'/EnergyFirstPaddle.pdf'
set xlabel 'Energy in First Paddle [MeV]'
plot \
	dir.'/1400cm_30dp_proton.hForemostEnergy.dat'   u 1:2 w steps ls 3 title 'p', \
	dir.'/1400cm_30dp_deuteron.hForemostEnergy.dat' u 1:2 w steps ls 4 title 'd', \
	dir.'/1400cm_30dp_triton.hForemostEnergy.dat'   u 1:2 w steps ls 5 title 't', \
	dir.'/1400cm_30dp_He3.hForemostEnergy.dat'      u 1:2 w steps ls 2 title 'He-3', \
	dir.'/1400cm_30dp_alpha.hForemostEnergy.dat'    u 1:2 w steps ls 1 title 'α', \

unset output


set output dir.'/Etot.pdf'
set xrange [0:2500]
set xlabel 'Total Energy [MeV]'
plot \
	dir.'/1400cm_30dp_proton.hEtot.dat'   u 1:2 w steps ls 3 title 'p', \
	dir.'/1400cm_30dp_deuteron.hEtot.dat' u 1:2 w steps ls 4 title 'd', \
	dir.'/1400cm_30dp_triton.hEtot.dat'   u 1:2 w steps ls 5 title 't', \
	dir.'/1400cm_30dp_He3.hEtot.dat'      u 1:2 w steps ls 2 title 'He-3', \
	dir.'/1400cm_30dp_alpha.hEtot.dat'    u 1:2 w steps ls 1 title 'α', \

unset output
