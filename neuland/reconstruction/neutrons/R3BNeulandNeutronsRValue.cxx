#include "R3BNeulandNeutronsRValue.h"
#include "R3BException.h"
#include "R3BNeulandCluster.h"
#include "R3BNeulandMultiplicity.h"
#include <FairRootManager.h>
#include <FairTask.h>
#include <IsElastic.h>
#include <Rtypes.h>
#include <RtypesCore.h>
#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <map>
#include <string_view>
#include <vector>

R3BNeulandNeutronsRValue::R3BNeulandNeutronsRValue(double EkinRefMeV,
                                                   std::string_view inputMult,
                                                   std::string_view inputCluster,
                                                   std::string_view output)
    : FairTask("R3BNeulandNeutronsRValue")
    , fEkinRefMeV(EkinRefMeV)
    , fInputMultName(inputMult)
    , fMultiplicity(nullptr)
    , fClusters(inputCluster)
    , fNeutrons(output)
{
}

auto R3BNeulandNeutronsRValue::Init() -> InitStatus
{
    auto* ioman = FairRootManager::Instance();
    if (ioman == nullptr)
    {
        throw R3B::runtime_error("TCAInputConnector: No FairRootManager");
    }
    fMultiplicity = ioman->InitObjectAs<const R3BNeulandMultiplicity*>(fInputMultName.c_str());

    fClusters.init();
    fNeutrons.init();
    return kSUCCESS;
}

void R3BNeulandNeutronsRValue::Exec(Option_t* /*option*/)
{
    fNeutrons.clear();
    cluster_buffer_.clear();

    cluster_buffer_ = fClusters.get();

    // Recreate R3BNeutronTracker2D Advanced Method
    // FilterClustersByElasticScattering(clusters); // Check all pairs of clusters. Remove clusters from elastic
    // scattering FilterClustersByEnergyDeposit(clusters); FilterClustersByKineticEnergy(clusters);
    SortClustersByRValue(cluster_buffer_);
    PrioritizeTimeWiseFirstCluster(cluster_buffer_);

    const auto mult = fMultiplicity->GetMultiplicity();
    for (size_t index = 0; index < cluster_buffer_.size() && index < mult; index++)
    {
        fNeutrons.get().emplace_back(cluster_buffer_.at(index));
    }
}

void R3BNeulandNeutronsRValue::SortClustersByRValue(std::vector<R3BNeulandCluster>& clusters) const
{
    std::sort(clusters.begin(),
              clusters.end(),
              [this](const R3BNeulandCluster& one, const R3BNeulandCluster& other)
              { return one.GetRECluster(fEkinRefMeV) < other.GetRECluster(fEkinRefMeV); });
}

void R3BNeulandNeutronsRValue::PrioritizeTimeWiseFirstCluster(std::vector<R3BNeulandCluster>& clusters)
{
    auto timewiseFirstCluster = std::min_element(clusters.begin(),
                                                 clusters.end(),
                                                 [](const R3BNeulandCluster& one, const R3BNeulandCluster& other)
                                                 { return one.GetT() < other.GetT(); });
    // Put first cluster in front
    std::rotate(clusters.begin(), timewiseFirstCluster, timewiseFirstCluster + 1);
}

void R3BNeulandNeutronsRValue::FilterClustersByEnergyDeposit(std::vector<R3BNeulandCluster>& clusters)
{
    static constexpr auto threshold = 2.5;
    clusters.erase(std::remove_if(clusters.begin(),
                                  clusters.end(),
                                  [&](const R3BNeulandCluster& cluster) { return cluster.GetE() < threshold; }),
                   clusters.end());
}

void R3BNeulandNeutronsRValue::FilterClustersByKineticEnergy(std::vector<R3BNeulandCluster>& clusters) const
{
    static constexpr auto threshold = 0.05;
    clusters.erase(std::remove_if(clusters.begin(),
                                  clusters.end(),
                                  [this](const R3BNeulandCluster& cluster)
                                  { return std::abs(cluster.GetEToF() - fEkinRefMeV) / fEkinRefMeV > threshold; }),
                   clusters.end());
}

void R3BNeulandNeutronsRValue::FilterClustersByElasticScattering(std::vector<R3BNeulandCluster>& clusters)
{
    std::map<const R3BNeulandCluster*, bool> marked;

    for (const auto& cluster : clusters)
    {
        marked[&cluster] = false;
    }

    for (const auto& one_cluster : clusters)
    {
        for (const auto& other_cluster : clusters)
        {
            if (&one_cluster != &other_cluster && one_cluster.GetT() < other_cluster.GetT() &&
                Neuland::IsElastic(&one_cluster, &other_cluster))
            {
                marked[&other_cluster] = true;
            }
        }
    }

    clusters.erase(
        std::remove_if(
            clusters.begin(), clusters.end(), [&](const R3BNeulandCluster& cluster) { return marked.at(&cluster); }),
        clusters.end());
}

ClassImp(R3BNeulandNeutronsRValue)
