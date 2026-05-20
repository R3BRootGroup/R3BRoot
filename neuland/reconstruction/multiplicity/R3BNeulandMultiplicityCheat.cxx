#include "R3BNeulandMultiplicityCheat.h"
#include "FairRootManager.h"
#include "R3BException.h"
#include "R3BNeulandMultiplicity.h"
#include <FairTask.h>
#include <Rtypes.h>
#include <RtypesCore.h>
#include <memory>
#include <string_view>

R3BNeulandMultiplicityCheat::R3BNeulandMultiplicityCheat(std::string_view input, std::string_view output)
    : FairTask("R3BNeulandMultiplicityCheat")
    , fPrimaryHits(input)
    , fMultiplicity(std::make_unique<R3BNeulandMultiplicity>())
    , multiplicity_ptr_{ fMultiplicity.get() }
    , fOutputName(output)
{
}

auto R3BNeulandMultiplicityCheat::Init() -> InitStatus
{
    // Input
    fPrimaryHits.init();

    // Output
    auto* ioman = FairRootManager::Instance();
    if (ioman == nullptr)
    {
        throw R3B::runtime_error("TCAInputConnector: No FairRootManager");
    }
    ioman->RegisterAny(fOutputName.c_str(), multiplicity_ptr_, true);

    return kSUCCESS;
}

void R3BNeulandMultiplicityCheat::Exec(Option_t* /*option*/)
{
    fMultiplicity->m.fill(0.);
    fMultiplicity->m.at(fPrimaryHits.size()) = 1.;
}

ClassImp(R3BNeulandMultiplicityCheat)
