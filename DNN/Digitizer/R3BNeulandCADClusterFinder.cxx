#include "R3BNeulandCADClusterFinder.h"
#include "FairLogger.h"
#include <algorithm>
#include <iostream>
#include <vector>

R3BNeulandCADClusterFinder::R3BNeulandCADClusterFinder(const Double_t dx,
                                                 const Double_t dy,
                                                 const Double_t dz,
                                                 const Double_t dt,
                                                 const TString input,
                                                 const TString output)
    : FairTask("R3BNeulandCADClusterFinder")
    , fDigis(input)
    , fClusters(output)
    , OutputNameTag("")
{
    fClusteringEngine.SetClusteringCondition([=](const R3BNeulandCADHit& a, const R3BNeulandCADHit& b) {
        return std::abs(a.GetPosition().X() - b.GetPosition().X()) < dx &&
               std::abs(a.GetPosition().Y() - b.GetPosition().Y()) < dy &&
               std::abs(a.GetPosition().Z() - b.GetPosition().Z()) < dz && std::abs(a.GetT() - b.GetT()) < dt;
    });
}

InitStatus R3BNeulandCADClusterFinder::Init()
{
    fDigis.SetTag(OutputNameTag);
    fDigis.Init();
    fClusters.SetTag(OutputNameTag);
    fClusters.Init();
    return kSUCCESS;
}

void R3BNeulandCADClusterFinder::Exec(Option_t*)
{
    // Get the vector of digis for this Event
    auto digis = fDigis.RetrieveObjects();
    const auto nDigis = digis.size();

    // Group them using the clustering condition set above: vector of digis -> vector of vector of digis
    auto clusteredDigis = fClusteringEngine.Clusterize(digis);
    const auto nClusters = clusteredDigis.size();

    // Convert to cluster type vector of vector of digis -> vector of clusters and store
    std::vector<R3BNeulandCADCluster> clusters;
    clusters.reserve(nClusters);
    std::transform(
        clusteredDigis.begin(), clusteredDigis.end(), std::back_inserter(clusters), [](std::vector<R3BNeulandCADHit>& v) {
            return R3BNeulandCADCluster(std::move(v));
        });
    fClusters.Store(clusters);

    LOG(DEBUG) << "R3BNeulandCADClusterFinder - nDigis nCluster:" << nDigis << " " << clusters.size() << FairLogger::endl;
}

ClassImp(R3BNeulandCADClusterFinder);
