#include "R3BNeulandMultiplicityCheat.h"
#include "FairLogger.h"
#include "FairRootManager.h"

R3BNeulandMultiplicityCheat::R3BNeulandMultiplicityCheat(TString input, TString output)
    : FairTask("R3BNeulandMultiplicityCheat")
    , fPrimaryHits(std::move(input))
    , fMultiplicity(new R3BNeulandMultiplicity())
    , fOutputName(std::move(output))
{
}

R3BNeulandMultiplicityCheat::~R3BNeulandMultiplicityCheat() { delete fMultiplicity; }

InitStatus R3BNeulandMultiplicityCheat::Init()
{
    // Input
    fPrimaryHits.Init();

    // Output
    auto ioman = FairRootManager::Instance();
    if (ioman == nullptr)
    {
        LOG(fatal) << "TCAInputConnector: No FairRootManager";
        return kFATAL;
    }
    ioman->RegisterAny(fOutputName, fMultiplicity, true);

    return kSUCCESS;
}

void R3BNeulandMultiplicityCheat::Exec(Option_t*)
{
    fMultiplicity->m.fill(0.);
    fMultiplicity->m[fPrimaryHits.Retrieve().size()] = 1.;
}

ClassImp(R3BNeulandMultiplicityCheat)
