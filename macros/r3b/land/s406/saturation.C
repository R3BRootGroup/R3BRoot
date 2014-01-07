#include "../drawStyle.C"


void saturation()
{
  TPstyle();

  TF1 *f0 = new TF1("f0", "x", 0., 100.);
  f0->SetLineColor(1);
  TF1 *f1 = new TF1("f1", "x/(1+[0]*x)", 0., 100.);
  f1->SetParameter(0, 0.012);
  f1->SetLineColor(2);
  TF1 *f2 = new TF1("f2", "[1]+(x-[1])/(1+[0]*(x-[1]))", 20., 100.);
  f2->SetParameters(0.012, 20.);
  f2->SetLineColor(4);

  Double_t x[] = {0., 100.};
  Double_t y[] = {0., 100.};
  TGraph *gr = new TGraph(2, x, y);
  
  gStyle->SetPaperSize(16, 16);
  TCanvas *c1 = new TCanvas("c1", "", 10, 10, 450, 450);
  TPostScript *ps1 = new TPostScript("plots/s406.saturation.eps", -113);
  ps1->Range(16, 16);
  gPad->SetLeftMargin(0.15);
  gPad->SetRightMargin(0.05);
  gPad->SetGridx();
  gPad->SetGridy();
  gPad->SetTickx();
  gPad->SetTicky();
  gr->Draw("AP");
  Style(gr->GetHistogram(), "input (MeV)", "output (MeV)");
  gr->GetHistogram()->GetXaxis()->SetNdivisions(510);
  gr->GetHistogram()->GetYaxis()->SetNdivisions(510);
  f0->Draw("same");
  f1->Draw("same");
  f2->Draw("same");
  ps1->Close();
}
