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

R3BAtima::R3BAtima()
{
    fProjectile = std::make_unique<catima::Projectile>();
    fProjectile->Z = 1;
    fProjectile->A = 1;
    fProjectile->T = 100;

    fMaterial = std::make_unique<catima::Material>();
    fMaterial->density(0.0708).thickness(0.106);

    fMaterial->add_element(1, // Z
                           1, // A
                           1.0);
}

const double R3BAtima::GetdEdx(double kinE)
{
    if (!fProjectile || !fMaterial)
    {
        throw std::runtime_error("R3BAtima::GetdEdx: Projectile or Material not initialized");
    }
    fProjectile->T = kinE;

    auto result = catima::calculate(*fProjectile, *fMaterial);
    return result.dEdxi;
}

const double R3BAtima::GetRange(double kinE)
{
    if (!fProjectile || !fMaterial)
    {
        throw std::runtime_error("R3BAtima::GetRange: Projectile or Material not initialized");
    }
    fProjectile->T = kinE;

    auto result = catima::calculate(*fProjectile, *fMaterial);

    return result.range;
}

const double R3BAtima::GetEnergy(double kinE, double thickness)
{
    auto dEdx = this->GetdEdx(kinE);
    return kinE - dEdx * thickness / fProjectile->A;
}

ClassImp(R3BAtima)
