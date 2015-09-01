#define CaloHitAna_cxx
#include "MergedCaloHitAna.h"
#include <TH1.h>
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TMath.h>
#include <TCutG.h>

#include <iostream>
#include <fstream>
using namespace std;

void CaloHitAna::DrawPID(bool cut)
{
   if (fChain == 0) return;

   TH2D *hpid = new TH2D("qpid", "QPID", 250, 0, 4E5, 250, 0, 4E5);
   TCutG *cut_proton = NULL;

   if(cut)
   {
      TFile *f = TFile::Open("qpid_cuts.root", "read");
      cut_proton = dynamic_cast<TCutG*>(f->Get("qpid_proton"));
   }

   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;
   int i;
   for (Long64_t jentry=0; jentry<nentries;jentry++)
   {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;

      for(i = 0; i < CaloHit_; i++)
      {
         if(!cut_proton || cut_proton->IsInside(CaloHit_fNf[i], CaloHit_fNs[i]))
            hpid->Fill(CaloHit_fNf[i], CaloHit_fNs[i]);
      }

   }

   hpid->Draw("colz");
}

// Ca-48, 550 AMeV
#define E_0 (47.952524131*931.494061)
#define E_Beam (E_0 + 48*550)
#define BETA (sqrt(1-(E_0*E_0)/(E_Beam*E_Beam)))
#define GAMMA (1/(sqrt(1-BETA*BETA)))

inline Double_t Doppler(Double_t E_det, Double_t theta)
{
   return E_det * GAMMA * (1 - BETA*TMath::Cos(theta));
}

void CaloHitAna::Loop(bool useProtonCut)
{
   if (fChain == 0) return;

   TFile *fcut = TFile::Open("qpid_cuts.root", "read");
   if(!fcut)
   {
      cerr << "Could not open qpid_cuts.root for reading!\n";
      return;
   }
   TCutG *cut_proton = dynamic_cast<TCutG*>(fcut->Get("qpid_proton"));
   if(!cut_proton)
   {
      cerr << "Could not find TCutG qpid_proton!\n";
      return;
   }
   TCutG *cut_pspx = dynamic_cast<TCutG*>(fcut->Get("pspx_zminus2"));
   if(!cut_pspx)
   {
      cerr << "Could not find TCutG pspx_zminus1\n";
      return;
   }
   fcut->Close();
   delete fcut;

   TH2D *hee = new TH2D("evse", "E1 vs E2", 200, 0, 400000, 200, 0, 400000);
   TH2D *htt = new TH2D("thetavstheta", "Theta 1 vs Theta 2", 16, 27.2, 61.9, 16, 27.2, 61.9);
   TH2D *hpp = new TH2D("phivsphi", "Phi 1 vs Phi 2", 180, 0, 180, 180, 180, 360);

   TH1D *hsumtheta = new TH1D("hSumTheta", "Sum Theta", 180, 0, 180);
   TH1D *hdiffphi = new TH1D("hDiffPhi", "Dela Phi", 180, 0, 180);
   TH1D *hegamma = new TH1D("hEgamma", "Gamma Energy", 1000, 0, 10000);

   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;
   int i;
   int idxEmax1, idxEmax2;
   Double_t sumTheta, eGamma;

   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;

      if(CaloHit_ < 2)
	continue;

      // Only take Z-1 events
      if(!cut_pspx->IsInside(Pspx04e, Pspx05e))
         continue;

      idxEmax1 = -1;
      idxEmax2 = -1;

      for(i = 0; i < CaloHit_; i++)
      {
         // Only consider completely stopped protons
         if(cut_proton->IsInside(CaloHit_fNf[i], CaloHit_fNs[i]) && CaloHit_fPhi[i] < TMath::Pi() && (idxEmax1 == -1 || CaloHit_fEnergy[i] > CaloHit_fEnergy[idxEmax1]))
         {
            // Petal 1
            idxEmax1 = i;
         }
         else if(cut_proton->IsInside(CaloHit_fNf[i], CaloHit_fNs[i]) && CaloHit_fPhi[i] >= TMath::Pi() && (idxEmax2 == -1 || CaloHit_fEnergy[i] > CaloHit_fEnergy[idxEmax2]))
         {
            // Petal 2
            idxEmax2 = i;
         }
//         if(idxEmax1 == -1 || CaloHit_fEnergy[i] > CaloHit_fEnergy[idxEmax1])
//         {
//            idxEmax2 = idxEmax1;
//            idxEmax1 = i;
//         }
//         else if(idxEmax2 == -1 || CaloHit_fEnergy[i] > CaloHit_fEnergy[idxEmax2])
//            idxEmax2 = i;
      }

      if(idxEmax1 != -1 && idxEmax2 != -1)
      {
         sumTheta = (CaloHit_fTheta[idxEmax1] + CaloHit_fTheta[idxEmax2]) * 180.0 / TMath::Pi();
	 htt->Fill(CaloHit_fTheta[idxEmax1] * 180.0 / TMath::Pi(), CaloHit_fTheta[idxEmax2] * 180.0 / TMath::Pi());
         hpp->Fill(CaloHit_fPhi[idxEmax1] * 180.0 / TMath::Pi(), CaloHit_fPhi[idxEmax2] * 180.0 / TMath::Pi());
         hsumtheta->Fill(sumTheta);
         hdiffphi->Fill((CaloHit_fPhi[idxEmax2] - CaloHit_fPhi[idxEmax1]) * 180.0 / TMath::Pi());

         if(sumTheta >= 70.0 && sumTheta <= 90.0)
         {
	    hee->Fill(CaloHit_fEnergy[idxEmax1], CaloHit_fEnergy[idxEmax2]);

            // Try to find coincident gammas
            if(CaloHit_ > 2)
            {
               for(i = 0; i < CaloHit_; i++)
               {
                  if(i != idxEmax1 && i != idxEmax2 && (eGamma = Doppler(CaloHit_fEnergy[i], CaloHit_fTheta[i])) < 10000)
                     // ToDo: Doppler correction
                     hegamma->Fill(eGamma);
               }
            }
         }
      }
   }
}
