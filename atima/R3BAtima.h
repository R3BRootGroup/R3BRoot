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

#pragma once

#include "Rtypes.h"
#include <TObject.h>

#include "R3BAtima.h"

#include <catima/catima.h>
#include <memory>
#include <tuple>
#include <vector>

namespace R3B
{
    class Atima : public TObject
    {
      protected:
        std::unique_ptr<catima::Material> fMaterial{ nullptr };
        std::unique_ptr<catima::Projectile> fProjectile{ nullptr };

        double fProjectileMassUma{ -1 };

        double fRangeStepSize{ 0.1 }; // mm

      public:
        /**
         * Initializer of the CATIMA ELossModel wrapper.
         * @param[in] density Density of the material.
         * @param[in] materialComponents Components of the material. They are passed as a vector of tuples (A, Z,
         * stoichiometry).
         */
        Atima(double density, std::vector<std::tuple<int, int, int>> materialComponents);

        double fDensity = 0.;

        virtual double GetdEdx(double energy) const;
        virtual double GetRange(double energyIni, double energyFin = 0) const;
        virtual double GetEnergyLoss(double energyIni, double distance) const
        {
            return energyIni - GetEnergy(energyIni, distance);
        }
        virtual double GetEnergy(double energyIni, double distance) const;

        /**
         * Setter of the catima projectile used for calculations.
         * @param[in] A Mass number of the projectile.
         * @param[in] Z Charge number of the projectile.
         * @param[in] massUma Mass of the projectile in umas.
         */
        void SetProjectile(double A, double Z, double massUma)
        {
            fProjectile = std::make_unique<catima::Projectile>(A, Z);
            fProjectileMassUma = massUma;
        }
        /**
         * Setter of the range step size used for calculations. By default it is set to 0.1mm.
         * @param[in] stepSize The step size used for ranges. It must be input in mm.
         */
        void SetRangeStepSize(double stepSize) { fRangeStepSize = stepSize; }
    };

}; // namespace R3B
