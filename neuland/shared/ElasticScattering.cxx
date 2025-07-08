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

#include "ElasticScattering.h"
#include "R3BNeulandCluster.h"
#include <cmath>

using std::pow;
using std::sqrt;

namespace Neuland
{
    auto RecoilProtonEnergy(const R3BNeulandCluster& cluster) -> double
    {
        // Range of the proton in material is proportional to its energy E(R) = aR^b
        // Here, the "energy moment" is used as a more robust representation of the neutron track length
        // Values for a and b fitted to simulated data by shooting protons at NeuLAND
        const double a_val = 55.629;
        const double b_val = 0.652103;
        return a_val * std::pow(cluster.GetEnergyMoment(), b_val);
    }

    auto RecoilScatteringAngle(const R3BNeulandCluster& cluster) -> double
    {
        const auto pNUnit = cluster.GetFirstHit().GetPosition().Unit();
        const auto pp_Unit = (cluster.GetEnergyCentroid() - cluster.GetFirstHit().GetPosition()).Unit();
        const double cosTheta = pNUnit.Dot(pp_Unit);
        return cosTheta;
    }

    auto ScatteredNeutronEnergy(const R3BNeulandCluster& first, const R3BNeulandCluster& second) -> double
    {
        static constexpr auto neutron_mass = 938.;                    // Rest Mass Neutron [MeV]
        static constexpr auto light_speed_sq = 898.75517873681758374; // cm²/ns²

        const auto first_hit_distance = second.GetFirstHit().GetPosition() - first.GetFirstHit().GetPosition();
        const double time_diff = second.GetT() - first.GetT();

        const double velocity_sq = first_hit_distance.Mag2() / std::pow(time_diff, 2); // cm²/ns²
        if (velocity_sq > light_speed_sq ||
            velocity_sq > first.GetFirstHit().GetPosition().Mag2() / std::pow(first.GetT(), 2))
        {
            return 0;
        }

        const double gamma = 1. / std::sqrt(1. - (velocity_sq / light_speed_sq));
        const double kinetic_energy = (gamma - 1.) * neutron_mass;
        return kinetic_energy;
    }

    auto ScatteredNeutronAngle(const R3BNeulandCluster& first, const R3BNeulandCluster& second) -> double
    {
        const auto pNUnit = first.GetFirstHit().GetPosition().Unit();
        const auto pN_Unit = (second.GetFirstHit().GetPosition() - first.GetFirstHit().GetPosition()).Unit();
        const double cosTheta = pNUnit.Dot(pN_Unit); // \cos(\theta_{NN'})
        return cosTheta;
    }

    auto NeutronEnergyFromElasticProtonScattering(const R3BNeulandCluster& cluster) -> double
    {
        const double minimalProtonKineticEnergy = 0.; // MeV
        // E0n:       Rest mass proton/neutron
        // En:       Total energy of incoming neutron
        // Ep_:      Total energy of scattered proton
        // Ep_Kin:   Kinetic energy of scattered proton
        // pp_:      Momentum Vector of scattered proton
        // pn:       Momentum Vector of incoming neutron
        // cosTheta: Angle between incoming neutron and scattered proton

        // Use the Energy Moment of the cluster to determine the proton kinetic energy.
        const double Ep_Kin = RecoilProtonEnergy(cluster);
        if (Ep_Kin < minimalProtonKineticEnergy)
        {
            // If the kinetic energy of the proton does not match, this method doesn't work
            return 0;
        }

        // Rest Mass of Proton/Neutron [MeV]
        const double E0n = 938.; // MeV
        const double Ep_ = E0n + Ep_Kin;

        const double cosTheta = RecoilScatteringAngle(cluster);

        /*if (pp_Unit.Z() < 0. || cosTheta < 0.)
        {
            // No scattering in backward directions
            // No scattering angle > 90°
            return 0;
        }*/

        // From Four-Momentum conservation one can deduce:
        // \frac{E_p' + E0n}{E_p' - E0n} \cos^2(\theta_{np'}) = \frac{E_N + E0n}{E_N - E0n}
        const double value = cosTheta * cosTheta * (Ep_ + E0n) / (Ep_ - E0n);
        const double Etot = E0n * ((value + 1.) / (value - 1.));

        // return EKin
        return Etot - E0n;
    }

