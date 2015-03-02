#!/bin/bash

for integration in 11 20 80; do
	root -l -q -b 'calibr_mini.c++("./r3bcalibr.600AMeV.%1dn.500keV.14m.s012i'${integration}'.root", "./r3bcalibr.600AMeV.500keV.14m.s012i'${integration}'")'
	root -l -q -b 'calibr_mini.c++("./r3bcalibr.600AMeV.%1dn.500keV.14m.s0i'${integration}'.root",   "./r3bcalibr.600AMeV.500keV.14m.s0i'${integration}'")'
done
