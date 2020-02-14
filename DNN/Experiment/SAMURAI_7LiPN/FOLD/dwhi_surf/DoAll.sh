#!/bin/bash

make clean
make -j

if [[ $1 == 'Snpot' ]]
then
cp -R ./Inputs_Snpot ./Inputs
elif [[ $1 == 'Zrpot' ]]
then
cp -R ./Inputs_Zrpot ./Inputs
elif [[ $1 == 'ZrDeep' ]]
then
cp -R ./Inputs_ZrDeep ./Inputs
elif [[ $1 == 'ZrSurf' ]]
then
cp -R ./Inputs_ZrSurf ./Inputs
elif [[ $1 == 'Paper' ]]
then
cp -R ./Inputs_Paper ./Inputs
elif [[ $1 == 'ZrDEEPEST' ]]
then
cp -R ./Inputs_ZrDEEPEST ./Inputs
elif [[ $1 == 'ZrRlarge' ]]
then
cp -R ./Inputs_ZrRlarge ./Inputs
elif [[ $1 == 'ZrSurf_Simple' ]]
then
cp -R ./Inputs_ZrSurf_SimpleSmearing ./Inputs
fi

./run.sh Sn116Sb_IAS
./run.sh Sn116Sb_IAS_q0
./run.sh Sn116Sb_GT
./run.sh Sn116Sb_GT_q0
./run.sh Sn116Sb_QGT
./run.sh Sn116Sb_QGT_q0
./run.sh Sn116Sb_SDR
./run.sh Sn116Sb_SDR_q0
./run.sh Sn116Sb_Quad
./run.sh Sn116Sb_Quad_q0
./run.sh Sn116Sb_Oct
./run.sh Sn116Sb_Oct_q0
./run.sh Sn116Sb_DL4
./run.sh Sn116Sb_DL4_q0

./run.sh Sn122Sb_IAS
./run.sh Sn122Sb_IAS_q0
./run.sh Sn122Sb_GT
./run.sh Sn122Sb_GT_q0
./run.sh Sn122Sb_QGT
./run.sh Sn122Sb_QGT_q0
./run.sh Sn122Sb_SDR
./run.sh Sn122Sb_SDR_q0
./run.sh Sn122Sb_Quad
./run.sh Sn122Sb_Quad_q0
./run.sh Sn122Sb_Oct
./run.sh Sn122Sb_Oct_q0
./run.sh Sn122Sb_DL4
./run.sh Sn122Sb_DL4_q0

./run.sh Sn120Sb

./run.sh Sn118Sb_IAS
./run.sh Sn118Sb_IAS_q0
./run.sh Sn118Sb_GT
./run.sh Sn118Sb_GT_q0
./run.sh Sn118Sb_QGT
./run.sh Sn118Sb_QGT_q0
