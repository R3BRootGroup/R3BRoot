#include "R3BNeulandMultiplicityCalorimetric.h"
#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRtdbRun.h"
#include "FairRuntimeDb.h"
#include <numeric>
#include <utility>

R3BNeulandMultiplicityCalorimetric::R3BNeulandMultiplicityCalorimetric(TString input, TString output)
    : FairTask("R3BNeulandMultiplicityCalorimetric")
    , fClusters(std::move(input))
    , fMultiplicity(new R3BNeulandMultiplicity())
    , fOutputName(std::move(output))
    , fPar(nullptr)
{
}

R3BNeulandMultiplicityCalorimetric::~R3BNeulandMultiplicityCalorimetric() { delete fMultiplicity; }

InitStatus R3BNeulandMultiplicityCalorimetric::Init()
{
    // Input
    fClusters.Init();

    // Output
    auto ioman = FairRootManager::Instance();
    if (ioman == nullptr)
    {
        LOG(fatal) << "TCAInputConnector: No FairRootManager";
        return kFATAL;
    }
    ioman->RegisterAny(fOutputName, fMultiplicity, true);

    // Parameter
    auto rtdb = FairRuntimeDb::instance();
    if (rtdb == nullptr)
    {
        LOG(fatal) << "R3BNeulandMultiplicityCalorimetric::Init: No FairRuntimeDb!";
        return kFATAL;
    }
    fPar = (R3BNeulandMultiplicityCalorimetricPar*)rtdb->getContainer("R3BNeulandMultiplicityCalorimetricPar");
    if (fPar == nullptr)
    {
        LOG(fatal) << "R3BNeulandMultiplicityCalorimetric::Init: No R3BNeulandMultiplicityCalorimetricPar!";
        return kFATAL;
    }
    // FIXME: FairRuntimeDB needs to be forced to load the Data from the second file with Run Id 1
    rtdb->initContainers(rtdb->getCurrentRun()->getRunId(), 1);

    return kSUCCESS;
}

void R3BNeulandMultiplicityCalorimetric::Exec(Option_t*)
{
    fMultiplicity->m.fill(0.);

    const auto clusters = fClusters.Retrieve();
    const auto Etot =
        std::accumulate(clusters.cbegin(), clusters.cend(), 0., [](const Double_t a, const R3BNeulandCluster* b) {
            return a + b->GetE();
        });
    const auto nClusters = clusters.size();

    // Calorimetric Multiplicity is One-Hot
    fMultiplicity->m[fPar->GetNeutronMultiplicity(Etot, nClusters)] = 1.;
}

ClassImp(R3BNeulandMultiplicityCalorimetric)
