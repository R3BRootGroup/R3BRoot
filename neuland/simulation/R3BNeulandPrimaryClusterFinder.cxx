#include "R3BNeulandPrimaryClusterFinder.h"
#include <utility>

bool IsPrimaryCluster(const R3BNeulandCluster* c, const std::vector<R3BNeulandHit*>& primaryHits)
{
    for (const auto ph : primaryHits)
    {
        for (const auto& h : c->GetHits())
        {
            if (h == *ph)
            {
                return true;
            }
        }
    }
    return false;
}

R3BNeulandPrimaryClusterFinder::R3BNeulandPrimaryClusterFinder(TString primaryHits,
                                                               TString clusters,
                                                               TString primaryClusters,
                                                               TString secondaryClusters)
    : fPrimaryHits(std::move(primaryHits))
    , fClusters(std::move(clusters))
    , fPrimaryClusters(std::move(primaryClusters))
    , fSecondaryClusters(std::move(secondaryClusters))
{
}

InitStatus R3BNeulandPrimaryClusterFinder::Init()
{
    fPrimaryHits.Init();
    fClusters.Init();
    fPrimaryClusters.Init();
    fSecondaryClusters.Init();

    return kSUCCESS;
}

void R3BNeulandPrimaryClusterFinder::Exec(Option_t*)
{
    const auto primaryHits = fPrimaryHits.Retrieve();
    const auto clusters = fClusters.Retrieve();
    fPrimaryClusters.Reset();
    fSecondaryClusters.Reset();

    for (const auto& cluster : clusters)
    {
        if (IsPrimaryCluster(cluster, primaryHits))
        {
            fPrimaryClusters.Insert(cluster);
        }
        else
        {
            fSecondaryClusters.Insert(cluster);
        }
    }
}
