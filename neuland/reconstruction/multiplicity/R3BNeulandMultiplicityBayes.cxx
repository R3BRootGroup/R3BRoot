#include "R3BNeulandMultiplicityBayes.h"
#include "FairRootManager.h"
#include "FairRtdbRun.h"
#include "FairRuntimeDb.h"
#include <numeric>

R3BNeulandMultiplicityBayes::R3BNeulandMultiplicityBayes(std::string_view input, std::string_view output)
    : FairTask("R3BNeulandMultiplicityBayes")
    , fClusters(input)
    , fMultiplicity{ std::make_unique<R3BNeulandMultiplicity>() }
    , multiplicity_par_ptr{ fMultiplicity.get() }
    , fOutputName(output)
{
}

auto R3BNeulandMultiplicityBayes::Init() -> InitStatus
{
    // Input
    fClusters.init();

    // Output
    auto* ioman = FairRootManager::Instance();
    if (ioman == nullptr)
    {
        LOG(fatal) << "R3BNeulandMultiplicityBayes:Init: No FairRootManager";
        return kFATAL;
    }
    ioman->RegisterAny(fOutputName.c_str(), multiplicity_par_ptr, true);

    return kSUCCESS;
}

void R3BNeulandMultiplicityBayes::SetParContainers()
{
    auto* rtdb = FairRuntimeDb::instance();
    if (rtdb == nullptr)
    {
        LOG(fatal) << "R3BNeulandMultiplicityBayes::SetParContainers: No FairRuntimeDb!";
        return;
    }

    fPar = dynamic_cast<R3BNeulandMultiplicityBayesPar*>(rtdb->getContainer("R3BNeulandMultiplicityBayesPar"));
    if (fPar == nullptr)
    {
        LOG(fatal) << "R3BNeulandMultiplicityBayes::SetParContainers: No NeulandMultiplicityBayesPar!";
        return;
    }

    // FIXME: FairRuntimeDB needs to be forced to load the Data from the second file with Run Id 1
    rtdb->initContainers(rtdb->getCurrentRun()->getRunId(), 1);
}

void R3BNeulandMultiplicityBayes::Exec(Option_t*)
{
    const auto& clusters = fClusters.get();
    const auto nClusters = clusters.size();

    if (nClusters == 0)
    {
        fMultiplicity->m.fill(0.);
        fMultiplicity->m[0] = 1.;
        return;
    }

    const auto nHits =
        std::accumulate(clusters.cbegin(),
                        clusters.cend(),
                        0,
                        [](auto size, const R3BNeulandCluster& cluster) { return size + cluster.GetSize(); });
    const auto Edep =
        std::accumulate(clusters.cbegin(),
                        clusters.cend(),
                        0.,
                        [](double energy, const R3BNeulandCluster& cluster) { return energy + cluster.GetE(); });

    fMultiplicity->m = fPar->GetProbabilities(nHits, static_cast<int>(nClusters), static_cast<int>(std::ceil(Edep)));
}

ClassImp(R3BNeulandMultiplicityBayes)
