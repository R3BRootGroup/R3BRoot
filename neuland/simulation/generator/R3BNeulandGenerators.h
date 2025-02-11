#pragma once

#include "CosmicMuon.h"
#include "CosmicMuonDistributions.h"
#include "FairBoxGenerator.h"
#include "FairPrimaryGenerator.h"
#include "R3BNeulandCommon.h"
#include <TRandom3.h>

// NOTE: what's the purpose of this file? Just function definitions?

inline auto create_muon_generator(TRandom3& random_gen,
                                  int num_dp,
                                  double energy,
                                  const R3B::Neuland::XYZCoordinate& pos)
{
    constexpr auto PID = 2112;
    auto detector_box_size = ::R3B::Neuland::DetectorBoxSize{};
    detector_box_size.xmin = pos.x - R3B::Neuland::BarLength / 2;
    detector_box_size.xmax = pos.x + R3B::Neuland::BarLength / 2;
    detector_box_size.ymin = pos.y - R3B::Neuland::BarLength / 2;
    detector_box_size.ymax = pos.y + R3B::Neuland::BarLength / 2;
    detector_box_size.zmin = pos.z - (R3B::Neuland::BarSize_Z * num_dp);
    detector_box_size.zmax = pos.z + (R3B::Neuland::BarSize_Z * num_dp);

    auto angle_dist = R3B::Neuland::AngleDist{};
    auto energy_dist = R3B::Neuland::EnergyDist{};
    auto position_dist = R3B::Neuland::PositionDist{};

    auto const mean = energy;
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
    constexpr auto PID = 2112;
    auto boxGen = std::make_unique<FairBoxGenerator>(PID, multi);
    boxGen->SetXYZ(0, 0, 0.);
    boxGen->SetThetaRange(0., 3.);
    boxGen->SetPhiRange(0., 360.);
    boxGen->SetEkinRange(energy, energy);
    auto primGen = std::make_unique<FairPrimaryGenerator>();
    primGen->AddGenerator(boxGen.release());
    return primGen;
}
