#!/bin/bash

root -l -q -b 'r3blandsim.c(1, 10000, "neuland_v12a_14m.geo.root", "131Sn_1n_600AMeV_500keV.dat", "600AMeV.1n.500keV.14m")' 1> /dev/null &
root -l -q -b 'r3blandsim.c(2, 10000, "neuland_v12a_14m.geo.root", "130Sn_2n_600AMeV_500keV.dat", "600AMeV.2n.500keV.14m")' 1> /dev/null &
root -l -q -b 'r3blandsim.c(3, 10000, "neuland_v12a_14m.geo.root", "129Sn_3n_600AMeV_500keV.dat", "600AMeV.3n.500keV.14m")' 1> /dev/null &
root -l -q -b 'r3blandsim.c(4, 10000, "neuland_v12a_14m.geo.root", "128Sn_4n_600AMeV_500keV.dat", "600AMeV.4n.500keV.14m")' 1> /dev/null &

wait
rm calor.out
rm flukaerr.dat
rm gphysi.dat
