#include "R3BNeulandNeutronReconstruction.h"

#include <vector>
#include "TClonesArray.h"
#include "FairLogger.h"

#include "R3BNeulandCluster.h"
#include "R3BNeulandNeutron.h"

#include "Reconstruction2DCalibr.h"

R3BNeulandNeutronReconstruction::R3BNeulandNeutronReconstruction(const TString input, const TString output)
    : FairTask("R3B Neuland Neutron Reconstruction")
    , fEngine(new Neuland::Reconstruction2DCalibr())
    , fInput(input)
    , fOutput(output)
    , fNeutrons(new TClonesArray("R3BNeulandNeutron"))
{
    LOG(INFO) << "Using R3B Neuland Neutron Reconstruction" << FairLogger::endl;
}

R3BNeulandNeutronReconstruction::R3BNeulandNeutronReconstruction(Neuland::ReconstructionEngine* engine,
                                                                 const TString input,
                                                                 const TString output)
    : FairTask("R3B Neuland Neutron Reconstruction")
    , fEngine(engine)
    , fInput(input)
    , fOutput(output)
    , fNeutrons(new TClonesArray("R3BNeulandNeutron"))
{
    LOG(INFO) << "Using R3B Neuland Neutron Reconstruction" << FairLogger::endl;
}

R3BNeulandNeutronReconstruction::~R3BNeulandNeutronReconstruction()
{
    if (fEngine)
    {
        delete fEngine;
    }
    if (fNeutrons)
    {
        delete fNeutrons;
    }
}

InitStatus R3BNeulandNeutronReconstruction::Init()
{
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman)
    {
        LOG(FATAL) << "R3BNeulandNeutronReconstruction::Init: No FairRootManager" << FairLogger::endl;
        return kFATAL;
    }

    // Set Input: TClonesArray of R3BNeulandClusters
    if ((TClonesArray*)ioman->GetObject(fInput) == nullptr)
    {
        LOG(FATAL) << "R3BNeulandNeutronReconstruction::Init No NeulandClusters!" << FairLogger::endl;
        return kFATAL;
    }
    if (!TString(((TClonesArray*)ioman->GetObject(fInput))->GetClass()->GetName()).EqualTo("R3BNeulandCluster"))
    {
        LOG(FATAL) << "R3BNeulandNeutronReconstruction::Init Branch " << fInput
                   << " does not contain R3BNeulandClusters!" << FairLogger::endl;
        return kFATAL;
    }
    fClusters = (TClonesArray*)ioman->GetObject(fInput);

    // Set Output: TClonesArray of R3BNeulandNeutrons
    ioman->Register(fOutput, "Neutrons detected in NeuLAND", fNeutrons, kTRUE);

    fEngine->Init();

    return kSUCCESS;
}

void R3BNeulandNeutronReconstruction::Exec(Option_t*)
{
    fNeutrons->Clear();

    // Input
    const UInt_t nClusters = fClusters->GetEntries();
    std::vector<R3BNeulandCluster*> clusters;
    clusters.reserve(nClusters);
    for (UInt_t i = 0; i < nClusters; i++)
    {
        clusters.push_back((R3BNeulandCluster*)fClusters->At(i));
    }

    // Processing & Output
    for (auto neutron : fEngine->GetNeutrons(clusters))
    {
        // TODO: Not sure of move even does anything in this context
        new ((*fNeutrons)[fNeutrons->GetEntries()]) R3BNeulandNeutron(std::move(neutron));
    }
}

void R3BNeulandNeutronReconstruction::Finish() {}

ClassImp(R3BNeulandNeutronReconstruction)
