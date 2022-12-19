#include "R3BNeulandMultiplicityBayes.h"
#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRtdbRun.h"
#include "FairRuntimeDb.h"
#include <numeric>
#include <utility>

R3BNeulandMultiplicityBayes::R3BNeulandMultiplicityBayes(TString input, TString output)
    : FairTask("R3BNeulandMultiplicityBayes")
    , fClusters(std::move(input))
    , fMultiplicity(new R3BNeulandMultiplicity())
    , fOutputName(std::move(output))
    , fPar(nullptr)
{
}

R3BNeulandMultiplicityBayes::~R3BNeulandMultiplicityBayes() { delete fMultiplicity; }

InitStatus R3BNeulandMultiplicityBayes::Init()
{
    // Input
    fClusters.Init();

    // Output
    auto ioman = FairRootManager::Instance();
    if (ioman == nullptr)
    {
        LOG(fatal) << "R3BNeulandMultiplicityBayes:Init: No FairRootManager";
        return kFATAL;
    }
    ioman->RegisterAny(fOutputName, fMultiplicity, true);

    return kSUCCESS;
}

void R3BNeulandMultiplicityBayes::SetParContainers()
{
    auto rtdb = FairRuntimeDb::instance();
    if (rtdb == nullptr)
    {
        LOG(fatal) << "R3BNeulandMultiplicityBayes::SetParContainers: No FairRuntimeDb!";
        return;
    }

    fPar = (R3BNeulandMultiplicityBayesPar*)rtdb->getContainer("R3BNeulandMultiplicityBayesPar");
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
    const auto clusters = fClusters.Retrieve();
    const int nClusters = clusters.size();

    if (nClusters == 0)
    {
        fMultiplicity->m.fill(0.);
        fMultiplicity->m[0] = 1.;
        return;
    }

    const int nHits = std::accumulate(
        clusters.cbegin(), clusters.cend(), 0, [](size_t s, const R3BNeulandCluster* c) { return s + c->GetSize(); });
    const int Edep = (int)std::accumulate(
        clusters.cbegin(), clusters.cend(), 0., [](Double_t s, const R3BNeulandCluster* c) { return s + c->GetE(); });

    fMultiplicity->m = fPar->GetProbabilities(nHits, nClusters, Edep);
}

ClassImp(R3BNeulandMultiplicityBayes)
