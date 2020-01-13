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

#ifndef R3BATIMA_H
#define R3BATIMA_H

#include "Rtypes.h"

#include <array>
#include <vector>

struct R3BAtimaMaterialCompound
{
    R3BAtimaMaterialCompound(const Double_t mass_u, const Double_t charge_e)
        : Mass_u(mass_u)
        , Charge_e(charge_e)
        , Ratio(1.)
    {
    }
    R3BAtimaMaterialCompound(const Double_t mass_u, const Double_t charge_e, const Double_t ratio)
        : Mass_u(mass_u)
        , Charge_e(charge_e)
        , Ratio(ratio)
    {
    }
    R3BAtimaMaterialCompound(const std::array<Double_t, 3>& arr)
        : Mass_u(arr[0])
        , Charge_e(arr[1])
        , Ratio(arr[2])
    {
    }

    bool operator==(const R3BAtimaMaterialCompound& other) const
    {
        return (Mass_u == other.Mass_u && Charge_e == other.Charge_e && Ratio == other.Ratio);
    }
    bool operator!=(const R3BAtimaMaterialCompound& other) const { return !(*this == other); }

    Double_t Mass_u;
    Double_t Charge_e;
    Double_t Ratio;
};

struct R3BAtimaTargetMaterial
{
    static const R3BAtimaTargetMaterial Air;
    static const R3BAtimaTargetMaterial LH2;
    static const R3BAtimaTargetMaterial Si;
    static const R3BAtimaTargetMaterial BC400;

    R3BAtimaTargetMaterial(const std::vector<R3BAtimaMaterialCompound>& compounds,
                           const Double_t density,
                           const Bool_t isGas)
        : Compounds(compounds)
        , Density(density)
        , IsGas(isGas)
    {
    }

    std::vector<R3BAtimaMaterialCompound> Compounds;
    Double_t Density;
    Bool_t IsGas;
};

struct R3BAtimaTransportResult
{
    Double_t EnergyIn_AMeV;
    Double_t EnergyOut_AMeV;
    Double_t ELoss_AMeV;
    Double_t EStrag_AMeV;
    Double_t AngStrag_mrad;
    Double_t Range_mg_per_cm2;
    Double_t RangeStrag_mg_per_cm2;
    Double_t RemainingRange_mg_per_cm2;
    Double_t dEdXIn_MeVcm2_per_mg;
    Double_t dEdXOut_MeVcm2_per_mg;
    Double_t ToF_ns;
    Double_t InterpolatedTargetThickness;
};

class R3BAtima
{
  public:
    static R3BAtimaTransportResult Calculate(Double_t projMass_u,
                                             Double_t projCharge_e,
                                             Double_t projEnergy_AMeV,
                                             const R3BAtimaTargetMaterial& targetMaterial,
                                             Double_t tarThickness_mg_per_cm2);

    static R3BAtimaTransportResult Calculate_mm(Double_t projMass_u,
                                                Double_t projCharge_e,
                                                Double_t projEnergy_AMeV,
                                                const R3BAtimaTargetMaterial& targetMaterial,
                                                Double_t tarThickness_mm)
    {
        return Calculate(
            projMass_u, projCharge_e, projEnergy_AMeV, targetMaterial, targetMaterial.Density * tarThickness_mm * 100);
    }
};

#endif
