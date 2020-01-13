/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "R3BAtima.h"

#include <numeric>

extern "C"
{
    void calculate_(double* proj,
                    int* pn,
                    double* ein,
                    double* targ,
                    int* nnuca,
                    int* tm,
                    double* rhoa,
                    int* igasa,
                    double* thick,
                    double* eout,
                    double* range,
                    double* dedxi,
                    double* dedxo,
                    double* remra,
                    double* rstr,
                    double* estr,
                    double* astr,
                    double* tof,
                    double* intrthick);
}

const R3BAtimaTargetMaterial R3BAtimaTargetMaterial::Air =
    R3BAtimaTargetMaterial({ R3BAtimaMaterialCompound(14, 7, 0.755267),
                             R3BAtimaMaterialCompound(16, 8, 0.231781),
                             R3BAtimaMaterialCompound(39.948, 18, 0.012827),
                             R3BAtimaMaterialCompound(12, 6, 0.000124) },
                           0.0012,
                           kTRUE);
const R3BAtimaTargetMaterial R3BAtimaTargetMaterial::LH2 =
    R3BAtimaTargetMaterial({ R3BAtimaMaterialCompound(1, 1) }, 0.0708, kFALSE);
const R3BAtimaTargetMaterial R3BAtimaTargetMaterial::Si =
    R3BAtimaTargetMaterial({ R3BAtimaMaterialCompound(28, 14) }, 2.336, kFALSE);
const R3BAtimaTargetMaterial R3BAtimaTargetMaterial::BC400 =
    R3BAtimaTargetMaterial({ R3BAtimaMaterialCompound(1, 1, 0.085292), R3BAtimaMaterialCompound(12, 6, 0.914708) },
                           1.032,
                           kFALSE);

R3BAtimaTransportResult R3BAtima::Calculate(Double_t projMass_u,
                                            Double_t projCharge_e,
                                            Double_t projEnergy_AMeV,
                                            const R3BAtimaTargetMaterial& targetMaterial,
                                            Double_t tarThickness_mg_per_cm2)
{
    R3BAtimaTransportResult res;

    const auto invTotRatio = std::accumulate(
        targetMaterial.Compounds.begin(),
        targetMaterial.Compounds.end(),
        0.,
        [](const Double_t sum, const R3BAtimaMaterialCompound& comp) -> Double_t { return sum + comp.Ratio; });

    Int_t tcompsize = 3, tcompnum = targetMaterial.Compounds.size(), pn = 2;
    std::vector<Double_t> tarFortran;
    tarFortran.resize(tcompnum * tcompsize);
    for (int i = 0; i < tcompnum; ++i)
    {
        tarFortran[i + tcompnum * 0] = targetMaterial.Compounds[i].Mass_u;
        tarFortran[i + tcompnum * 1] = targetMaterial.Compounds[i].Charge_e;
        tarFortran[i + tcompnum * 2] = targetMaterial.Compounds[i].Ratio * invTotRatio;
    }

    Double_t proj[2] = { projMass_u, projCharge_e };
    Double_t density = targetMaterial.Density;
    Int_t tGas = 0;
    if (targetMaterial.IsGas)
        ++tGas;

    calculate_(proj,
               &pn,
               &projEnergy_AMeV,
               &tarFortran[0],
               &tcompnum,
               &tcompsize,
               &density,
               &tGas,
               &tarThickness_mg_per_cm2,
               &res.EnergyOut_AMeV,
               &res.Range_mg_per_cm2,
               &res.dEdXIn_MeVcm2_per_mg,
               &res.dEdXOut_MeVcm2_per_mg,
               &res.RemainingRange_mg_per_cm2,
               &res.RangeStrag_mg_per_cm2,
               &res.EStrag_AMeV,
               &res.AngStrag_mrad,
               &res.ToF_ns,
               &res.InterpolatedTargetThickness);

    res.EnergyIn_AMeV = projEnergy_AMeV;
    res.EStrag_AMeV *= res.dEdXOut_MeVcm2_per_mg;
    res.ELoss_AMeV = res.EnergyIn_AMeV - res.EnergyOut_AMeV;
    res.AngStrag_mrad *= 1e3;

    return res;
}