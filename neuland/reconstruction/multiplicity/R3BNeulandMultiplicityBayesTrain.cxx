#include "R3BNeulandMultiplicityBayesTrain.h"
#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRtdbRun.h"
#include "FairRuntimeDb.h"
#include <numeric>
#include <utility>

R3BNeulandMultiplicityBayesTrain::R3BNeulandMultiplicityBayesTrain(TString clusters, TString tracks)
    : FairTask("R3BNeulandMultiplicityBayesTrain")
    , fClusters(std::move(clusters))
    , fTracks(std::move(tracks))
    , fPar(nullptr)
//, fPar(new R3BNeulandMultiplicityBayesPar())
{
}

R3BNeulandMultiplicityBayesTrain::~R3BNeulandMultiplicityBayesTrain() {}

InitStatus R3BNeulandMultiplicityBayesTrain::Init()
{
    fClusters.Init();
    fTracks.Init();
    return kSUCCESS;
}

void R3BNeulandMultiplicityBayesTrain::SetParContainers()
{
    auto rtdb = FairRuntimeDb::instance();
    if (rtdb == nullptr)
    {
        LOG(fatal) << "R3BNeulandMultiplicityBayesTrain::Init: No FairRuntimeDb!";
        return;
    }

    fPar = (R3BNeulandMultiplicityBayesPar*)rtdb->getContainer("R3BNeulandMultiplicityBayesPar");
    if (fPar == nullptr)
    {
        LOG(fatal) << "R3BNeulandMultiplicityBayesTrain::Init: No R3BNeulandMultiplicityBayesPar!";
        return;
    }
}

void R3BNeulandMultiplicityBayesTrain::Exec(Option_t*)
{
    const int nPN = fTracks.Retrieve().size();

    const auto clusters = fClusters.Retrieve();
    const int nClusters = clusters.size();

    if (nClusters == 0)
    {
        return;
    }

    const int nHits = std::accumulate(
        clusters.cbegin(), clusters.cend(), 0, [](size_t s, const R3BNeulandCluster* c) { return s + c->GetSize(); });
    const int Edep = (int)std::accumulate(
        clusters.cbegin(), clusters.cend(), 0., [](Double_t s, const R3BNeulandCluster* c) { return s + c->GetE(); });

    fPar->Fill(nPN, nHits, nClusters, Edep);
}

void R3BNeulandMultiplicityBayesTrain::FinishTask()
{
    fPar->Finish();
    auto rtdb = FairRuntimeDb::instance();
    rtdb->addRun(1);
    fPar->setChanged();
    rtdb->writeContainer(fPar, rtdb->getRun(1), nullptr);
}

ClassImp(R3BNeulandMultiplicityBayesTrain)
