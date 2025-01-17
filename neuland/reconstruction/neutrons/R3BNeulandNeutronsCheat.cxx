#include "FairRootManager.h"
#include "R3BNeulandNeutronsCheat.h"

R3BNeulandNeutronsCheat::R3BNeulandNeutronsCheat(std::string_view inputMult,
                                                 std::string_view inputHit,
                                                 std::string_view output)
    : FairTask("R3BNeulandNeutronsCheat")
    , fInputMultName(inputMult)
    , fMultiplicity(nullptr)
    , fHits(inputHit)
    , fNeutrons(output)
{
}

auto R3BNeulandNeutronsCheat::Init() -> InitStatus
{
    auto* ioman = FairRootManager::Instance();
    if (ioman == nullptr)
    {
        LOG(fatal) << "TCAInputConnector: No FairRootManager";
        return kFATAL;
    }
    fMultiplicity = ioman->InitObjectAs<const R3BNeulandMultiplicity*>(fInputMultName.c_str());

    fHits.init();
    fNeutrons.init();
    return kSUCCESS;
}

void R3BNeulandNeutronsCheat::Exec(Option_t*)
{
    fNeutrons.clear();

    const auto& hits = fHits.get();
    const auto mult = fMultiplicity->GetMultiplicity();

    for (size_t n = 0; n < hits.size() && n < mult; n++)
    {
        fNeutrons.get().emplace_back(hits.at(n));
    }
}

ClassImp(R3BNeulandNeutronsCheat)
