#!/bin/bash

for i in 1 2 3; 
do 
var=`head -n $i check_resolutions.dat | tail -n 1`; 
if [[ "$i" = 1 ]]; then 
if [[ "$var" < 20 ]]; then echo "Failed Photopeak Efficiency: $var less than 20%"; fi; 
fi;
if [[ "$i" = 2 ]]; then 
if [[ "$var" > 9 ]]; then echo "Failed Photopeak Resolution [FWHM] : $var greater than 9%"; fi; 
fi;
if [[ "$i" = 3 ]]; then 
if [[ "$var" > 2 ]]; then echo "Failed Polar Angle Resolution: $var greater than 2 deg."; fi; 
fi;
done
