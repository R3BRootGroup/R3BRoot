#!gnuplot

reset

#load '../gp/Paired.plt'
#load '../gp/Axes.plt'

set terminal pdf enhanced font "Open Sans"

set key top left
set yrange [0:1]
set grid

set xlabel "Number of Doubleplanes"
set xrange [0:50]

set output "plotEfficiencyVSPlanes.pdf"
set ylabel "Efficiency"
set yrange [0:1]
plot \
	"< awk '{print FILENAME \"\t\" $0}' output/*.dat | awk '{sub(\"cm_\",\"\t\",$0); print}' | awk '{sub(\"dp.dat\",\"\t\",$0); print}' | grep '1n:' | sort -nk 2" u ($2-0.5):4 title "1n" ls 1 lw 1 w steps, \
	"< awk '{print FILENAME \"\t\" $0}' output/*.dat | awk '{sub(\"cm_\",\"\t\",$0); print}' | awk '{sub(\"dp.dat\",\"\t\",$0); print}' | grep '2n:' | sort -nk 2" u ($2-0.5):5 title "2n" ls 3 lw 1 w steps, \
	"< awk '{print FILENAME \"\t\" $0}' output/*.dat | awk '{sub(\"cm_\",\"\t\",$0); print}' | awk '{sub(\"dp.dat\",\"\t\",$0); print}' | grep '3n:' | sort -nk 2" u ($2-0.5):6 title "3n" ls 5 lw 1 w steps, \
	"< awk '{print FILENAME \"\t\" $0}' output/*.dat | awk '{sub(\"cm_\",\"\t\",$0); print}' | awk '{sub(\"dp.dat\",\"\t\",$0); print}' | grep '4n:' | sort -nk 2" u ($2-0.5):7 title "4n" ls 7 lw 1 w steps
unset output

set output "plotEPVSPlanes.pdf"
set ylabel "Efficiency • Purity"
plot \
	"< awk '{print FILENAME \"\t\" $0}' output/*.dat | awk '{sub(\"cm_\",\"\t\",$0); print}' | awk '{sub(\"dp.dat\",\"\t\",$0); print}' | grep '1n:' | sort -nk 2" u ($2-0.5):($8*$4) title "1n" ls 1 lw 1 w steps, \
	"< awk '{print FILENAME \"\t\" $0}' output/*.dat | awk '{sub(\"cm_\",\"\t\",$0); print}' | awk '{sub(\"dp.dat\",\"\t\",$0); print}' | grep '2n:' | sort -nk 2" u ($2-0.5):($8*$5) title "2n" ls 3 lw 1 w steps, \
	"< awk '{print FILENAME \"\t\" $0}' output/*.dat | awk '{sub(\"cm_\",\"\t\",$0); print}' | awk '{sub(\"dp.dat\",\"\t\",$0); print}' | grep '3n:' | sort -nk 2" u ($2-0.5):($8*$6) title "3n" ls 5 lw 1 w steps, \
	"< awk '{print FILENAME \"\t\" $0}' output/*.dat | awk '{sub(\"cm_\",\"\t\",$0); print}' | awk '{sub(\"dp.dat\",\"\t\",$0); print}' | grep '4n:' | sort -nk 2" u ($2-0.5):($8*$7) title "4n" ls 7 lw 1 w steps
unset output
