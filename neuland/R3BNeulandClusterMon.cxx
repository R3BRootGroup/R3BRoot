#include "R3BNeulandClusterMon.h"

#include <iostream>
#include <algorithm>
#include <numeric>

#include "TClonesArray.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"

#include "FairRootManager.h"
#include "FairLogger.h"

#include "R3BNeulandCluster.h"

R3BNeulandClusterMon::R3BNeulandClusterMon(const Option_t* option)
    : FairTask("R3B NeuLAND NeulandCluster Monitor")
{
    LOG(INFO) << "Using R3B NeuLAND NeulandCluster Monitor" << FairLogger::endl;

    TString opt = option;
    opt.ToUpper();

    if (opt.Contains("3DTRACK"))
    {
        fIs3DTrackEnabled = true;
        LOG(INFO) << "... with 3D track visualization" << FairLogger::endl;
    }
    else
    {
        fIs3DTrackEnabled = false;
    }
}

R3BNeulandClusterMon::~R3BNeulandClusterMon() {}

InitStatus R3BNeulandClusterMon::Init()
{
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman)
    {
        LOG(FATAL) << "R3BNeulandClusterMon::Init: No FairRootManager" << FairLogger::endl;
        return kFATAL;
    }

    // Set Input: TClonesArray of R3BNeulandClusters
    if ((TClonesArray*)ioman->GetObject("NeulandClusters") == nullptr)
    {
        LOG(FATAL) << "R3BNeulandClusterMon::Init No NeulandClusters!" << FairLogger::endl;
        return kFATAL;
    }
    if (!TString(((TClonesArray*)ioman->GetObject("NeulandClusters"))->GetClass()->GetName())
             .EqualTo("R3BNeulandCluster"))
    {
        LOG(FATAL) << "R3BNeulandClusterMon::Init Branch NeulandClusters does not contain R3BNeulandClusters!"
                   << FairLogger::endl;
        return kFATAL;
    }
    fNeulandClusters = (TClonesArray*)ioman->GetObject("NeulandClusters");

    if (fIs3DTrackEnabled)
    {
        // XYZ -> ZXY (side view)
        fh3 = new TH3D("hClusters", "hClusters", 60, 1400, 1700, 50, -125, 125, 50, -125, 125);
        fh3->SetTitle("NeuLAND Clusters");
        fh3->GetXaxis()->SetTitle("Z");
        fh3->GetYaxis()->SetTitle("X");
        fh3->GetZaxis()->SetTitle("Y");

        ioman->Register("NeulandClusterMon", "Cluster TH3Ds in NeuLAND", fh3, kTRUE);
    }

    fhClusters = new TH1D("Neuland/Clusters", "Number of clusters in one event", 100, 0, 100);
    fhClusterSize = new TH1D("Neuland/ClusterSize", "Number of Digis for each Cluster", 100, 0, 100);
    fhClusterEnergy = new TH1D("Neuland/ClusterEnergy", "Energy for each Cluster", 5000, 0., 500.);
    fhClusterNumberVSEnergy =
        new TH2D("Neuland/ClusterNumberEtot", "Number of Clusters vs. Total Energy", 100, 0, 1000, 50, 0, 50);
    fhClusterNumberVSEnergy->GetXaxis()->SetTitle("Total energy [MeV]");
    fhClusterNumberVSEnergy->GetYaxis()->SetTitle("Number of Clusters");

    return kSUCCESS;
}

void R3BNeulandClusterMon::Exec(Option_t*)
{
    const UInt_t nClusters = fNeulandClusters->GetEntries();

    if (fIs3DTrackEnabled)
    {
        fh3->Reset("ICES");
        for (UInt_t i = 0; i < nClusters; i++)
        {
            const R3BNeulandCluster* cluster = (R3BNeulandCluster*)fNeulandClusters->At(i);
            const auto start = cluster->GetFirstDigi().GetPosition();
            // XYZ -> ZXY (side view)
            fh3->Fill(start.Z(), start.X(), start.Y(), cluster->GetE());
        }
    }

    fhClusters->Fill(nClusters);

    Double_t etot = 0.;
    for (UInt_t i = 0; i < nClusters; i++)
    {
        const R3BNeulandCluster* cluster = (R3BNeulandCluster*)fNeulandClusters->At(i);
        fhClusterSize->Fill(cluster->GetSize());
        fhClusterEnergy->Fill(cluster->GetE());
        etot += cluster->GetE();
    }
    fhClusterNumberVSEnergy->Fill(etot, nClusters);
}

void R3BNeulandClusterMon::Finish()
{
    fhClusters->Write();
    fhClusterSize->Write();
    fhClusterEnergy->Write();
    fhClusterNumberVSEnergy->Write();
}

ClassImp(R3BNeulandClusterMon)
