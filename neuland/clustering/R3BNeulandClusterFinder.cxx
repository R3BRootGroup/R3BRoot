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

#include "R3BNeulandClusterFinder.h"
#include "R3BLogger.h"
#include "R3BNeulandHit.h"
#include <FairTask.h>
#include <Rtypes.h>
#include <RtypesCore.h>
#include <cstdlib>
#include <fmt/core.h>
#include <string_view>
#include <utility>
#include <vector>

R3BNeulandClusterFinder::R3BNeulandClusterFinder(const double dist_x,
                                                 const double dist_y,
                                                 const double dist_z,
                                                 const double dist_t,
                                                 const std::string_view input,
                                                 const std::string_view output)
    : FairTask("R3BNeulandClusterFinder")
    , fDigis(input)
    , fClusters(output)
{
    fClusteringEngine.SetClusteringCondition(
        [=](const R3BNeulandHit& one, const R3BNeulandHit& other)
        {
            return std::abs(one.GetPosition().X() - other.GetPosition().X()) < dist_x &&
                   std::abs(one.GetPosition().Y() - other.GetPosition().Y()) < dist_y &&
                   std::abs(one.GetPosition().Z() - other.GetPosition().Z()) < dist_z &&
                   std::abs(one.GetT() - other.GetT()) < dist_t;
        });
}

auto R3BNeulandClusterFinder::Init() -> InitStatus
{
    fDigis.init();
    fClusters.init();
    return kSUCCESS;
}

void R3BNeulandClusterFinder::Exec(Option_t* /*option*/)
{
    fClusters.clear();
    neuland_hits_buffer_.clear();
    clustered_hits_buffer_.clear();

    neuland_hits_buffer_ = fDigis.get();
    const auto nDigis = neuland_hits_buffer_.size();

    // Group them using the clustering condition set above: vector of digis -> vector of vector of digis
    clustered_hits_buffer_ = fClusteringEngine.Clusterize(neuland_hits_buffer_);
    const auto nClusters = clustered_hits_buffer_.size();

    R3BLOG(debug, fmt::format("R3BNeulandClusterFinder - nDigis nCluster: {} {}", nDigis, nClusters));

    for (auto& cluster : clustered_hits_buffer_)
    {
        fClusters.get().emplace_back(std::move(cluster));
    }
}

ClassImp(R3BNeulandClusterFinder);
