#include "R3BNeulandNeutronReconstruction.h"

#include <iostream>
#include <algorithm>
#include <numeric>
#include <cmath>

#include "TClonesArray.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairRtdbRun.h"
#include "FairLogger.h"

#include "R3BNeulandNeutron2DPar.h"
#include "R3BNeulandCluster.h"
#include "R3BNeulandNeutron.h"

R3BNeulandNeutronReconstruction::R3BNeulandNeutronReconstruction()
    : FairTask("R3B NeuLAND NeulandCluster Monitor")
    , fNeutrons(new TClonesArray("R3BNeulandNeutron"))
{
    LOG(INFO) << "Using R3B Neuland Neutron Reconstruction" << FairLogger::endl;
}

R3BNeulandNeutronReconstruction::~R3BNeulandNeutronReconstruction() {}

void R3BNeulandNeutronReconstruction::SetParContainers()
{
    FairRunAna* run = FairRunAna::Instance();
    if (!run)
    {
        LOG(FATAL) << "R3BNeulandNeutronReconstruction::SetParContainers: No analysis run" << FairLogger::endl;
        return;
    }

    FairRuntimeDb* rtdb = run->GetRuntimeDb();
    if (!rtdb)
    {
        LOG(FATAL) << "R3BNeulandNeutronReconstruction::SetParContainers: No runtime database" << FairLogger::endl;
        return;
    }

    fNeulandNeutron2DPar = (R3BNeulandNeutron2DPar*)rtdb->getContainer("R3BNeulandNeutron2DPar");
    if (fNeulandNeutron2DPar == nullptr)
    {
        LOG(FATAL) << "R3BNeulandNeutronReconstruction::SetParContainers: No R3BNeulandNeutron2DPar"
                   << FairLogger::endl;
        return;
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
    if ((TClonesArray*)ioman->GetObject("NeulandClusters") == nullptr)
    {
        LOG(FATAL) << "R3BNeulandNeutronReconstruction::Init No NeulandClusters!" << FairLogger::endl;
        return kFATAL;
    }
    if (!TString(((TClonesArray*)ioman->GetObject("NeulandClusters"))->GetClass()->GetName())
             .EqualTo("R3BNeulandCluster"))
    {
        LOG(FATAL)
            << "R3BNeulandNeutronReconstruction::Init Branch NeulandClusters does not contain R3BNeulandClusters!"
            << FairLogger::endl;
        return kFATAL;
    }
    fClusters = (TClonesArray*)ioman->GetObject("NeulandClusters");

    // Set Output: TClonesArray of R3BNeulandNeutrons
    ioman->Register("NeulandNeutrons", "Neutrons detected in NeuLAND", fNeutrons, kTRUE);

    // FIXME: FairRuntimeDB needs to be forced to load the Data from the second file with Run Id 1
    FairRuntimeDb::instance()->initContainers(FairRuntimeDb::instance()->getCurrentRun()->getRunId(), 1);
    fNeulandNeutron2DPar->printParams();

    return kSUCCESS;
}

void R3BNeulandNeutronReconstruction::Exec(Option_t*)
{
    Reset();

    const UInt_t nClusters = fClusters->GetEntries();

    std::vector<R3BNeulandCluster*> clusters;
    clusters.reserve(nClusters);
    for (UInt_t i = 0; i < nClusters; i++)
    {
        clusters.push_back((R3BNeulandCluster*)fClusters->At(i));
    }

    const Double_t Etot = std::accumulate(clusters.begin(),
                                          clusters.end(),
                                          Double_t(0.),
                                          [](const Double_t a, const R3BNeulandCluster* b)
                                          {
                                              return a + b->GetE();
                                          });

    // Get Number of neutrons using the previously calibrated cuts stored in the parameter file
    const UInt_t nNeutrons = fNeulandNeutron2DPar->GetNeutronMultiplicity(Etot, nClusters);

    // WIP: Sort the digis by likeliness of being a first neutron interaction descending
    std::sort(clusters.begin(),
              clusters.end(),
              [](const R3BNeulandCluster* a, const R3BNeulandCluster* b)
              {
                  return a->GetT() < b->GetT();
              });

    for (UInt_t n = 0; n < nNeutrons && n < clusters.size(); n++)
    {
        const R3BNeulandDigi digi = clusters.at(n)->GetFirstDigi();

        // WIP: ToF Calculation -> Should respect other origin than 0,0,0,0.
        const Double_t s2 = std::pow(digi.GetPosition().X(), 2) + std::pow(digi.GetPosition().Y(), 2) +
                            std::pow(digi.GetPosition().Z(), 2); // cm²
        const Double_t v2 = s2 / std::pow(digi.GetT(), 2);       // ns²

        const Double_t c2 = 898.8;            // cm²/ns²
        const Double_t massNeutron = 939.565; // MeV/c²
        const Double_t ETimeOfFlight = massNeutron * ((1. / std::sqrt(1 - (v2 / c2))) - 1);

        new ((*fNeutrons)[fNeutrons->GetEntries()]) R3BNeulandNeutron(digi.GetT(), ETimeOfFlight, digi.GetPosition());
    }
}

void R3BNeulandNeutronReconstruction::Reset() { fNeutrons->Clear(); }

void R3BNeulandNeutronReconstruction::Finish() {}

ClassImp(R3BNeulandNeutronReconstruction)
