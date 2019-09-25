#include "R3BNeulandNeutronReconstruction.h"
#include "FairLogger.h"

R3BNeulandNeutronReconstruction::R3BNeulandNeutronReconstruction(Neuland::ReconstructionEngine* engine,
                                                                 TString input,
                                                                 TString output)
    : FairTask("R3B Neuland Neutron Reconstruction")
    , fEngine(engine)
    , fClusters(input)
    , fNeutrons(output)
{
    LOG(INFO) << "Using R3B Neuland Neutron Reconstruction";
}

InitStatus R3BNeulandNeutronReconstruction::Init()
{
    fEngine->Init();
    fClusters.Init();
    fNeutrons.Init();
    return kSUCCESS;
}

void R3BNeulandNeutronReconstruction::Exec(Option_t*)
{
    fNeutrons.Reset();
    auto clusters = fClusters.Retrieve();
    auto neutrons = fEngine->GetNeutrons(clusters);
    fNeutrons.Insert(neutrons);
}

ClassImp(R3BNeulandNeutronReconstruction)
