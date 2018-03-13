#include "R3BNeulandClusterFinder.h"
#include "FairLogger.h"
#include <algorithm>
#include <iostream>
#include <vector>

R3BNeulandClusterFinder::R3BNeulandClusterFinder(const Double_t dx,
                                                 const Double_t dy,
                                                 const Double_t dz,
                                                 const Double_t dt,
                                                 const TString input,
                                                 const TString output)
    : FairTask("R3BNeulandClusterFinder")
    , fDigis(input)
    , fClusters(output)
{
    fClusteringEngine.SetClusteringCondition([=](const R3BNeulandDigi& a, const R3BNeulandDigi& b) {
        return std::abs(a.GetPosition().X() - b.GetPosition().X()) < dx &&
               std::abs(a.GetPosition().Y() - b.GetPosition().Y()) < dy &&
               std::abs(a.GetPosition().Z() - b.GetPosition().Z()) < dz && std::abs(a.GetT() - b.GetT()) < dt;
    });
}

InitStatus R3BNeulandClusterFinder::Init()
{
    try
    {
        fDigis.Init();
        fClusters.Init();
    }
    catch (const std::exception& e)
    {
        LOG(FATAL) << "R3BNeulandClusterFinder: " << e.what() << FairLogger::endl;
    }
    return kSUCCESS;
}

void R3BNeulandClusterFinder::Exec(Option_t*)
{
    // Get the vector of digis for this Event
    auto digis = fDigis.RetrieveObjects();
    const auto nDigis = digis.size();

    // Group them using the clustering condition set above: vector of digis -> vector of vector of digis
    auto clusteredDigis = fClusteringEngine.Clusterize(digis);
    const auto nClusters = clusteredDigis.size();

    // Convert to cluster type vector of vector of digis -> vector of clusters and store
    std::vector<R3BNeulandCluster> clusters;
    clusters.reserve(nClusters);
    std::transform(
        clusteredDigis.begin(), clusteredDigis.end(), std::back_inserter(clusters), [](std::vector<R3BNeulandDigi>& v) {
            return R3BNeulandCluster(std::move(v));
        });
    fClusters.Store(clusters);

    LOG(DEBUG) << "R3BNeulandClusterFinder - nDigis nCluster:" << nDigis << " " << clusters.size() << FairLogger::endl;
}

ClassImp(R3BNeulandClusterFinder);
