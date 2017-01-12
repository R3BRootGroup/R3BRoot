#include "R3BNeulandNeutronReconstruction.h"

#include "FairLogger.h"
#include "ReconstructionOperation.h"
#include <exception>

R3BNeulandNeutronReconstruction::R3BNeulandNeutronReconstruction(Neuland::ReconstructionEngine* engine,
                                                                 const TString input,
                                                                 const TString output)
    : FairTask("R3B Neuland Neutron Reconstruction")
    , fEngine(engine)
    , fClusters(input)
    , fNeutrons(output)
{
    LOG(INFO) << "Using R3B Neuland Neutron Reconstruction" << FairLogger::endl;
}

InitStatus R3BNeulandNeutronReconstruction::Init()
{
    try
    {
        fEngine->Init();
        fClusters.Init();
        fNeutrons.Init();
    }
    catch (const std::exception& e)
    {
        LOG(FATAL) << "R3BNeulandNeutronReconstruction" << e.what() << FairLogger::endl;
    }

    return kSUCCESS;
}

void R3BNeulandNeutronReconstruction::Exec(Option_t*)
{
    auto clusters = fClusters.Retrieve();
    auto neutrons = fEngine->GetNeutrons(clusters);
    fNeutrons.Store(neutrons);
}

ClassImp(R3BNeulandNeutronReconstruction)
