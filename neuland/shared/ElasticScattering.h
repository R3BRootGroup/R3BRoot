/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2025 Members of R3B Collaboration                     *
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

#include "R3BNeulandCluster.h"

namespace Neuland
{
    auto RecoilProtonEnergy(const R3BNeulandCluster&) -> double;
    auto RecoilScatteringAngle(const R3BNeulandCluster&) -> double;

    /**
     * \brief Calculate neutron kinetic energy from the scattering
     *
     * Calculate the neutron kinetic energy based on the first hits of two clusters, which are both caused by the same
     * neutron one after another.
     * @param first First cluster in time.
     * @param second Second cluster in time.
     * @return Kinetic energy of the neutron.
     */
    auto ScatteredNeutronEnergy(const R3BNeulandCluster& first, const R3BNeulandCluster& second) -> double;

    /**
     * \brief Calculate the scattering angle of the neutron.
     *
     * Calculation of the scattering angle is based on the two clusters, presumably caused by the same neutron one after
     * another.
     * @param first First cluster in time.
     * @param second Second cluster in time.
     * @return \f$\cos(\theta)\f$, where \f$\theta\f$ is the scattering angle with respect to the direction of the first
     * hit of the first cluster.
     */
    auto ScatteredNeutronAngle(const R3BNeulandCluster& first, const R3BNeulandCluster& second) -> double;
    auto NeutronEnergyFromElasticProtonScattering(const R3BNeulandCluster&) -> double;

    /**
     * \brief Calculate neutron kinetic energy from the elastic scattering
     *
     * Calculation of neutron energy based on the elastic scattering with a staionary havey neucleus form the detector
     * material.
     *
     * @param first First cluster in time.
     * @param second Second cluster in time.
     * @param target_mass Rest Mass heavy nucleus, which neutron is scattered with. Unit [MeV], e.g. 11000 for Carbon.
     * @return Kinetic energy value of the impinging neutron
     */
    auto NeutronEnergyFromElasticScattering(const R3BNeulandCluster& first,
                                            const R3BNeulandCluster& second,
                                            double target_mass) -> double;
    auto MaybeElasticScattering(const R3BNeulandCluster& first,
                                const R3BNeulandCluster& second,
                                double targetMass) -> double;
    auto ElasticScatteringTargetMass(const R3BNeulandCluster& first, const R3BNeulandCluster& second) -> double;
} // namespace Neuland
