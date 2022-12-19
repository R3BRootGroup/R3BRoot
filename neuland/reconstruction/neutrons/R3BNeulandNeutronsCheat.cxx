#include "R3BNeulandNeutronsCheat.h"
#include "FairLogger.h"
#include "FairRootManager.h"

R3BNeulandNeutronsCheat::R3BNeulandNeutronsCheat(TString inputMult, TString inputHit, TString output)
    : FairTask("R3BNeulandNeutronsCheat")
    , fInputMultName(std::move(inputMult))
    , fMultiplicity(nullptr)
    , fHits(std::move(inputHit))
    , fNeutrons(std::move(output))
{
}

InitStatus R3BNeulandNeutronsCheat::Init()
{
    auto ioman = FairRootManager::Instance();
    if (ioman == nullptr)
    {
        LOG(fatal) << "TCAInputConnector: No FairRootManager";
        return kFATAL;
    }
    fMultiplicity = ioman->InitObjectAs<const R3BNeulandMultiplicity*>(fInputMultName);

    fHits.Init();
    fNeutrons.Init();
    return kSUCCESS;
}

void R3BNeulandNeutronsCheat::Exec(Option_t*)
{
    fNeutrons.Reset();

    auto hits = fHits.Retrieve();
    const auto mult = fMultiplicity->GetMultiplicity();

    for (size_t n = 0; n < hits.size() && n < mult; n++)
    {
        fNeutrons.Insert(R3BNeulandNeutron(*hits.at(n)));
    }
}

ClassImp(R3BNeulandNeutronsCheat)
