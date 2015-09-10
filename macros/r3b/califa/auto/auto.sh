#!/bin/bash

. clean.sh
mkdir RESULTS
root -l -b -q 'r3bsim_batch.C(0.005,1,5000,11,1)' > out_r3bsim.txt
mv out_r3bsim.txt RESULTS
cp r3bsim.root RESULTS
root -l -b -q 'califaAna_batch.C(5000,11,0.000050,5.,6.3,6.3)' > out_califaAna.txt
mv califaAna.root out_califaAna.txt RESULTS
root -l -b -q 'checkResults_batch.C(5000,11,0.050,5,1,5,6.5,"./RESULTS/califaAna.root","./RESULTS/r3bsim.root")' > out_check.txt
mv out_check.txt RESULTS
ps2pdf output.ps
mv output.pdf v13_811_5_res5.pdf
mv output.root v13_811_5_res5n5000.root
rm output.ps
. check_resolutions.sh