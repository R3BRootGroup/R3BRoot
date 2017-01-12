#!/bin/env gnuplot

reset

set encoding utf8
load '../gp/Paired.plt'
load '../gp/Axes.plt'

set terminal pdfcairo enhanced font "NotoSans,12"

set key inside top right Left reverse spacing 1.0 font ",9" maxrows 6

set grid

set xlabel "Neutron kinetic energy [MeV]" offset 0,0.5
set logscale x
set xrange [1e1:1e4]
set mxtics 10


set ylabel "Reacted neutrons (∝σ_{n,tot}) [%]" offset 1,0
set yrange [0:40]
set mytics 5

set output "crosssection.pdf"
plot \
	sprintf("< awk '$1 == \"%s\"' eval-digi.dat | sort -nk 2", "HADRSTD"       ) using 2:($4*100) w steps ls 4  dt 3 lw 1 title 'G3 Default (HADR=1)', \
	sprintf("< awk '$1 == \"%s\"' eval-digi.dat | sort -nk 2", "HADRRB"        ) using 2:($4*100) w steps ls 4  dt 1 lw 1 title 'G3 R3BRoot (HADR=5)', \
	sprintf("< awk '$1 == \"%s\"' eval-digi.dat | sort -nk 2", "FTFP_BERT"     ) using 2:($4*100) w steps ls 1  dt 1 lw 1 title 'G4 FTFP\_BERT', \
	sprintf("< awk '$1 == \"%s\"' eval-digi.dat | sort -nk 2", "FTFP_BERT_HP"  ) using 2:($4*100) w steps ls 1  dt 3 lw 1 title 'G4 FTFP\_BERT\_HP', \
	sprintf("< awk '$1 == \"%s\"' eval-digi.dat | sort -nk 2", "FTFP_INCLXX"   ) using 2:($4*100) w steps ls 2  dt 1 lw 1 title 'G4 FTFP\_INCLXX', \
	sprintf("< awk '$1 == \"%s\"' eval-digi.dat | sort -nk 2", "FTFP_INCLXX_HP") using 2:($4*100) w steps ls 2  dt 3 lw 1 title 'G4 FTFP\_INCLXX\_HP', \
	sprintf("< awk '$1 == \"%s\"' eval-digi.dat | sort -nk 2", "FTF_BIC"       ) using 2:($4*100) w steps ls 9  dt 1 lw 1 title 'G4 FTF\_BIC', \
	sprintf("< awk '$1 == \"%s\"' eval-digi.dat | sort -nk 2", "QBBC"          ) using 2:($4*100) w steps ls 10 dt 1 lw 1 title 'G4 QBBC', \
	sprintf("< awk '$1 == \"%s\"' eval-digi.dat | sort -nk 2", "QGSP_BERT"     ) using 2:($4*100) w steps ls 5  dt 1 lw 1 title 'G4 QGSP\_BERT', \
	sprintf("< awk '$1 == \"%s\"' eval-digi.dat | sort -nk 2", "QGSP_BERT_HP"  ) using 2:($4*100) w steps ls 5  dt 3 lw 1 title 'G4 QGSP\_BERT\_HP', \
	sprintf("< awk '$1 == \"%s\"' eval-digi.dat | sort -nk 2", "QGSP_BIC"      ) using 2:($4*100) w steps ls 6  dt 1 lw 1 title 'G4 QGSP\_BIC', \
	sprintf("< awk '$1 == \"%s\"' eval-digi.dat | sort -nk 2", "QGSP_BIC_HP"   ) using 2:($4*100) w steps ls 6  dt 3 lw 1 title 'G4 QGSP\_BIC\_HP', \
	sprintf("< awk '$1 == \"%s\"' eval-digi.dat | sort -nk 2", "QGSP_FTFP_BERT") using 2:($4*100) w steps ls 7  dt 1 lw 1 title 'G4 QGSP\_FTFP\_BERT', \
	sprintf("< awk '$1 == \"%s\"' eval-digi.dat | sort -nk 2", "QGSP_INCLXX"   ) using 2:($4*100) w steps ls 8  dt 1 lw 1 title 'G4 QGSP\_INCLXX', \
	sprintf("< awk '$1 == \"%s\"' eval-digi.dat | sort -nk 2", "QGSP_INCLXX_HP") using 2:($4*100) w steps ls 8  dt 3 lw 1 title 'G4 QGSP\_INCLXX\_HP'
unset output
unset label
