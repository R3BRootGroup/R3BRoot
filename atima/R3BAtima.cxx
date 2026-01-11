/******************************************************************************
 *   Copyright (C) 2020 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2020-2026 Members of R3B Collaboration                     *
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

#include <FairLogger.h>

R3B::Atima::Atima(double density, std::vector<std::tuple<int, int, int>> materialComponents)
{
    fMaterial = std::make_unique<catima::Material>();

    for (auto materialComponent : materialComponents)
        fMaterial->add_element(
            std::get<0>(materialComponent), std::get<1>(materialComponent), std::get<2>(materialComponent));
}

double R3B::Atima::GetdEdx(double energy) const
{
    if (fProjectile == nullptr)
    {
        LOG(warning)
            << " Warning in AtTools::AtELossCATIMA::GetdEdx : The projectile was not set! GetdEdx will return 0!";
        return 0;
    }

    if (fProjectileMassUma <= 0)
    {
        LOG(error) << " Error in AtTools::AtELossCATIMA::GetdEdx : The projectile's mass in umas can not be <= 0! "
                      "GetdEdx will return 0!";
        return 0;
    }

    catima::Result result = catima::calculate(*fProjectile, *fMaterial, energy / fProjectileMassUma);
    double dEdx = result.dEdxi * fDensity;
    return dEdx;
}

double R3B::Atima::GetRange(double energyIni, double energyFin) const
{
    if (energyFin < 0)
    {
        LOG(warning) << " Warning in AtTools::AtELossCATIMA::GetRange : The final energy was set to a negative value! "
                        "Setting energyFin to 0!";
        energyFin = 0;
    }

    if (energyFin == 0)
    {
        fProjectile->T = energyIni / fProjectileMassUma;
        return catima::range(*fProjectile, *fMaterial) / fDensity * 10.;
    }

    double remainingEnergy{ energyIni };
    double range{ 0 };
    while (remainingEnergy > energyFin)
    {
        catima::Result result = catima::calculate(*fProjectile, *fMaterial, remainingEnergy / fProjectileMassUma);
        double dEdx = result.dEdxi * fDensity;
        double DE = dEdx * fRangeStepSize / 10.;

        if (remainingEnergy - energyFin > DE)
        {
            range += fRangeStepSize;
            remainingEnergy -= DE;
        }
        else
        {
            range += (remainingEnergy - energyFin) / dEdx * 10.;
            remainingEnergy = energyFin;
            break;
        }
    }
    return range;
}

double R3B::Atima::GetEnergy(double energyIni, double distance) const
{
    double remainingEnergy{ energyIni };
    double range{ 0 };
    while (range < distance)
    {
        catima::Result result = catima::calculate(*fProjectile, *fMaterial, remainingEnergy / fProjectileMassUma);
        double dEdx = result.dEdxi * fDensity;
        double DE{};

        if (range + fRangeStepSize < distance)
        {
            DE = dEdx * fRangeStepSize / 10.;
            range += fRangeStepSize;
            remainingEnergy -= DE;
        }
        else
        {
            DE = dEdx * (distance - range) / 10.;
            range = distance;
            remainingEnergy -= DE;
            break;
        }
    }

    return remainingEnergy;
}
