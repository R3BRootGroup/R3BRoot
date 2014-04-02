#include "../../land/drawStyle.C"

void plot_s406()
{
  TPstyle();
  
  TFile *file = new TFile("rawData.s406.root");
  TTree *tree = (TTree*) file->Get("cbmsim");
  TH1F *h_id = new TH1F("h_id", "", 160, -4.5, 155.5);
  tree->Draw("LandHit.fBarId>>h_id", "LandHit.fSide==1", "goff");
  TH1F *h_time = new TH1F("h_time", "", 450, 0., 4500.);
  tree->Draw("LandHit.fTime>>h_time", "LandHit.fIs17==0", "goff");
  TH1F *h_qdc = new TH1F("h_qdc", "", 150, 0., 1500.);
  tree->Draw("LandHit.fCharge>>h_qdc", "LandHit.fIs17==0", "goff");
  
  Style(h_id, "Bar ID", "counts");
  Style(h_time, "Time", "counts");
  Style(h_qdc, "Charge", "counts");
  
  gStyle->SetPaperSize(16, 16);
  
  TCanvas *c2 = new TCanvas("c2", "", 10, 10, 450, 450);
  TPostScript *ps2 = new TPostScript("plots/s406_barid.eps", -113);
  ps2->Range(16, 16);
  gPad->SetLeftMargin(0.15);
  gPad->SetRightMargin(0.05);
  h_id->Draw();
  ps2->Close();

  TCanvas *c3 = new TCanvas("c3", "", 200, 100, 450, 450);
  TPostScript *ps3 = new TPostScript("plots/s406_time.eps", -113);
  ps3->Range(16, 16);
  gPad->SetLeftMargin(0.15);
  gPad->SetRightMargin(0.05);
  h_time->Draw();
  ps3->Close();
  
  TCanvas *c4 = new TCanvas("c4", "", 300, 200, 450, 450);
  TPostScript *ps4 = new TPostScript("plots/s406_qdc.eps", -113);
  ps4->Range(16, 16);
  gPad->SetLeftMargin(0.15);
  gPad->SetRightMargin(0.05);
  h_qdc->Draw();
  ps4->Close();
}

