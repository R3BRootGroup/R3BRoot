#include <iostream>
#include <algorithm>

#include "TClonesArray.h"
#include "TMath.h"
#include "TH1F.h"
#include "TH2F.h"

#include "FairRootManager.h"
#include "FairLogger.h"

#include "R3BNeulandDigi.h"
#include "R3BNeulandCluster.h"
#include "R3BNeulandClusterFinder.h"

#include "ClusteringEngine.h"

/* Clustering function for R3BNeulandDigis.
 * Returns true if the Digis belong together in the same cluster and false otherwise. */
static const auto clusteringCondition = [](const R3BNeulandDigi& a, const R3BNeulandDigi& b)
{
    return TMath::Abs(a.GetPosition().X() - b.GetPosition().X()) < 7.5 &&
           TMath::Abs(a.GetPosition().Y() - b.GetPosition().Y()) < 7.5 &&
           TMath::Abs(a.GetPosition().Z() - b.GetPosition().Z()) < 7.5 && TMath::Abs(a.GetT() - b.GetT()) < 1.0;
};

R3BNeulandClusterFinder::R3BNeulandClusterFinder()
    : FairTask("R3BNeulandClusterFinder")
    , fNeulandClusters(new TClonesArray("R3BNeulandCluster"))
// TODO: #ROOT6
//, fClusteringEngine(new Neuland::ClusteringEngine<R3BNeulandDigi>(clusteringCondition))
{
}

R3BNeulandClusterFinder::~R3BNeulandClusterFinder()
{
    if (fNeulandClusters)
    {
        delete fNeulandClusters;
    }
    // TODO: #ROOT6
    //    if (fClusteringEngine)
    //   {
    //      delete fClusteringEngine:
    //    }
}

InitStatus R3BNeulandClusterFinder::Init()
{
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman)
    {
        LOG(FATAL) << "R3BNeulandClusterFinder::Init: No FairRootManager" << FairLogger::endl;
        return kFATAL;
    }

    // Set Input: TClonesArray of R3BNeulandDigis
    if ((TClonesArray*)ioman->GetObject("NeulandDigis") == nullptr)
    {
        LOG(FATAL) << "R3BNeulandClusterFinder::Init No NeulandDigis!" << FairLogger::endl;
        return kFATAL;
    }
    if (!TString(((TClonesArray*)ioman->GetObject("NeulandDigis"))->GetClass()->GetName()).EqualTo("R3BNeulandDigi"))
    {
        LOG(FATAL) << "R3BNeulandClusterFinder::Init Branch NeulandDigis does not contain R3BNeulandDigis!"
                   << FairLogger::endl;
        return kFATAL;
    }
    fNeulandDigis = (TClonesArray*)ioman->GetObject("NeulandDigis");

    // Set Output: TClonesArray of R3BNeulandDigis
    ioman->Register("NeulandClusters", "Clusters in NeuLAND", fNeulandClusters, kTRUE);

    return kSUCCESS;
}

void R3BNeulandClusterFinder::Exec(Option_t*)
{
    Reset();

    // Convert the TClonesArray to a std::vector
    // TODO: #ROOT6 Convert this into a (FairRoot) template function
    const Int_t nDigis = fNeulandDigis->GetEntries();
    // Note: Reseving space can improve performance
    fVectorDigis.reserve(nDigis);
    R3BNeulandDigi* digi;
    for (Int_t i = 0; i < nDigis; i++)
    {
        digi = (R3BNeulandDigi*)fNeulandDigis->At(i);
        // Note: Derefencing, the vector does contain full objects, not just pointers
        fVectorDigis.push_back(*digi);
    }

    // TODO: #ROOT6 Promote this to a member function so the object does not have to be recreated for every event
    auto fClusteringEngine = Neuland::ClusteringEngine<R3BNeulandDigi>(clusteringCondition);
    auto clusters = fClusteringEngine.Clusterize(fVectorDigis);

    for (auto& cluster : clusters)
    {
        new ((*fNeulandClusters)[fNeulandClusters->GetEntries()]) R3BNeulandCluster(std::move(cluster));
    }
}

void R3BNeulandClusterFinder::Finish() {}

void R3BNeulandClusterFinder::Reset()
{
    fVectorDigis.clear();
    fNeulandClusters->Clear();
}

ClassImp(R3BNeulandClusterFinder);
