#include "CosmicMuon.h"
#include "CosmicMuonDistributions.h"

namespace R3B::Neuland
{
    TrackGenerator::TrackGenerator()
        : TrackGenerator{ AngleDist{}, EnergyDist{}, PositionDist{} }
    {
    }
} // namespace R3B::Neuland
