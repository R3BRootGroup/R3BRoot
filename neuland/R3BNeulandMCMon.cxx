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
  FairRootManager* frm = FairRootManager::Instance();
  mc_tracks_ = (TClonesArray*)frm->GetObject("MCTrack");

  h_PDG_ = new TH1D("h_PDG", "Number of particles by PDG code created by primary neutron interaction", 8000, -4000, 4000);
  h_E_primary_neutrons_ = new TH1D("h_E_primary_neutrons", "Energy of primary Neutrons", 1000, 0, 1000);
  h_E_tot_ = new TH1D("h_E_tot", "Total energy of non-neutron tracks in kLAND created by primary neutron interaction(s)", 1000, 0, 6000);
  h_E_secondary_neutrons_ = new TH1D("h_E_secondary_neutrons", "Energy of neutron tracks created by primary neutron interaction", 1000, 0, 6000);
  h_motherIDs_ = new TH1D("h_motherIDs", "MotherIDs", 6001, -1, 6000);
  h_primary_daughter_IDs_ = new TH1D("h_primary_daughter_IDs", "IDs of tracks with a primary mother", 6001, -1, 6000);

  return kSUCCESS;
}


void R3BNeulandMCMon::Exec(Option_t* option)
{
  const unsigned int num_tracks = mc_tracks_->GetEntries();
  R3BMCTrack* mc_track;

  std::map<int, double> E_tot_PDG;
  double E_tot = 0.;

  for (unsigned int i = 0; i < num_tracks; i++) {
    mc_track = (R3BMCTrack*)mc_tracks_->At(i);

    // Distribution of MC Track mother id's
    h_motherIDs_->Fill(mc_track->GetMotherId());


    // Energy of Inital Neutrons
    if (isPrimaryNeutron(mc_track)) {
      h_E_primary_neutrons_->Fill(GetKineticEnergy(mc_track));
    }


    //Select tracks with a primary neutron mother
    if (isMotherPrimaryNeutron(mc_track)) {
      // Remaining energy of neutrpns after first interaction
      if (mc_track->GetPdgCode() == 2112) {
        h_E_secondary_neutrons_->Fill(GetKineticEnergy(mc_track));
      }


      if (mc_track->GetNPoints(kLAND) > 0) {
        // Total energy of non-neutron secondary particles where mother is a primary neutron
        if (mc_track->GetPdgCode() != 2112) {
          E_tot += (GetKineticEnergy(mc_track));
        }


        // Distribution of secondary particles
        h_PDG_->Fill( mc_track->GetPdgCode() );
        h_primary_daughter_IDs_->Fill( i );


        // Buld Histograms for each particle PDG if it donst exist
        if (!hm_E_PDG_[mc_track->GetPdgCode()]) {
          TString name = TString("Energy of PID ") + TString::Itoa(mc_track->GetPdgCode(), 10) + TString(" with a primary neutron mother");
          hm_E_PDG_[mc_track->GetPdgCode()] = new TH1D("h_E_PDG_" + TString::Itoa(mc_track->GetPdgCode(), 10), name, 1000, 0, 1000);
        }
        // Get Energy py particle where the mother is a primary neutron
        hm_E_PDG_[mc_track->GetPdgCode()]->Fill(GetKineticEnergy(mc_track));


        // Sum energy per particle type per event
        if (!E_tot_PDG[mc_track->GetPdgCode()]) {
          E_tot_PDG[mc_track->GetPdgCode()] = 0.;
        }
        E_tot_PDG[mc_track->GetPdgCode()] += GetKineticEnergy(mc_track);
      }
    }
  }

  h_E_tot_->Fill(E_tot);


  for (const auto &kv : E_tot_PDG) {
    if (!hm_E_tot_PDG_[kv.first]) {
      TString name = TString("Sum Energy of PID ") + TString::Itoa(kv.first, 10) + TString(" with a primary neutron mother");
      hm_E_tot_PDG_[kv.first] = new TH1D("h_E_tot_PDG_" + TString::Itoa(kv.first, 10), name, 1000, 0, 1000);
    }
    hm_E_tot_PDG_[kv.first]->Fill( kv.second );
  }

}


void R3BNeulandMCMon::Finish()
{
  h_PDG_->Write();
  h_E_primary_neutrons_->Write();
  h_E_tot_->Write();
  h_E_secondary_neutrons_->Write();
  h_motherIDs_->Write();
  h_primary_daughter_IDs_->Write();

  for (const auto &kv : hm_E_PDG_) {
    kv.second->Write();
  }
  for (const auto &kv : hm_E_tot_PDG_) {
    kv.second->Write();
  }
}


ClassImp(R3BNeulandMCMon);
