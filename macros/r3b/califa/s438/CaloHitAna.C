#define CaloHitAna_cxx
#include "CaloHitAna.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TMath.h>
#include <TCutG.h>
#include <TImage.h>

#include <iostream>

using namespace std;

void CaloHitAna::DrawPID()
{
   if (fChain == 0) return;

   TH2D *hpid = new TH2D("qpid", "QPID", 250, 0, .5, 250, 0, .5);

   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;
   int i;
   int idxEmax1, idxEmax2;
   for (Long64_t jentry=0; jentry<nentries;jentry++)
   {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;

      for(i = 0; i < CaloHit_; i++)
         hpid->Fill(CaloHit_fNf[i], CaloHit_fNs[i]);

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

void CaloHitAna::Loop()
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
   fcut->Close();
   delete fcut;

   TH2D *hee = new TH2D("evse", "E1 vs E2", 200, 0, 400000, 200, 0, 400000);
   TH2D *htt = new TH2D("thetavstheta", "Theta 1 vs Theta 2", 16, 27.2, 61.9, 16, 27.2, 61.9);

   TH1D *hsumtheta = new TH1D("hSumTheta", "Sum Theta", 45, 0, 180);
   TH1D *hegamma = new TH1D("hEgamma", "Gamma Energy", 1000, 0, 10000);

   TH1D *hevstheta = new TH1D("hEvsTheta", "E vs Theta", 16, 27.2, 61.9, 200, 0, 400E3);

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
         hsumtheta->Fill(sumTheta);

         if(sumTheta >= 70.0 && sumTheta <= 90.0)
         {
	    hee->Fill(CaloHit_fEnergy[idxEmax1], CaloHit_fEnergy[idxEmax2]);
            hevsthata->Fill(CaloHit_fTheta[idxEmax1], CaloHit_fEnergy[idxEmax1]);
            hevsthata->Fill(CaloHit_fTheta[idxEmax2], CaloHit_fEnergy[idxEmax2]);

            // Try to find coincident gammas
            if(CaloHit_ > 2)
            {
               for(i = 0; i < CaloHit_; i++)
               {
                  if(i != idxEmax1 && i != idxEmax2 && (eGamma = Doppler(CaloHit_fEnergy[i], CaloHit_fTheta[i])) < 10000)
                     hegamma->Fill(eGamma);
               }
            }
         }
      }
   }

   TCanvas *c = new TCanvas("ctmp", "tmp");
   c->cd();
   hevstheta->Draw();
   c->SetLogz(1);

   TImage *img = TImage::Create();
   img->FromPad(c);
   img->WriteImage("evstheta.png");
}
//void CaloHitAna::Loop()
//{
//   if (fChain == 0) return;
//
//   TFile *fcut = TFile::Open("qpid_cuts.root", "read");
//   if(!fcut)
//   {
//      cerr << "Could not open qpid_cuts.root for reading!\n";
//      return;
//   }
//   TCutG *cut_proton = dynamic_cast<TCutG*>(fcut->Get("qpid_proton"));
//   if(!cut_proton)
//   {
//      cerr << "Could not find object qpid_proton!\n";
//      return;
//   }
//   fcut->Close();
//   delete fcut;
//
//   TH2D *hee = new TH2D("evse", "E1 vs E2", 200, 0, 400000, 200, 0, 400000);
//   TH2D *htt = new TH2D("thetavstheta", "Theta 1 vs Theta 2", 16, 27.2, 61.9, 16, 27.2, 61.9);
//
//   Long64_t nentries = fChain->GetEntriesFast();
//
//   Long64_t nbytes = 0, nb = 0;
//   int i, nproton;
//   int idxEmax1, idxEmax2;
//   for (Long64_t jentry=0; jentry<nentries;jentry++) {
//      Long64_t ientry = LoadTree(jentry);
//      if (ientry < 0) break;
//      nb = fChain->GetEntry(jentry);   nbytes += nb;
//
//      if(CaloHit_ < 2)
//	continue;
//
//      nproton = 0;
//      idxEmax1 = -1;
//      idxEmax2 = -1;
//
//      for(i = 0; i < CaloHit_; i++)
//      {
//         // Only consider completely stopped protons
//         if(!cut_proton->IsInside(CaloHit_fNf[i], CaloHit_fNs[i]))
//            continue;
//
//         nproton++;
//         if(idxEmax1 == -1 || CaloHit_fEnergy[i] > CaloHit_fEnergy[idxEmax1])
//         {
//            idxEmax2 = idxEmax1;
//            idxEmax1 = i;
//         }
//         else if(idxEmax2 == -1 || CaloHit_fEnergy[i] > CaloHit_fEnergy[idxEmax2])
//            idxEmax2 = i;
//      }
//
//      if(nproton >= 2)
//      {
//	hee->Fill(CaloHit_fEnergy[idxEmax1], CaloHit_fEnergy[idxEmax2]);
//	htt->Fill(CaloHit_fTheta[idxEmax1] * 180.0 / TMath::Pi(), CaloHit_fTheta[idxEmax2] * 180.0 / TMath::Pi());
//      }
//   }
//}
