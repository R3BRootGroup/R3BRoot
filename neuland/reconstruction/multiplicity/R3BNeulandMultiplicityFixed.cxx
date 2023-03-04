#include "R3BNeulandMultiplicityFixed.h"
#include "FairLogger.h"
#include "FairRootManager.h"

R3BNeulandMultiplicityFixed::R3BNeulandMultiplicityFixed(const int mult, TString output)
    : FairTask("R3BNeulandMultiplicityFixed")
    , fMult(mult)
    , fMultiplicity(new R3BNeulandMultiplicity())
    , fOutputName(std::move(output))
{
}

R3BNeulandMultiplicityFixed::~R3BNeulandMultiplicityFixed() { delete fMultiplicity; }

InitStatus R3BNeulandMultiplicityFixed::Init()
{
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

void R3BNeulandMultiplicityFixed::Exec(Option_t*)
{
    fMultiplicity->m.fill(0.);
    fMultiplicity->m[fMult] = 1.;
}

ClassImp(R3BNeulandMultiplicityFixed)
