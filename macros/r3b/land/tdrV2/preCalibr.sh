#!/bin/bash

for n in 1 2 3 4; do
	for integration in 11 20 80; do
		nice -n 19 root -l -q -b 'preCalibr.c(10000, "600AMeV.'${n}'n.500keV.14m", 600, 0.012, '${integration}', ".s012i'${integration}'")' &> /dev/null &
		nice -n 19 root -l -q -b 'preCalibr.c(10000, "600AMeV.'${n}'n.500keV.14m", 600, 0,     '${integration}', ".s0i'${integration}'")'   &> /dev/null &
	done
done

wait
