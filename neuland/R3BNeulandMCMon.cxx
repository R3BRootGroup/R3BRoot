#include "R3BNeulandMCMon.h"

#include <iostream>
#include <string>

#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TDirectory.h"

#include "FairRootManager.h"
#include "FairLogger.h"

R3BNeulandMCMon::R3BNeulandMCMon(const Option_t* option)
    : FairTask("R3B NeuLAND Neuland Monte Carlo Monitor")
{
    LOG(INFO) << "Using R3B NeuLAND Neuland Monte Carlo Monitor" << FairLogger::endl;

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

R3BNeulandMCMon::~R3BNeulandMCMon() {}

InitStatus R3BNeulandMCMon::Init()
{
    FairRootManager* rm = FairRootManager::Instance();
    fMCTracks = (TClonesArray*)rm->GetObject("MCTrack");
    if (fMCTracks == nullptr)
    {
        LOG(FATAL) << "R3BNeulandMCMon: No MCTrack!" << FairLogger::endl;
        return kFATAL;
    }

    fNeulandPoints = (TClonesArray*)rm->GetObject("NeulandPoints");
    if (fNeulandPoints == nullptr)
    {
        LOG(FATAL) << "R3BNeulandMCMon: No NeulandPoints!" << FairLogger::endl;
        return kFATAL;
    }

    fNPNIPs = (TClonesArray*)rm->GetObject("NeulandPrimaryNeutronInteractionPoints");
    if (fNeulandPoints == nullptr)
    {
        LOG(FATAL) << "R3BNeulandMCMon: No NeulandPrimaryNeutronInteractionPoints!" << FairLogger::endl;
        return kFATAL;
    }

    fhNPNIPsEToFVSTime = new TH2D("NPNIPEToFVSTime", "NPNIP E_{ToF} vs. NPNIP Time", 100, 0, 1000, 500, 0, 500);
    fhNPNIPsEToFVSTime->GetXaxis()->SetTitle("NPNIP E_{ToF} [MeV]");
    fhNPNIPsEToFVSTime->GetYaxis()->SetTitle("NPNIP t [ns]");

    fhPDG = new TH1D("hPDG",
                     "Number of particles by PDG code created by primary neutron interaction resulting in a point",
                     8000,
                     -4000,
                     4000);
    fhEPrimarys = new TH1D("hE_primarys", "Energy of primary particles", 10000, 0, 10000);
    fhEPrimaryNeutrons = new TH1D("hE_primary_neutrons", "Energy of primary Neutrons", 10000, 0, 10000);
    fhEtotPrim = new TH1D("hE_tot_prim",
                          "Total Light Yield of non-neutron LandPoints created by primary neutron interaction(s)",
                          6000,
                          0,
                          6000);
    fhEtot = new TH1D("hE_tot", "Total Light Yield of non-neutron LandPoints", 6000, 0, 6000);
    fhESecondaryNeutrons = new TH1D(
        "hE_secondary_neutrons", "Energy of neutron tracks created by primary neutron interaction", 6000, 0, 6000);
    fhMotherIDs = new TH1D("hmotherIDs", "MotherIDs", 6001, -1, 6000);
    fhPrimaryDaughterIDs = new TH1D("hprimary_daughter_IDs", "IDs of tracks with a primary mother", 6001, -1, 6000);
    fhMCToF = new TH1D("fhMCToF", "Energy of primary Neutron - ToF Energy from PNIPS", 2001, -1000, 1000);

    fhNPNIPSrvsz = new TH2D("fhNPNIPSrvsz", "NPNIPS R = #sqrt(x**2+y**2) vs z", 600, 1000, 4000, 40, 0, 200);
    fhNPNIPSrvsz->GetXaxis()->SetTitle("NPNIP Z [cm]");
    fhNPNIPSrvsz->GetYaxis()->SetTitle("NPNIP R [cm]");

    fhNPNIPSxy = new TH2D("fhNPNIPSxy", "NPNIPS xy", 101, -200, 200, 101, -200, 200);
    fhNPNIPSxy->GetXaxis()->SetTitle("NPNIP X [cm]");
    fhNPNIPSxy->GetYaxis()->SetTitle("NPNIP Y [cm]");

    fhnNPNIPs = new TH1D("fhnNPNIPs", "Number of NPNIPs per event", 10, -0.5, 9.5);

    if (fIs3DTrackEnabled)
    {
        // XYZ -> ZXY (side view)
        fh3 = new TH3D("hMCTracks", "hMCTracks", 60, 1400, 1700, 50, -125, 125, 50, -125, 125);
        fh3->SetTitle("NeuLAND MCTracks");
        fh3->GetXaxis()->SetTitle("Z");
        fh3->GetYaxis()->SetTitle("X");
        fh3->GetZaxis()->SetTitle("Y");

        rm->Register("NeulandMCMon", "MC Tracks in NeuLAND", fh3, kTRUE);
    }

    return kSUCCESS;
}

void R3BNeulandMCMon::Exec(Option_t*)
{

    {
        /* raw MC Track based analysis */
        const UInt_t nTracks = fMCTracks->GetEntries();
        R3BMCTrack* mcTrack;

        for (UInt_t i = 0; i < nTracks; i++)
        {
            mcTrack = (R3BMCTrack*)fMCTracks->At(i);

            // Distribution of MC Track mother id's
            fhMotherIDs->Fill(mcTrack->GetMotherId());

            // Energy of Inital Particles
            if (mcTrack->GetMotherId() == -1)
            {
                fhEPrimarys->Fill(GetKineticEnergy(mcTrack));
            }

            // Energy of Inital Neutrons
            if (IsPrimaryNeutron(mcTrack))
            {
                fhEPrimaryNeutrons->Fill(GetKineticEnergy(mcTrack));
            }

            // Remaining energy of neutrons after first interaction
            if (IsMotherPrimaryNeutron(mcTrack) && mcTrack->GetPdgCode() == 2112)
            {
                fhESecondaryNeutrons->Fill(GetKineticEnergy(mcTrack));
            }
        }

        const UInt_t nNPNIPs = fNPNIPs->GetEntries();
        fhnNPNIPs->Fill(nNPNIPs);
        FairMCPoint* npnip;
        for (UInt_t i = 0; i < nNPNIPs; i++)
        {
            npnip = (FairMCPoint*)fNPNIPs->At(i);

            // WIP: ToF Calculation -> Should respect other origin than 0,0,0,0.
            const Double_t s2 =
                std::pow(npnip->GetX(), 2) + std::pow(npnip->GetY(), 2) + std::pow(npnip->GetZ(), 2); // cm²
            const Double_t v2 = s2 / std::pow(npnip->GetTime(), 2);                                   // ns²

            const Double_t c2 = 898.75517873681758374898; // cm²/ns²
            const Double_t massNeutron = 939.565379;      // MeV/c²
            const Double_t ETimeOfFlight = massNeutron * ((1. / std::sqrt(1 - (v2 / c2))) - 1);

            mcTrack = (R3BMCTrack*)fMCTracks->At(npnip->GetTrackID());
            fhNPNIPsEToFVSTime->Fill(ETimeOfFlight, npnip->GetTime());
            fhMCToF->Fill(GetKineticEnergy(mcTrack) - ETimeOfFlight);
            fhNPNIPSrvsz->Fill(npnip->GetZ(), std::sqrt(std::pow(npnip->GetX(), 2) + std::pow(npnip->GetY(), 2)));
            fhNPNIPSxy->Fill(npnip->GetX(), npnip->GetY());
        }
    }

    {
        std::map<Int_t, Double_t> EtotPDG;
        Double_t Etot = 0.;
        Double_t EtotPrim = 0.;

        const UInt_t nLandPoints = fNeulandPoints->GetEntries();
        R3BNeulandPoint* point;
        R3BMCTrack* mcTrack;

        for (UInt_t iLP = 0; iLP < nLandPoints; iLP++)
        {
            point = (R3BNeulandPoint*)fNeulandPoints->At(iLP);
            mcTrack = (R3BMCTrack*)fMCTracks->At(point->GetTrackID());

            Etot += point->GetLightYield() * 1000.;

            // Select tracks with a primary neutron mother
            if (IsMotherPrimaryNeutron(mcTrack))
            {

                // Total energy of non-neutron secondary particles where mother is a primary neutron
                if (mcTrack->GetPdgCode() != 2112)
                {
                    EtotPrim += point->GetLightYield() * 1000.;
                }

                // Distribution of secondary particles
                fhPDG->Fill(mcTrack->GetPdgCode());
                fhPrimaryDaughterIDs->Fill(point->GetTrackID());

                // Buld Histograms for each particle PDG if it donst exist
                if (!fhmEPdg[mcTrack->GetPdgCode()])
                {
                    TString name = TString("Light Yield of PID ") + TString::Itoa(mcTrack->GetPdgCode(), 10) +
                                   TString(" with a primary neutron mother");
                    fhmEPdg[mcTrack->GetPdgCode()] =
                        new TH1D("hE_PDG_" + TString::Itoa(mcTrack->GetPdgCode(), 10), name, 3000, 0, 3000);
                }
                // Get Energy py particle where the mother is a primary neutron
                fhmEPdg[mcTrack->GetPdgCode()]->Fill(point->GetLightYield() * 1000.); // point->GetEnergyLoss()*1000.);
            }                                                                         // end primary neutron mother

            // Sum energy per particle type per event
            if (!EtotPDG[mcTrack->GetPdgCode()])
            {
                EtotPDG[mcTrack->GetPdgCode()] = 0.;
            }
            EtotPDG[mcTrack->GetPdgCode()] += point->GetLightYield() * 1000.; // point->GetEnergyLoss()*1000.;
        }

        fhEtot->Fill(Etot);
        fhEtotPrim->Fill(EtotPrim);

        for (const auto& kv : EtotPDG)
        {
            if (!fhmEtotPdg[kv.first])
            {
                TString name = TString("Sum Light Yield of PID ") + TString::Itoa(kv.first, 10);
                fhmEtotPdg[kv.first] = new TH1D("fhEtotPDG_" + TString::Itoa(kv.first, 10), name, 3000, 0, 3000);
            }
            fhmEtotPdg[kv.first]->Fill(kv.second);

            if (!fhmEtotPdgRel[kv.first])
            {
                TString name = TString("Percent Light Yield of PID ") + TString::Itoa(kv.first, 10) +
                               TString(" to total Light Yield.");
                fhmEtotPdgRel[kv.first] = new TH1D("fhEtotPDGRel_" + TString::Itoa(kv.first, 10), name, 110, 0, 110);
            }
            if (Etot == 0)
            {
                fhmEtotPdgRel[kv.first]->Fill(0);
            }
            else
            {
                fhmEtotPdgRel[kv.first]->Fill(kv.second / Etot * 100.);
            }
        }
    }

    if (fIs3DTrackEnabled)
    {
        // For 3D Vis
        const UInt_t nPoints = fNeulandPoints->GetEntries();
        R3BNeulandPoint* point;
        fh3->Reset("ICES");
        for (UInt_t i = 0; i < nPoints; i++)
        {
            point = (R3BNeulandPoint*)fNeulandPoints->At(i);
            if (point->GetLightYield() > 0)
            {
                fh3->Fill(point->GetPosition().Z(),
                          point->GetPosition().X(),
                          point->GetPosition().Y(),
                          point->GetEnergyLoss() * 1000.);
            }
        }
    }
}

void R3BNeulandMCMon::Finish()
{
    TDirectory* tmp = gDirectory;
    FairRootManager::Instance()->GetOutFile()->cd();

    gDirectory->mkdir("NeulandMCMon");
    gDirectory->cd("NeulandMCMon");

    fhPDG->Write();
    fhEPrimarys->Write();
    fhEPrimaryNeutrons->Write();
    fhEtot->Write();
    fhEtotPrim->Write();
    fhESecondaryNeutrons->Write();
    fhMotherIDs->Write();
    fhPrimaryDaughterIDs->Write();
    fhMCToF->Write();
    fhNPNIPsEToFVSTime->Write();
    fhNPNIPSrvsz->Write();
    fhNPNIPSxy->Write();
    fhnNPNIPs->Write();

    for (const auto& kv : fhmEPdg)
    {
        kv.second->Write();
    }
    for (const auto& kv : fhmEtotPdg)
    {
        kv.second->Write();
    }
    for (const auto& kv : fhmEtotPdgRel)
    {
        kv.second->Write();
    }

    gDirectory = tmp;
}

ClassImp(R3BNeulandMCMon)
