#include "R3BNeulandMCMon.h"

#include <iostream>
#include <string>

#include "TH3D.h"

#include "FairRootManager.h"



R3BNeulandMCMon::R3BNeulandMCMon() : FairTask("R3B NeuLAND Neuland Monte Carlo Monitor")
{}


R3BNeulandMCMon::~R3BNeulandMCMon()
{}


InitStatus R3BNeulandMCMon::Init()
{
   FairRootManager *rm = FairRootManager::Instance();
   fMCTracks = (TClonesArray *)rm->GetObject("MCTrack");

   fhPDG = new TH1D("h_PDG", "Number of particles by PDG code created by primary neutron interaction", 8000, -4000, 4000);
   fhEPrimaryNeutrons = new TH1D("h_E_primary_neutrons", "Energy of primary Neutrons", 1000, 0, 1000);
   fhEtot = new TH1D("h_E_tot", "Total energy of non-neutron tracks in kLAND created by primary neutron interaction(s)", 1000, 0, 6000);
   fhESecondaryNeutrons = new TH1D("h_E_secondary_neutrons", "Energy of neutron tracks created by primary neutron interaction", 1000, 0, 6000);
   fhMotherIDs = new TH1D("h_motherIDs", "MotherIDs", 6001, -1, 6000);
   fhPrimaryDaughterIDs = new TH1D("h_primary_daughter_IDs", "IDs of tracks with a primary mother", 6001, -1, 6000);

   // XYZ -> ZXY (side view)
   fh3 = new TH3D("hMCTracks", "hMCTracks", 60, 1400, 1700, 50, -125, 125, 50, -125, 125);
   fh3->SetTitle("NeuLAND MCTracks");
   fh3->GetXaxis()->SetTitle("Z");
   fh3->GetYaxis()->SetTitle("X");
   fh3->GetZaxis()->SetTitle("Y");

   rm->Register("NeulandMCMon", "MC Tracks in NeuLAND", fh3, kTRUE);


   return kSUCCESS;
}


void R3BNeulandMCMon::Exec(Option_t *option)
{
   const UInt_t nTracks = fMCTracks->GetEntries();
   R3BMCTrack *mcTrack;

   std::map<Int_t, Double_t> EtotPDG;
   Double_t Etot = 0.;

   for (UInt_t i = 0; i < nTracks; i++) {
      mcTrack = (R3BMCTrack *)fMCTracks->At(i);

      // Distribution of MC Track mother id's
      fhMotherIDs->Fill(mcTrack->GetMotherId());


      // Energy of Inital Neutrons
      if (IsPrimaryNeutron(mcTrack)) {
         fhEPrimaryNeutrons->Fill(GetKineticEnergy(mcTrack));
      }


      //Select tracks with a primary neutron mother
      if (IsMotherPrimaryNeutron(mcTrack)) {
         // Remaining energy of neutrpns after first interaction
         if (mcTrack->GetPdgCode() == 2112) {
            fhESecondaryNeutrons->Fill(GetKineticEnergy(mcTrack));
         }


         if (mcTrack->GetNPoints(kLAND) > 0) {
            // Total energy of non-neutron secondary particles where mother is a primary neutron
            if (mcTrack->GetPdgCode() != 2112) {
               Etot += (GetKineticEnergy(mcTrack));
            }


            // Distribution of secondary particles
            fhPDG->Fill(mcTrack->GetPdgCode());
            fhPrimaryDaughterIDs->Fill(i);


            // Buld Histograms for each particle PDG if it donst exist
            if (!fhmEPdg[mcTrack->GetPdgCode()]) {
               TString name = TString("Energy of PID ") + TString::Itoa(mcTrack->GetPdgCode(), 10) + TString(" with a primary neutron mother");
               fhmEPdg[mcTrack->GetPdgCode()] = new TH1D("h_E_PDG_" + TString::Itoa(mcTrack->GetPdgCode(), 10), name, 1000, 0, 1000);
            }
            // Get Energy py particle where the mother is a primary neutron
            fhmEPdg[mcTrack->GetPdgCode()]->Fill(GetKineticEnergy(mcTrack));


            // Sum energy per particle type per event
            if (!EtotPDG[mcTrack->GetPdgCode()]) {
               EtotPDG[mcTrack->GetPdgCode()] = 0.;
            }
            EtotPDG[mcTrack->GetPdgCode()] += GetKineticEnergy(mcTrack);
         }
      }
   }

   fhEtot->Fill(Etot);


   for (const auto &kv : EtotPDG) {
      if (!fhmEtotPdg[kv.first]) {
         TString name = TString("Sum Energy of PID ") + TString::Itoa(kv.first, 10) + TString(" with a primary neutron mother");
         fhmEtotPdg[kv.first] = new TH1D("fhEtotPDG_" + TString::Itoa(kv.first, 10), name, 1000, 0, 1000);
      }
      fhmEtotPdg[kv.first]->Fill(kv.second);
   }



   // For 3D Vis
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


void R3BNeulandMCMon::Finish()
{
   fhPDG->Write();
   fhEPrimaryNeutrons->Write();
   fhEtot->Write();
   fhESecondaryNeutrons->Write();
   fhMotherIDs->Write();
   fhPrimaryDaughterIDs->Write();

   for (const auto &kv : fhmEPdg) {
      kv.second->Write();
   }
   for (const auto &kv : fhmEtotPdg) {
      kv.second->Write();
   }
}


ClassImp(R3BNeulandMCMon);
