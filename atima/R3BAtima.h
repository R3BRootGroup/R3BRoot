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

#include "R3BAtima.h"

#include <Rtypes.h>
#include <TObject.h>
#include <catima/catima.h>
#include <memory>

class R3BAtima : public TObject
{
  public:
    /** Default constructor */
    R3BAtima();

    /** Destructor **/
    ~R3BAtima() = default;

    struct MaterialComponent
    {
        int a;           // Mass number
        int z;           // Atomic number
        double fraction; // Stoichiometric fraction
    };

    /**
     * Setters for energy loss and range calculations
     * @param kinE      Kinetic energy per nucleon
     * @param thickness Thickness of the layer in g/cm2
     */
    const double GetdEdx(double kinE);

    const double GetRange(double kinE);

    const double GetEnergyLoss(double kinE, double thickness)
    {
        return (kinE - GetEnergy(kinE, thickness)) * fProjectile->A;
    }

    const double GetEnergy(double kinE, double thickness);

    /**
     * Setter of the catima projectile used for calculations
     * @param a    Mass number of the projectile
     * @param z    Charge number of the projectile
     * @param kinE Kinetic energy per nucleon
     * @param q    Charge state
     */
    void SetProjectile(double a, double z, double kinE = 0., double q = 0.)
    {
        fProjectile = std::make_unique<catima::Projectile>(a, z, q);
        if (kinE > 0)
            fProjectile->T = kinE;
    }

    /**
     * Setter of the catima materials
     * @param a    Mass number of the projectile
     * @param z    Charge number of the projectile
     * @param kinE Kinetic energy per nucleon
     * @param q    Charge state
     */
    void SetMaterial(const std::vector<MaterialComponent>& components,
                     const double density_g_cm3,
                     const double thickness_g_cm2,
                     const double ionization_eV = 0.)
    {
        auto material = std::make_unique<catima::Material>();

        for (const auto& c : components)
        {
            material->add_element(c.a, c.z, c.fraction);
        }

        if (density_g_cm3 > 0)
        {
            material->density(density_g_cm3);
        }

        if (thickness_g_cm2 > 0)
        {
            material->thickness(thickness_g_cm2);
        }

        if (ionization_eV > 0.)
        {
            material->I(ionization_eV);
        }

        fMaterial = std::move(material);
    }

  private:
    std::unique_ptr<catima::Material> fMaterial{ nullptr };
    std::unique_ptr<catima::Projectile> fProjectile{ nullptr };

  public:
    ClassDefOverride(R3BAtima, 0);
};
