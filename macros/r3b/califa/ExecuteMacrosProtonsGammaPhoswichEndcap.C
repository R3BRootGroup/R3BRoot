#Instituto de Estructura de la Materia -IEM-, CSIC, Madrid
#J.Sánchez del Río Sáez, A. Perea. E. Nácher, O.Tengblad for the R3B Collaboration. Contact: josesrs@gmail.com & jose.sanchez@csic.es
#Phoswich Endcap CALIFA, Version 1.0



#Put the proper paths according to the user 

export SIMPATH=/home/jose/fairsoft
export ROOTSYS=/home/jose/fairsoft/tools/root
export PATH=$ROOTSYS/bin:$PATH

export VMCWORKDIR=/home/jose/fairsoft/r3broot


cd /home/jose/fairsoft/r3broot/build/

cmake ../
. ./config.sh
make

cd /home/jose/fairsoft/r3broot/macros/r3b/califa/

#rm -rf *.dat *.out



root -l r3bsim_Protons_50MeV.C -q
#root -l eventDisplay_Protons_50MeV.C

#root -l califaAna_Protons_50MeV.C -q
#root -l checkResults_Protons_50MeV.C -q



#root -l r3bsim_Protons_100MeV.C -q
#root -l eventDisplay.C

#root -l califaAna_Protons_100MeV.C -q
#root -l checkResults_Protons_100MeV.C  -q

#..... etc.

#For gamma radiation:

#root -l r3bsim_Gamma_0_5MeV.C -q
#root -l eventDisplay_Gamma_50MeV.C

#root -l califaAna_Gamma_0_5MeV.C -q
#root -l checkResults_Gamma_0_5MeV.C -q



#root -l r3bsim_Gamma_1MeV.C -q
#root -l eventDisplay_Gamma_1MeV.C

#root -l califaAna_Gamma_1MeV.C -q
#root -l checkResults_Gamma_1MeV.C  -q

#..... etc.

