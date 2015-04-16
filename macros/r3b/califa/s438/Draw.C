#define Draw_cxx
#include "Draw.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TH1I.h>

void Draw::Energy(UInt_t CrystalId, TH1 *hist)
{
   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();

   hist = getHist(hist);
   Int_t j;

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;

      for(j = 0; j < CaloCrystalHit_; j++)
	{
		if(CaloCrystalHit_fCrystalId[j] == CrystalId)
			hist->Fill(CaloCrystalHit_fEnergy[j]);
	}
   }
}

void Draw::Pid(UInt_t CrystalId, TH2 *hist)
{
   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();

   hist = getHist(hist);
   Int_t j;

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;

      for(j = 0; j < CaloCrystalHit_; j++)
	{
		if(CaloCrystalHit_fCrystalId[j] == CrystalId)
			hist->Fill(CaloCrystalHit_fNf[j], CaloCrystalHit_fNs[j]);
	}
   }

   hist->Draw("colz");
}

TH1 *Draw::getHist(TH1 *hist)
{
	if(hist)
		return hist;

	return new TH1I("hist", "Histogram", 1000, 0, 300000);	
}

TH2 *Draw::getHist(TH2 *hist)
{
	if(hist)
		return hist;

	return new TH2I("hist", "Histogram", 1000, 0, 350000, 1000, 0, 350000);	
}

void Draw::GenAll(Int_t n_channels, bool drawEnergy, bool drawPid)
{
   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();

   Int_t i, j;
   TH1I **e_hists = new TH1I*[n_channels];
   TH2D **pid_hists = new TH2D*[n_channels];
   for(i = 0; i < n_channels; i++)
   {
      e_hists[i] = new TH1I(Form("he%03d", i), Form("Energy %d", i), 1000, 0, 350000);
      pid_hists[i] = new TH2D(Form("hpid%03d", i), Form("QPID %d", i), 250, 0, 350000, 250, 0, 350000);
   }

   TH2D *pid_total_hist = new TH2D("hpid", "QPID All", 1000, 0, 350000, 1000, 0, 350000);

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;

      for(j = 0; j < CaloCrystalHit_; j++)
	{
	    if(CaloCrystalHit_fCrystalId[j] < n_channels)
            {
               e_hists[CaloCrystalHit_fCrystalId[j]]->Fill(CaloCrystalHit_fEnergy[j]);
               pid_hists[CaloCrystalHit_fCrystalId[j]]->Fill(CaloCrystalHit_fNf[j], CaloCrystalHit_fNs[j]);
               pid_total_hist->Fill(CaloCrystalHit_fNf[j], CaloCrystalHit_fNs[j]);
            }
	}
   }

   if(drawEnergy)
   {
      TCanvas *cenergy = new TCanvas("cenergy", "Energies");
      cenergy->Divide(16, 8);
      
      for(i = 0; i < 128; i++)
      {
         cenergy->cd(i+1);
         e_hists[i]->Draw();
      }
   }

   if(drawPid)
   {
      TCanvas *cpid = new TCanvas("cpid", "QPID");
      cpid->Divide(16, 8);

      for(i = 0; i < 128; i++)
      {
         cpid->cd(i+1);
         pid_hists[i]->Draw("colz");
      }
   }
}

