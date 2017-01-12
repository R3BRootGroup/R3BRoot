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
set output "plot-nnpnips-".energy."MeV.pdf"
set ylabel "Fraction of Events" offset 1

# The probability of getting exactly k successes in n trials is given by the probability mass function:
bd(k, n, p) = int(n)!/(int(k)!*int(n-k)!)*p**k*(1-p)**(n-k)
pdp = 0.11
# x = number of dp, n neut in (= trials), m (neut react = successes)
f(x, n, m) = bd(m, n, (1-(1-pdp)**x))
g(x1, x2, x3) = f(x1, x2, x3)
set dummy x1, x2, x3, x4, x5
fit g(x1,x2,x3) sprintf("< awk '$1 == 1400 && $4 == %s && $3 != 1' eval-nnpnips.dat | sort -nk 2", energy) using 2:3:5:6 via pdp

set label at graph 0.98,0.92 right sprintf("%s MeV", energy) font ",8"

set key at graph 0.99,0.8 right top

#set style textbox opaque noborder
#set label boxed at graph 0.98,0.65 right \
#"NeuLAND + GLAD(Vacuum) in Vacuum\n\
#Distances: 14m (dark lines), 35m (bright lines)\n\
#Geant4 QGSP\\_BIC\\_HP, all Cuts 1MeV\n\
#Sn input files (E_{rel} 500keV), 10k events each"

#set label boxed at graph 0.98,0.47 right "Total Detection Efficiency:"
#set label boxed at graph 0.98,0.22 right "P(nDP, nN_{in}, nN_{reac}) = ({nN_{in}} over {nN_{reac}}) (1-(1-p)^{nDP})^{nN_{reac}} (1-(1-(1-p)^{nDP}))^{nN_{in}-nN_{reac}}" font ",5"
#set label boxed at graph 0.98,0.18 right sprintf("nDP: number of doubleplanes\nnN_{in}: number of incoming neutrons\nnN_{reac}: number of reacted neutrons\np = %.4f(%.4f) (fitted)", pdp , pdp_err) font ",5"
set label boxed at graph 0.98,0.88 right sprintf("p = %.4f", pdp) font ",5"

plot \
	for [n = 1:4] for [m = 0:n] f(x1,n,m) notitle ls 2*n lw 1 dt m+1, \
	for [n = 1:4] for [m = 0:n] sprintf("< awk '$1 == 1400 && $3 == %d && $4 == %s && $5 == %d' eval-nnpnips.dat | sort -nk 2", n, energy, m) u ($2-0.5):6 ls 2*n   lw 1.5 dt m+1 w steps title sprintf("%dn incoming, %dn reacted", n, m), \
	for [n = 1:4] for [m = 0:n] sprintf("< awk '$1 == 3500 && $3 == %d && $4 == %s && $5 == %d' eval-nnpnips.dat | sort -nk 2", n, energy, m) u ($2-0.5):6 ls 2*n-1 lw 1.5 dt m+1 w steps notitle
unset output

do for [nN = 1:4] {
set output "plot-nnpnips-".energy."MeV-".nN."n.pdf"
plot \
	for [n = nN:nN] for [m = 0:n] f(x1,n,m) notitle ls 2*n lw 1 dt m+1, \
	for [n = nN:nN] for [m = 0:n] sprintf("< awk '$1 == 1400 && $3 == %d && $4 == %s && $5 == %d' eval-nnpnips.dat | sort -nk 2", n, energy, m) u ($2-0.5):6 ls 2*n   lw 1.5 dt m+1 w steps title sprintf("%dn incoming, %dn reacted", n, m)#, \
	#for [n = 4:4] for [m = 0:n] sprintf("< awk '$1 == 3500 && $3 == %d && $4 == %s && $5 == %d' eval-nnpnips.dat | sort -nk 2", n, energy, m) u ($2-0.5):6 ls 2*m-1 lw 1.5 w steps notitle
unset output
}
unset label

} # do energy
