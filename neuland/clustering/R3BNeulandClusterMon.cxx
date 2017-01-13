#include "R3BNeulandClusterMon.h"

#include <iostream>
#include <algorithm>
#include <numeric>

#include "TClonesArray.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TDirectory.h"

#include "FairRootManager.h"
#include "FairLogger.h"

#include "R3BNeulandCluster.h"

R3BNeulandClusterMon::R3BNeulandClusterMon(const TString input, const TString output, const Option_t* option)
    : FairTask("R3B NeuLAND NeulandCluster Monitor")
    , fInput(input)
    , fOutput(output)
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
    if ((TClonesArray*)ioman->GetObject(fInput) == nullptr)
    {
        LOG(FATAL) << "R3BNeulandClusterMon::Init No NeulandClusters!" << FairLogger::endl;
        return kFATAL;
    }
    if (!TString(((TClonesArray*)ioman->GetObject(fInput))->GetClass()->GetName()).EqualTo("R3BNeulandCluster"))
    {
        LOG(FATAL) << "R3BNeulandClusterMon::Init Branch " << fInput << " does not contain R3BNeulandClusters!"
                   << FairLogger::endl;
        return kFATAL;
    }
    fNeulandClusters = (TClonesArray*)ioman->GetObject(fInput);

    if (fIs3DTrackEnabled)
    {
        // XYZ -> ZXY (side view)
        fh3 = new TH3D("hClusters", "hClusters", 60, 1400, 1700, 50, -125, 125, 50, -125, 125);
        fh3->SetTitle("NeuLAND Clusters");
        fh3->GetXaxis()->SetTitle("Z");
        fh3->GetYaxis()->SetTitle("X");
        fh3->GetZaxis()->SetTitle("Y");

        ioman->Register(fOutput, "Cluster TH3Ds in NeuLAND", fh3, kTRUE);
    }

    fhClusters = new TH1D("Neuland/Clusters", "Number of clusters in one event", 100, 0, 100);
    fhClusterSize = new TH1D("Neuland/ClusterSize", "Number of Digis for each Cluster", 100, 0, 100);
    fhClusterEnergy = new TH1D("Neuland/ClusterEnergy", "Energy for each Cluster", 5000, 0., 500.);
    fhClusterTime = new TH1D("ClusterTime", "Time for each Cluster", 5000, 0., 500.);
    fhClusterEToF = new TH1D("ClusterEToF", "Cluster EToF", 1000, 0, 1000);
    fhClusterNumberVSEnergy =
        new TH2D("ClusterNumberEtot", "Number of Clusters vs. Total Energy", 200, 0, 2000, 50, 0, 50);
    fhClusterNumberVSEnergy->GetXaxis()->SetTitle("Total energy [MeV]");
    fhClusterNumberVSEnergy->GetYaxis()->SetTitle("Number of Clusters");

    fhClusterEToFVSEnergy =
        new TH2D("ClusterEToFVSEnergy", "Cluster E_{ToF} vs. Cluster Energy", 100, 0, 1000, 100, 0, 1000);
    fhClusterEToFVSEnergy->GetXaxis()->SetTitle("Cluster E_{ToF} [MeV]");
    fhClusterEToFVSEnergy->GetYaxis()->SetTitle("Cluster E [MeV]");

    fhClusterEToFVSTime = new TH2D("ClusterEToFVSTime", "Cluster E_{ToF} vs. Cluster Time", 100, 0, 1000, 500, 0, 500);
    fhClusterEToFVSTime->GetXaxis()->SetTitle("Cluster E_{ToF} [MeV]");
    fhClusterEToFVSTime->GetYaxis()->SetTitle("Cluster t [ns]");

    fhClusterEVSTime = new TH2D("ClusterEVSTime", "Cluster E vs. Cluster Time", 250, 0, 250, 500, 0, 250);
    fhClusterEVSTime->GetXaxis()->SetTitle("Cluster E [MeV]");
    fhClusterEVSTime->GetYaxis()->SetTitle("Cluster t [ns]");

    fhClusterForemostMinusCentroidVSEnergy =
        new TH2D("ClusterForemostMinusCentroidVSEnergy",
                 "Distance between Foremost Digi and Energy Centroid vs Cluster Energy",
                 101,
                 0,
                 100,
                 60,
                 0,
                 600);
    fhClusterForemostMinusCentroidVSEnergy->GetXaxis()->SetTitle("|#vec{r_{z_{min}}}-#vec{r_{EC}}| [cm]");
    fhClusterForemostMinusCentroidVSEnergy->GetYaxis()->SetTitle("Cluster Energy [MeV]");

    fhClusterForemostMinusMaxEnergyDigiPosVSEnergy =
        new TH2D("ClusterForemostMinusMaxEnergyDigiPosVSEnergy",
                 "Distance between Foremost Digi and Max Energy Digi vs Cluster Energy",
                 101,
                 0,
                 100,
                 60,
                 0,
                 600);
    fhClusterForemostMinusMaxEnergyDigiPosVSEnergy->GetXaxis()->SetTitle("|#vec{r_{z_{min}}}-#vec{r_{E_{max}}}| [cm]");
    fhClusterForemostMinusMaxEnergyDigiPosVSEnergy->GetYaxis()->SetTitle("Cluster Energy [MeV]");

    fhClusterCentroidMinusFirstDigiPosVSEnergy =
        new TH2D("ClusterCentroidMinusFirstDigiPosVSEnergy",
                 "Distance between Cluster Energy Centroid and First Hit vs Cluster Energy",
                 101,
                 0,
                 100,
                 60,
                 0,
                 600);
    fhClusterCentroidMinusFirstDigiPosVSEnergy->GetXaxis()->SetTitle("|#vec{r_{EC}}-#vec{r_{t_{min}}}| [cm]");
    fhClusterCentroidMinusFirstDigiPosVSEnergy->GetYaxis()->SetTitle("Cluster Energy [MeV]");

    fhClusterMaxEnergyDigiMinusFirstDigiPosVSEnergy =
        new TH2D("ClusterMaxEnergyDigiMinusFirstDigiPosVSEnergy",
                 "Distance between Max Energy Digi and First Digi vs Cluster Energy",
                 101,
                 0,
                 100,
                 60,
                 0,
                 600);
    fhClusterMaxEnergyDigiMinusFirstDigiPosVSEnergy->GetXaxis()->SetTitle("|#vec{r_{E_{max}}}-#vec{r_{t_{min}}}| [cm]");
    fhClusterMaxEnergyDigiMinusFirstDigiPosVSEnergy->GetYaxis()->SetTitle("Cluster Energy [MeV]");

    fhClusterMaxEnergyDigiMinusCentroidVSEnergy =
        new TH2D("ClusterMaxEnergyDigiMinusCentroidVSEnergy",
                 "Distance between Max Energy Digi and Energy Centroid vs Cluster Energy",
                 101,
                 0,
                 100,
                 60,
                 0,
                 600);
    fhClusterMaxEnergyDigiMinusCentroidVSEnergy->GetXaxis()->SetTitle("|#vec{r_{E_{max}}}-#vec{r_{EC}}| [cm]");
    fhClusterMaxEnergyDigiMinusCentroidVSEnergy->GetYaxis()->SetTitle("Cluster Energy [MeV]");

    fhClusterEnergyMomentVSEnergy =
        new TH2D("ClusterEnergyMomentVSEnergy", "Cluster Energy Moment VS Energy", 101, 0, 100, 60, 0, 600);
    fhClusterEnergyMomentVSEnergy->GetXaxis()->SetTitle("|#vec{EM}| [cm]");
    fhClusterEnergyMomentVSEnergy->GetYaxis()->SetTitle("Cluster Energy [MeV]");

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

    fVectorClusters.reserve(nClusters);
    for (UInt_t i = 0; i < nClusters; i++)
    {
        const auto cluster = (R3BNeulandCluster*)fNeulandClusters->At(i);
        // Note: Dereferencing, the vector does contain full objects, not just pointers
        fVectorClusters.push_back(*cluster);
    }

    fhClusters->Fill(nClusters);
    Double_t etot = 0.;
    for (UInt_t i = 0; i < nClusters; i++)
    {
        const R3BNeulandCluster* cluster = (R3BNeulandCluster*)fNeulandClusters->At(i);
        fhClusterTime->Fill(cluster->GetT());
        fhClusterSize->Fill(cluster->GetSize());
        fhClusterEnergy->Fill(cluster->GetE());
        fhClusterEToF->Fill(cluster->GetFirstDigi().GetEToF());
        fhClusterEToFVSEnergy->Fill(cluster->GetFirstDigi().GetEToF(), cluster->GetE());
        fhClusterEToFVSTime->Fill(cluster->GetFirstDigi().GetEToF(), cluster->GetT());
        fhClusterEVSTime->Fill(cluster->GetFirstDigi().GetE(), cluster->GetT());
        etot += cluster->GetE();

        // if(cluster->GetFirstDigi().GetEToF() > 590){
        fhClusterForemostMinusCentroidVSEnergy->Fill(
            (cluster->GetForemostDigi().GetPosition() - cluster->GetEnergyCentroid()).Mag(), cluster->GetE());

        fhClusterForemostMinusMaxEnergyDigiPosVSEnergy->Fill(
            (cluster->GetForemostDigi().GetPosition() - cluster->GetMaxEnergyDigi().GetPosition()).Mag(),
            cluster->GetE());

        fhClusterCentroidMinusFirstDigiPosVSEnergy->Fill(
            (cluster->GetEnergyCentroid() - cluster->GetFirstDigi().GetPosition()).Mag(), cluster->GetE());

        fhClusterMaxEnergyDigiMinusFirstDigiPosVSEnergy->Fill(
            (cluster->GetMaxEnergyDigi().GetPosition() - cluster->GetFirstDigi().GetPosition()).Mag(), cluster->GetE());
        fhClusterMaxEnergyDigiMinusCentroidVSEnergy->Fill(
            (cluster->GetMaxEnergyDigi().GetPosition() - cluster->GetEnergyCentroid()).Mag(), cluster->GetE());
        fhClusterEnergyMomentVSEnergy->Fill(cluster->GetEnergyMoment(), cluster->GetE());
        //}
    }
    fhClusterNumberVSEnergy->Fill(etot, nClusters);
}

void R3BNeulandClusterMon::Reset() { fVectorClusters.clear(); }

void R3BNeulandClusterMon::Finish()
{
    TDirectory* tmp = gDirectory;
    FairRootManager::Instance()->GetOutFile()->cd();

    gDirectory->mkdir(fOutput);
    gDirectory->cd(fOutput);

    fhClusters->Write();
    fhClusterSize->Write();
    fhClusterEnergy->Write();
    fhClusterTime->Write();
    fhClusterNumberVSEnergy->Write();
    fhClusterEToF->Write();
    fhClusterEToFVSEnergy->Write();
    fhClusterEToFVSTime->Write();

    fhClusterForemostMinusCentroidVSEnergy->Write();
    fhClusterForemostMinusMaxEnergyDigiPosVSEnergy->Write();
    fhClusterCentroidMinusFirstDigiPosVSEnergy->Write();
    fhClusterMaxEnergyDigiMinusFirstDigiPosVSEnergy->Write();
    fhClusterMaxEnergyDigiMinusCentroidVSEnergy->Write();
    fhClusterEnergyMomentVSEnergy->Write();
    fhClusterEVSTime->Write();

    gDirectory = tmp;
}

ClassImp(R3BNeulandClusterMon)
