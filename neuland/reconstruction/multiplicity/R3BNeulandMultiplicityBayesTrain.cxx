#include "FairRuntimeDb.h"
#include "R3BNeulandMultiplicityBayesTrain.h"
#include <numeric>

R3BNeulandMultiplicityBayesTrain::R3BNeulandMultiplicityBayesTrain(std::string_view clusters, std::string_view tracks)
    : FairTask("R3BNeulandMultiplicityBayesTrain")
    , fClusters{ clusters }
    , fTracks(tracks)
{
}

auto R3BNeulandMultiplicityBayesTrain::Init() -> InitStatus
{
    fClusters.init();
    fTracks.init();
    return kSUCCESS;
}

void R3BNeulandMultiplicityBayesTrain::SetParContainers()
{
    auto* rtdb = FairRuntimeDb::instance();
    if (rtdb == nullptr)
    {
        LOG(fatal) << "R3BNeulandMultiplicityBayesTrain::Init: No FairRuntimeDb!";
        return;
    }

    fPar = dynamic_cast<R3BNeulandMultiplicityBayesPar*>(rtdb->getContainer("R3BNeulandMultiplicityBayesPar"));
    if (fPar == nullptr)
    {
        LOG(fatal) << "R3BNeulandMultiplicityBayesTrain::Init: No R3BNeulandMultiplicityBayesPar!";
        return;
    }
}

void R3BNeulandMultiplicityBayesTrain::Exec(Option_t* /*option*/)
{
    const auto nPN = fTracks.size();

    const auto& clusters = fClusters.get();
    const auto nClusters = clusters.size();

    if (nClusters == 0)
    {
        return;
    }

    const auto nHits =
        std::accumulate(clusters.cbegin(),
                        clusters.cend(),
                        std::size_t{},
                        [](auto sum, const R3BNeulandCluster& cluster) { return sum + cluster.GetSize(); });
    const auto Edep = std::accumulate(clusters.cbegin(),
                                      clusters.cend(),
                                      double{},
                                      [](auto sum, const R3BNeulandCluster& cluster) { return sum + cluster.GetE(); });

    fPar->Fill(static_cast<int>(nPN), static_cast<int>(nHits), static_cast<int>(nClusters), Edep);
}

void R3BNeulandMultiplicityBayesTrain::FinishTask()
{
    fPar->Finish();
    auto* rtdb = FairRuntimeDb::instance();
    rtdb->addRun(1);
    fPar->setChanged();
    rtdb->writeContainer(fPar, rtdb->getRun(1), nullptr);
}

ClassImp(R3BNeulandMultiplicityBayesTrain)
