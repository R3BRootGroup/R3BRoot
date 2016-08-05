#include "R3BNeulandMCMon.h"

#include <iostream>
#include <string>

#include "TH3D.h"

#include "FairRootManager.h"
#include "FairLogger.h"



R3BNeulandMCMon::R3BNeulandMCMon(const Option_t *option) : FairTask("R3B NeuLAND Neuland Monte Carlo Monitor")
{
    LOG(INFO) << "Using R3B NeuLAND Neuland Monte Carlo Monitor" << FairLogger::endl;

    TString opt = option;
    opt.ToUpper();

    if (opt.Contains("3DTRACK")) {
        fIs3DTrackEnabled = true;
        LOG(INFO) << "... with 3D track visualization" << FairLogger::endl;
    } else {
        fIs3DTrackEnabled = false;
    }
}


R3BNeulandMCMon::~R3BNeulandMCMon()
{}


InitStatus R3BNeulandMCMon::Init()
{
    FairRootManager *rm = FairRootManager::Instance();
    fMCTracks = (TClonesArray *)rm->GetObject("MCTrack");
    if (fMCTracks == nullptr) {
        LOG(FATAL) << "R3BNeulandDigiMon: No MCTrack!" << FairLogger::endl;
        return kFATAL;
    }

    fLandPoints = (TClonesArray *)rm->GetObject("NeulandPoints");
    if (fLandPoints == nullptr) {
        LOG(FATAL) << "R3BNeulandDigiMon: No NeulandPoints!" << FairLogger::endl;
        return kFATAL;
    }

    fhPDG = new TH1D("hPDG", "Number of particles by PDG code created by primary neutron interaction resulting in a LandPoint", 8000, -4000, 4000);
    fhEPrimarys = new TH1D("hE_primarys", "Energy of primary particles", 10000, 0, 10000);
    fhEPrimaryNeutrons = new TH1D("hE_primary_neutrons", "Energy of primary Neutrons", 10000, 0, 10000);
    fhEtotPrim = new TH1D("hE_tot_prim", "Total Light Yield of non-neutron LandPoints created by primary neutron interaction(s)", 6000, 0, 6000);
    fhEtot = new TH1D("hE_tot", "Total Light Yield of non-neutron LandPoints", 6000, 0, 6000);
    fhESecondaryNeutrons = new TH1D("hE_secondary_neutrons", "Energy of neutron tracks created by primary neutron interaction", 6000, 0, 6000);
    fhMotherIDs = new TH1D("hmotherIDs", "MotherIDs", 6001, -1, 6000);
    fhPrimaryDaughterIDs = new TH1D("hprimary_daughter_IDs", "IDs of tracks with a primary mother", 6001, -1, 6000);

    if (fIs3DTrackEnabled) {
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


void R3BNeulandMCMon::Exec(Option_t *)
{

    /* raw MC Track based analysis */
    {
        const UInt_t nTracks = fMCTracks->GetEntries();
        R3BMCTrack *mcTrack;

        for (UInt_t i = 0; i < nTracks; i++) {
            mcTrack = (R3BMCTrack *)fMCTracks->At(i);

            // Distribution of MC Track mother id's
            fhMotherIDs->Fill(mcTrack->GetMotherId());

            // Energy of Inital Particles
            if (mcTrack->GetMotherId() == -1) {
                fhEPrimarys->Fill(GetKineticEnergy(mcTrack));
            }

            // Energy of Inital Neutrons
            if (IsPrimaryNeutron(mcTrack)) {
                fhEPrimaryNeutrons->Fill(GetKineticEnergy(mcTrack));
            }

            // Remaining energy of neutrpns after first interaction
            if (IsMotherPrimaryNeutron(mcTrack) && mcTrack->GetPdgCode() == 2112) {
                fhESecondaryNeutrons->Fill(GetKineticEnergy(mcTrack));
            }
        }
    }


    {
        std::map<Int_t, Double_t> EtotPDG;
        Double_t Etot = 0.;
        Double_t EtotPrim = 0.;

        const UInt_t nLandPoints = fLandPoints->GetEntries();
        R3BLandPoint *landPoint;
        R3BMCTrack *mcTrack;

        for (UInt_t iLP = 0; iLP < nLandPoints; iLP++) {
            landPoint = (R3BLandPoint *)fLandPoints->At(iLP);
            mcTrack = (R3BMCTrack *)fMCTracks->At(landPoint->GetTrackID());

            Etot += landPoint->GetLightYield() * 1000.;

            //Select tracks with a primary neutron mother
            if (IsMotherPrimaryNeutron(mcTrack)) {

                // Total energy of non-neutron secondary particles where mother is a primary neutron
                if (mcTrack->GetPdgCode() != 2112) {
                    EtotPrim += landPoint->GetLightYield() * 1000.;
                }

                // Distribution of secondary particles
                fhPDG->Fill(mcTrack->GetPdgCode());
                fhPrimaryDaughterIDs->Fill(landPoint->GetTrackID());


                // Buld Histograms for each particle PDG if it donst exist
                if (!fhmEPdg[mcTrack->GetPdgCode()]) {
                    TString name = TString("Light Yield of PID ") + TString::Itoa(mcTrack->GetPdgCode(), 10) + TString(" with a primary neutron mother");
                    fhmEPdg[mcTrack->GetPdgCode()] = new TH1D("hE_PDG_" + TString::Itoa(mcTrack->GetPdgCode(), 10), name, 3000, 0, 3000);
                }
                // Get Energy py particle where the mother is a primary neutron
                fhmEPdg[mcTrack->GetPdgCode()]->Fill(landPoint->GetLightYield() * 1000.); //landPoint->GetEnergyLoss()*1000.);
            } // end primary neutron mother

            // Sum energy per particle type per event
            if (!EtotPDG[mcTrack->GetPdgCode()]) {
                EtotPDG[mcTrack->GetPdgCode()] = 0.;
            }
            EtotPDG[mcTrack->GetPdgCode()] += landPoint->GetLightYield() * 1000.; //landPoint->GetEnergyLoss()*1000.;
        }

        fhEtot->Fill(Etot);
        fhEtotPrim->Fill(EtotPrim);


        for (const auto &kv : EtotPDG) {
            if (!fhmEtotPdg[kv.first]) {
                TString name = TString("Sum Light Yield of PID ") + TString::Itoa(kv.first, 10);
                fhmEtotPdg[kv.first] = new TH1D("fhEtotPDG_" + TString::Itoa(kv.first, 10), name, 3000, 0, 3000);
            }
            fhmEtotPdg[kv.first]->Fill(kv.second);

            if (!fhmEtotPdgRel[kv.first]) {
                TString name = TString("Percent Light Yield of PID ") + TString::Itoa(kv.first, 10) + TString(" to total Light Yield.");
                fhmEtotPdgRel[kv.first] = new TH1D("fhEtotPDGRel_" + TString::Itoa(kv.first, 10), name, 110, 0, 110);
            }
            if (Etot == 0) {
                fhmEtotPdgRel[kv.first]->Fill(0);
            } else {
                fhmEtotPdgRel[kv.first]->Fill(kv.second / Etot * 100.);
            }
        }
    }


    if (fIs3DTrackEnabled) {
        // For 3D Vis
        const UInt_t nTracks = fMCTracks->GetEntries();
        R3BMCTrack *mcTrack;
        fh3->Reset("ICES");
        for (UInt_t i = 0; i < nTracks; i++) {
            mcTrack = (R3BMCTrack *)fMCTracks->At(i);
            if (mcTrack->GetMotherId() != -1) {
                if (mcTrack->GetNPoints(kLAND) > 0) {
                    fh3->Fill(mcTrack->GetStartZ(), mcTrack->GetStartX(), mcTrack->GetStartY(), GetKineticEnergy(mcTrack));
                } else {
                    fh3->Fill(mcTrack->GetStartZ(), mcTrack->GetStartX(), mcTrack->GetStartY(), -1.*GetKineticEnergy(mcTrack));
                }
            }
        }
    }

}


void R3BNeulandMCMon::Finish()
{
    fhPDG->Write();
    fhEPrimarys->Write();
    fhEPrimaryNeutrons->Write();
    fhEtot->Write();
    fhEtotPrim->Write();
    fhESecondaryNeutrons->Write();
    fhMotherIDs->Write();
    fhPrimaryDaughterIDs->Write();

    for (const auto &kv : fhmEPdg) {
        kv.second->Write();
    }
    for (const auto &kv : fhmEtotPdg) {
        kv.second->Write();
    }
    for (const auto &kv : fhmEtotPdgRel) {
        kv.second->Write();
    }
}


ClassImp(R3BNeulandMCMon)
