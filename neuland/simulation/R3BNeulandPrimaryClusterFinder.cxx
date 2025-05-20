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

#include "R3BNeulandPrimaryClusterFinder.h"
#include "R3BNeulandCluster.h"
#include "R3BNeulandHit.h"
#include <FairTask.h>
#include <RtypesCore.h>
#include <string_view>
#include <vector>

namespace
{
    auto IsPrimaryCluster(const R3BNeulandCluster& cluster, const std::vector<R3BNeulandHit>& primaryHits) -> bool
    {
        for (const auto& primary_hit : primaryHits)
        {
            for (const auto& hit : cluster.GetHits())
            {
                if (hit == primary_hit)
                {
                    return true;
                }
            }
        }
        return false;
    }

} // namespace

// NOLINTNEXTLINE: bugprone-easily-swappable-parameters
R3BNeulandPrimaryClusterFinder::R3BNeulandPrimaryClusterFinder(std::string_view primaryHits,
                                                               std::string_view clusters,
                                                               std::string_view primaryClusters,
                                                               std::string_view secondaryClusters)
    : fPrimaryHits{ primaryHits }
    , fClusters{ clusters }
    , fPrimaryClusters{ primaryClusters }
    , fSecondaryClusters{ secondaryClusters }
{
}

auto R3BNeulandPrimaryClusterFinder::Init() -> InitStatus
{
    fPrimaryHits.init();
    fClusters.init();
    fPrimaryClusters.init();
    fSecondaryClusters.init();

    return kSUCCESS;
}

void R3BNeulandPrimaryClusterFinder::Exec(Option_t* /*option*/)
{
    const auto& primaryHits = fPrimaryHits.get();
    const auto& clusters = fClusters.get();
    fPrimaryClusters.clear();
    fSecondaryClusters.clear();

    for (const auto& cluster : clusters)
    {
        if (IsPrimaryCluster(cluster, primaryHits))
        {
            fPrimaryClusters.get().push_back(cluster);
        }
        else
        {
            fSecondaryClusters.get().push_back(cluster);
        }
    }
}
