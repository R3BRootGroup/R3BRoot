
#include "../../land/drawStyle.C"

void plot_s438b()
{
  TPstyle();

  TFile *file_old = new TFile("/Users/kresan/Sites/hist_s438b_run336_raw_land.root");
  TH1F *h_old = (TH1F*) file_old->Get("h_land_mapped_qdc");
  Style(h_old, "QDC raw (a.u.)", "counts");
  TH1F *h_trig_old = (TH1F*) file_old->Get("h_trigger");
  Style(h_trig_old, "trigger value", "events");

  TFile *file = new TFile("/Users/kresan/Sites/hist_s438b_run336_raw.root");
  TH1F *h = (TH1F*) file->Get("h_land_qdc_cut");
  Style(h, "QDC raw (a.u.)", "counts");
  TH1F *ht = (TH1F*) file->Get("hDelay");
  Style(ht, "Time delay (ns)", "counts");
  ht->GetXaxis()->SetRangeUser(-250., 750.);
  TH1F *h_trig = (TH1F*) file->Get("h_trigger_cut");

  gStyle->SetPaperSize(32, 16);
  TCanvas *c1 = new TCanvas("c1", "", 10, 10, 900, 450);
  TPostScript *ps1 = new TPostScript("plots/qdc.eps", -113);
  ps1->Range(32, 16);
  c1->Divide(2, 1);
  c1->cd(1);
  gPad->SetLeftMargin(0.15);
  gPad->SetRightMargin(0.05);
  h_old->Draw();
  c1->cd(2);
  gPad->SetLeftMargin(0.15);
  gPad->SetRightMargin(0.05);
  h->Draw();
  c1->cd(0);
  ps1->Close();

  gStyle->SetPaperSize(16, 16);
  TCanvas *c2 = new TCanvas("c2", "", 100, 100, 450, 450);
  TPostScript *ps2 = new TPostScript("plots/tdelay.eps", -113);
  ps2->Range(16, 16);
  gPad->SetLeftMargin(0.15);
  gPad->SetRightMargin(0.05);
  gPad->SetLogy();
  ht->Draw();
  ps2->Close();


  h_trig->SetFillColor(42);
  gStyle->SetPaperSize(16, 16);
  TCanvas *c3 = new TCanvas("c3", "", 200, 200, 450, 450);
  TPostScript *ps3 = new TPostScript("plots/trigger.eps", -113);
  ps3->Range(16, 16);
  gPad->SetLeftMargin(0.15);
  gPad->SetRightMargin(0.05);
  gPad->SetLogy();
  h_trig_old->Draw();
  h_trig->Draw("same");
  ps3->Close();
}




