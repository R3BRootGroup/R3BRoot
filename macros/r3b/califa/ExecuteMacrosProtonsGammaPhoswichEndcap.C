#Instituto de Estructura de la Materia -IEM-, CSIC, Madrid
#J.Sánchez del Río Sáez, A. Perea. E. Nácher, O.Tengblad for the R3B Collaboration. Contact: josesrs@gmail.com & jose.sanchez@csic.es
#Phoswich Endcap CALIFA, Version 1.0



#Put the proper paths according to the user 

export SIMPATH=/home/josesrs/fairsoft_sept2012_externalPackages
#export ROOTSYS=/home/josesrs/fairsoft/tools/root
export ROOTSYS=/home/josesrs/fairsoft_sept2012_externalPackages
export PATH=$ROOTSYS/bin:$PATH

export VMCWORKDIR=/home/josesrs/r3broot_sept2012

. $ROOTSYS/bin/thisroot.sh

cd /home/josesrs/r3broot_sept2012/build/

cmake ../
. ./config.sh
make

cd /home/josesrs/r3broot_sept2012/macros/r3b/califa/

#rm -rf *.dat *.out



#root -l r3bsim.C

root -l r3bsim_Protons_50MeV.C -q
root -l eventDisplay_Protons_50MeV.C

root -l califaAna_Protons_50MeV.C -q
root -l checkResults_Protons_50MeV.C -q




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

