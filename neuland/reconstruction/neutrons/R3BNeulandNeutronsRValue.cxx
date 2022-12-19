#include "R3BNeulandNeutronsRValue.h"
#include "FairLogger.h"
#include "FairRootManager.h"
#include <IsElastic.h>
#include <utility>

R3BNeulandNeutronsRValue::R3BNeulandNeutronsRValue(double EkinRefMeV,
                                                   TString inputMult,
                                                   TString inputCluster,
                                                   TString output)
    : FairTask("R3BNeulandNeutronsRValue")
    , fEkinRefMeV(EkinRefMeV)
    , fInputMultName(std::move(inputMult))
    , fMultiplicity(nullptr)
    , fClusters(std::move(inputCluster))
    , fNeutrons(std::move(output))
{
}

InitStatus R3BNeulandNeutronsRValue::Init()
{
    auto ioman = FairRootManager::Instance();
    if (ioman == nullptr)
    {
        LOG(fatal) << "TCAInputConnector: No FairRootManager";
        return kFATAL;
    }
    fMultiplicity = ioman->InitObjectAs<const R3BNeulandMultiplicity*>(fInputMultName);

    fClusters.Init();
    fNeutrons.Init();
    return kSUCCESS;
}

void R3BNeulandNeutronsRValue::Exec(Option_t*)
{
    fNeutrons.Reset();

    auto clusters = fClusters.Retrieve();

    // Recreate R3BNeutronTracker2D Advanced Method
    // FilterClustersByElasticScattering(clusters); // Check all pairs of clusters. Remove clusters from elastic
    // scattering FilterClustersByEnergyDeposit(clusters); FilterClustersByKineticEnergy(clusters);
    SortClustersByRValue(clusters);
    PrioritizeTimeWiseFirstCluster(clusters);

    const auto mult = fMultiplicity->GetMultiplicity();
    for (size_t n = 0; n < clusters.size() && n < mult; n++)
    {
        fNeutrons.Insert(R3BNeulandNeutron(*clusters.at(n)));
    }
}

void R3BNeulandNeutronsRValue::SortClustersByRValue(std::vector<R3BNeulandCluster*>& clusters) const
{
    std::sort(clusters.begin(), clusters.end(), [&](const R3BNeulandCluster* a, const R3BNeulandCluster* b) {
        return a->GetRECluster(fEkinRefMeV) < b->GetRECluster(fEkinRefMeV);
    });
}

void R3BNeulandNeutronsRValue::PrioritizeTimeWiseFirstCluster(std::vector<R3BNeulandCluster*>& clusters) const
{
    auto timewiseFirstCluster =
        std::min_element(clusters.begin(), clusters.end(), [](const R3BNeulandCluster* a, const R3BNeulandCluster* b) {
            return a->GetT() < b->GetT();
        });
    // Put first cluster in front
    std::rotate(clusters.begin(), timewiseFirstCluster, timewiseFirstCluster + 1);
}

void R3BNeulandNeutronsRValue::FilterClustersByEnergyDeposit(std::vector<R3BNeulandCluster*>& clusters) const
{
    clusters.erase(
        std::remove_if(clusters.begin(), clusters.end(), [&](const R3BNeulandCluster* a) { return a->GetE() < 2.5; }),
        clusters.end());
}

void R3BNeulandNeutronsRValue::FilterClustersByKineticEnergy(std::vector<R3BNeulandCluster*>& clusters) const
{
    clusters.erase(std::remove_if(clusters.begin(),
                                  clusters.end(),
                                  [&](const R3BNeulandCluster* a) {
                                      return std::abs(a->GetEToF() - fEkinRefMeV) / fEkinRefMeV > 0.05;
                                  }),
                   clusters.end());
}

void R3BNeulandNeutronsRValue::FilterClustersByElasticScattering(std::vector<R3BNeulandCluster*>& clusters) const
{
    std::map<const R3BNeulandCluster*, bool> marked;

    for (const auto& c : clusters)
    {
        marked[c] = false;
    }

    for (const auto& a : clusters)
    {
        for (const auto& b : clusters)
        {
            if (a != b && a->GetT() < b->GetT() && Neuland::IsElastic(a, b))
            {
                marked[b] = true;
            }
        }
    }

    clusters.erase(
        std::remove_if(clusters.begin(), clusters.end(), [&](const R3BNeulandCluster* a) { return marked.at(a); }),
        clusters.end());
}

ClassImp(R3BNeulandNeutronsRValue)
