#include <algorithm>
#include <iostream>
#include <vector>

#include "TMath.h"
#include "TH1F.h"
#include "TH2F.h"

#include "FairRootManager.h"
#include "FairLogger.h"

#include "R3BNeulandDigi.h"
#include "R3BNeulandCluster.h"
#include "R3BNeulandClusterFinder.h"

R3BNeulandClusterFinder::R3BNeulandClusterFinder(const Double_t dx,
                                                 const Double_t dy,
                                                 const Double_t dz,
                                                 const Double_t dt,
                                                 const TString input,
                                                 const TString output)
    : FairTask("R3BNeulandClusterFinder")
    , fClusteringEngine(Neuland::ClusteringEngine<R3BNeulandDigi>())
    , fInput(input)
    , fOutput(output)
    , fClusters(new TClonesArray("R3BNeulandCluster"))
{
    fClusteringEngine.SetClusteringCondition([=](const R3BNeulandDigi& a, const R3BNeulandDigi& b)
                                             {
                                                 return TMath::Abs(a.GetPosition().X() - b.GetPosition().X()) < dx &&
                                                        TMath::Abs(a.GetPosition().Y() - b.GetPosition().Y()) < dy &&
                                                        TMath::Abs(a.GetPosition().Z() - b.GetPosition().Z()) < dz &&
                                                        TMath::Abs(a.GetT() - b.GetT()) < dt;
                                             });
}

InitStatus R3BNeulandClusterFinder::Init()
{
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman)
    {
        LOG(FATAL) << "R3BNeulandClusterFinder::Init: No FairRootManager" << FairLogger::endl;
        return kFATAL;
    }

    // Set Input: TClonesArray of R3BNeulandDigi
    if ((TClonesArray*)ioman->GetObject(fInput) == nullptr)
    {
        LOG(FATAL) << "R3BNeulandClusterFinder::Init No NeulandDigis!" << FairLogger::endl;
        return kFATAL;
    }
    if (!TString(((TClonesArray*)ioman->GetObject(fInput))->GetClass()->GetName()).EqualTo("R3BNeulandDigi"))
    {
        LOG(FATAL) << "R3BNeulandClusterFinder::Init Branch " << fInput << " does not contain R3BNeulandDigis!"
                   << FairLogger::endl;
        return kFATAL;
    }
    fDigis = (TClonesArray*)ioman->GetObject(fInput);

    // Set Output: TClonesArray of R3BNeulandCluster
    ioman->Register(fOutput, "Clusters in NeuLAND", fClusters.get(), kTRUE);

    LOG(INFO) << "R3BNeulandClusterFinder " << fInput << " -> " << fOutput << FairLogger::endl;

    return kSUCCESS;
}

void R3BNeulandClusterFinder::Exec(Option_t*)
{
    fClusters->Clear();

    // Input
    const UInt_t nDigis = fDigis->GetEntries();
    std::vector<R3BNeulandDigi> digis;
    digis.reserve(nDigis);
    for (UInt_t i = 0; i < nDigis; i++)
    {
        // Note: Dereferencing, the vector does contain full objects, not just pointers
        digis.push_back(*((R3BNeulandDigi*)fDigis->At(i)));
    }

    // Processing
    auto clusters = fClusteringEngine.Clusterize(digis);
    LOG(DEBUG) << "R3BNeulandClusterFinder - nDigis nCluster:" << nDigis << " " << clusters.size() << FairLogger::endl;

    // Output
    for (auto& cluster : clusters)
    {
        new ((*fClusters)[fClusters->GetEntries()]) R3BNeulandCluster(std::move(cluster));
    }
}

void R3BNeulandClusterFinder::Finish() { LOG(INFO) << "R3BNeulandClusterFinder Finished" << FairLogger::endl; }

ClassImp(R3BNeulandClusterFinder);