    auto NeutronEnergyFromElasticScattering(const R3BNeulandCluster& first,
                                            const R3BNeulandCluster& second,
                                            double target_mass) -> double
    {
        const double neutron_m = 938.; // Rest Mass Neutron [MeV]

        const double scattered_neutron_KE = ScatteredNeutronEnergy(first, second) + neutron_m;
        // \cos(\theta_{NN'})
        const double scattered_angle = ScatteredNeutronAngle(first, second);

        // Formula for En. Yes its long, no the pows do not make it slow
        const double neutron_energy =
            (sqrt((pow(scattered_angle, 4) * pow(scattered_neutron_KE, 4) * pow(neutron_m, 2)) -
                  (2 * pow(scattered_angle, 4) * pow(scattered_neutron_KE, 2) * pow(neutron_m, 4)) +
                  (pow(scattered_angle, 4) * pow(neutron_m, 6)) +
                  (pow(scattered_angle, 2) * pow(scattered_neutron_KE, 4) * pow(target_mass, 2)) -
                  (pow(scattered_angle, 2) * pow(scattered_neutron_KE, 4) * pow(neutron_m, 2)) -
                  (2 * pow(scattered_angle, 2) * pow(scattered_neutron_KE, 2) * pow(target_mass, 2) *
                   pow(neutron_m, 2)) +
                  (2 * pow(scattered_angle, 2) * pow(scattered_neutron_KE, 2) * pow(neutron_m, 4)) +
                  (pow(scattered_angle, 2) * pow(target_mass, 2) * pow(neutron_m, 4)) -
                  (pow(scattered_angle, 2) * pow(neutron_m, 6))) +
             pow(scattered_neutron_KE, 2) * target_mass - scattered_neutron_KE * pow(target_mass, 2) -
             scattered_neutron_KE * pow(neutron_m, 2) + target_mass * pow(neutron_m, 2)) /
            (pow(scattered_angle, 2) * pow(scattered_neutron_KE, 2) - pow(scattered_angle, 2) * pow(neutron_m, 2) -
             pow(scattered_neutron_KE, 2) + 2 * scattered_neutron_KE * target_mass - pow(target_mass, 2));

        return neutron_energy - neutron_m;
    }

    auto MaybeElasticScattering(const R3BNeulandCluster& first,
                                const R3BNeulandCluster& second,
                                const double targetMass) -> double
    {
        const double E0n = 938.;
        const double E0k = targetMass;

        const double Etot = first.GetFirstHit().GetEToF() + E0n;
        const double E_neutron = ScatteredNeutronEnergy(first, second) + E0n;
        const double E_kinetic = first.GetE() + targetMass;
        const double cosTheta = ScatteredNeutronAngle(first, second);

        const double zero = (E_neutron * E_neutron) + (E_neutron * E_kinetic) - (Etot * E0k) - (E0n * E0n) -
                            (sqrt((E_neutron * E_neutron - E0n * E0n) * (Etot * Etot - E0n * E0n)) * cosTheta);
        return zero;
    }

    auto ElasticScatteringTargetMass(const R3BNeulandCluster& first, const R3BNeulandCluster& second) -> double
    {
        const double E0n = 938.;

        const double Etot = first.GetFirstHit().GetEToF() + E0n;
        const double E_neutron = ScatteredNeutronEnergy(first, second) + E0n;
        const double Ek_kin = first.GetE();
        const double cosTheta = ScatteredNeutronAngle(first, second);

        const double E0k =
            (E_neutron * E_neutron - E0n * E0n -
             sqrt((E_neutron * E_neutron - E0n * E0n) * (Etot * Etot - E0n * E0n)) * cosTheta + E_neutron * Ek_kin) /
            (Etot - E_neutron);

        return E0k;
    }
} // namespace Neuland
