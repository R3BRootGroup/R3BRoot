#pragma once

#include "CosmicMuon.h"
#include "CosmicMuonDistributions.h"
#include "FairBoxGenerator.h"
#include "FairPrimaryGenerator.h"
#include "R3BNeulandCommon.h"
#include <TRandom3.h>

inline constexpr auto z_pos = 0.;
inline constexpr auto nDP = 13;
inline constexpr auto PID = 2112;

inline auto create_muon_generator(TRandom3& random_gen)
{
    auto detector_box_size = ::R3B::Neuland::DetectorBoxSize{};
    detector_box_size.xmin = -R3B::Neuland::BarLength / 2;
    detector_box_size.xmax = R3B::Neuland::BarLength / 2;
    detector_box_size.ymin = -R3B::Neuland::BarLength / 2;
    detector_box_size.ymax = R3B::Neuland::BarLength / 2;
    detector_box_size.zmin = z_pos - (R3B::Neuland::BarSize_Z * nDP);
    detector_box_size.zmax = z_pos + (R3B::Neuland::BarSize_Z * nDP);
    // detector_box_size.xmin = -R3B::Neuland::BarLength / 2;
    // detector_box_size.xmax = R3B::Neuland::BarLength / 2;
    // detector_box_size.ymin = -R3B::Neuland::BarLength / 2;
    // detector_box_size.ymax = R3B::Neuland::BarLength / 2;
    // detector_box_size.zmin = 1650.;
    // detector_box_size.zmax = 1650. + (2 * R3B::Neuland::BarSize_Z * nDP);

    auto angle_dist = R3B::Neuland::AngleDist{};
    auto energy_dist = R3B::Neuland::EnergyDist{};
    auto position_dist = R3B::Neuland::PositionDist{};

    // auto const mean = 3000.;
    // auto const sigma = 400.;
    auto const mean = 30.;
    auto const sigma = 0.2;
    energy_dist.set_mean_sigma(mean, sigma);

    position_dist.set_box_size(detector_box_size);

    auto CosmicMuonGenerator = R3B::Neuland::CreateTrackGenerator(angle_dist, energy_dist, position_dist);

    CosmicMuonGenerator->set_rd_engine(&random_gen);

    auto primGen = std::make_unique<FairPrimaryGenerator>();
    primGen->AddGenerator(CosmicMuonGenerator.release());
    return primGen;
}

inline auto create_box_generator(double energy, int multi)
{
    // Primary particle generator
    auto boxGen = std::make_unique<FairBoxGenerator>(PID, multi);
    boxGen->SetXYZ(0, 0, 0.);
    boxGen->SetThetaRange(0., 3.);
    boxGen->SetPhiRange(0., 360.);
    boxGen->SetEkinRange(energy, energy);
    auto primGen = std::make_unique<FairPrimaryGenerator>();
    primGen->AddGenerator(boxGen.release());
    return primGen;
}
