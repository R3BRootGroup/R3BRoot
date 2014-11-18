#include "drawStyle.C"




// -----------------------------------------------------------------------------
void plot_minv_tn(Float_t nsigma = 1.3)
{
  TPstyle();


  TFile *file_4n = new TFile("r3breco.600AMeV.4n.100keV.35m.root");
  TH1F *h_minv_4n = (TH1F*) file_4n->Get("Minv");
  Style(h_minv_4n, "E_{rel} (MeV)", "events");
  h_minv_4n->SetLineColor(4);
  // h_minv_4n->Rebin(2);
  h_minv_4n->GetXaxis()->SetRangeUser(0., 1.);
//   h_minv_4n->GetYaxis()->SetRangeUser(0., 440.);

  TF1 *f1 = new TF1("f1", "gaus", 0.1 - 2.5*0.035, 0.1 + 2*0.035);
  f1->FixParameter(1, 0.1);
  h_minv_4n->Fit(f1, "NR+");
  cout << f1->GetParameter(1) << "  +-  " << f1->GetParError(1) << endl;
  cout << f1->GetParameter(2) << "  +-  " << f1->GetParError(2) << endl;
  f1->SetRange(0.1 - 2.5*0.035, 0.1 + (nsigma+0.075)*0.035);

  TF1 *f2 = new TF1("f2", "[0]*exp(-[1]*(x-[2]))", 0.1 + nsigma*0.035, 1);
  f2->FixParameter(0,f1->Eval(0.1 + nsigma*0.035));
  f2->FixParameter(2,0.1 + nsigma*0.035);
  f2->SetLineColor(4);
  h_minv_4n->Fit(f2, "NR+");
  cout << f2->GetParameter(0) << "  +-  " << f2->GetParError(0) << endl;

  gStyle->SetPaperSize(16, 16);
  TCanvas *c1 = new TCanvas("c1", "", 0, 0, 900, 450);
  TPostScript *ps1 = new TPostScript("minv_tetra-neutron.eps", -113);
  ps1->Range(16, 16);

  gPad->SetLeftMargin(0.15);
  gPad->SetRightMargin(0.05);
  gPad->SetTickx();
  gPad->SetTicky();
  h_minv_4n->Draw();
  f1->Draw("same");
  f2->Draw("same");

  char strT[100];
  sprintf(strT, "#sigma = %2.0f keV", f1->GetParameter(2)*1000.);
  TPaveLabel *l1 = new TPaveLabel(0.303, 0.667, 0.693, 0.733, strT, "NDC");
  Style(l1);
  l1->SetTextColor(2);
  l1->Draw();

  TPaveLabel *l2 = new TPaveLabel(0.462, 0.795, 0.915, 0.868, "tetra-neutron", "NDC");
  Style(l2);
  l2->Draw();

//   sprintf(strT, "%3.2f*exp(-%3.2f*(x-%3.2f))", f2->GetParameter(0),f2->GetParameter(1),f2->GetParameter(2));
//   TPaveLabel *l3 = new TPaveLabel(0.507, 0.335, 0.897, 0.401, strT, "NDC");
//   Style(l3);
//   l3->SetTextColor(4);
//   l3->Draw();

  cout << h_minv_4n->Integral() << endl;
  cout << 100.*h_minv_4n->Integral(TMath::Nint((0.1 - 3*0.035 + 0.01)/0.01)+1,
				   TMath::Nint((0.1 + 2*0.035 + 0.01)/0.01)+1) / 
    h_minv_4n->Integral() << endl;

  ps1->Close();
}
// -----------------------------------------------------------------------------



// -----------------------------------------------------------------------------
void plot_minv_tn_compare()
{
  TPstyle();


  TFile *file_4n = new TFile("r3breco.600AMeV.4n.100keV.35m.root");
  TH1F *h_minv_4n = (TH1F*) file_4n->Get("Minv");
  Style(h_minv_4n, "E_{rel} (MeV)", "events");
  h_minv_4n->SetLineColor(4);
  // h_minv_4n->Rebin(2);
  h_minv_4n->GetXaxis()->SetRangeUser(0., 0.5);
  // h_minv_4n->GetYaxis()->SetRangeUser(0., 1000.);

  TFile *file_4n_o = new TFile("r3breco.600AMeV.4n.100keV.35m.old.root");
  TH1F *h_minv_4n_o = (TH1F*) file_4n_o->Get("Minv");
  h_minv_4n_o->SetLineColor(2);

//   TF1 *f1 = new TF1("f1", "gaus", 0.1 - 3*0.035, 0.1 + 2*0.035);
//   f1->FixParameter(1, 0.1);
//   h_minv_4n->Fit(f1, "NR+");
//   cout << f1->GetParameter(1) << "  +-  " << f1->GetParError(1) << endl;
//   cout << f1->GetParameter(2) << "  +-  " << f1->GetParError(2) << endl;

//   TF1 *f2 = new TF1("f2", "[0]*exp(-[1]*x)", 0.1 + 2*0.035, 1.);
//   f2->SetLineColor(4);
//   h_minv_4n->Fit(f2, "NR+");
//   cout << f2->GetParameter(0) << "  +-  " << f2->GetParError(0) << endl;

  gStyle->SetPaperSize(16, 16);
  TCanvas *c1 = new TCanvas("c1", "", 0, 0, 900, 450);
  TPostScript *ps1 = new TPostScript("minv_tetra-neutron_compare.eps", -113);
  ps1->Range(16, 16);

  gPad->SetLeftMargin(0.15);
  gPad->SetRightMargin(0.05);
  gPad->SetTickx();
  gPad->SetTicky();
  h_minv_4n->Draw();
  h_minv_4n_o->Draw("same");
//   f1->Draw("same");
//   f2->Draw("same");

//   char strT[100];
//   sprintf(strT, "#sigma = %2.0f keV", f1->GetParameter(2)*1000.);
//   TPaveLabel *l1 = new TPaveLabel(0.303, 0.667, 0.693, 0.733, strT, "NDC");
//   Style(l1);
//   l1->SetTextColor(2);
//   l1->Draw();

//   TPaveLabel *l2 = new TPaveLabel(0.462, 0.795, 0.915, 0.868, "tetra-neutron", "NDC");
//   Style(l2);
//   l2->Draw();

//   sprintf(strT, "exp(-%3.2fx)", f2->GetParameter(1));
//   TPaveLabel *l3 = new TPaveLabel(0.507, 0.335, 0.897, 0.401, strT, "NDC");
//   Style(l3);
//   l3->SetTextColor(4);
//   l3->Draw();

//   cout << h_minv_4n->Integral() << endl;
//   cout << 100.*h_minv_4n->Integral(TMath::Nint((0.1 - 3*0.035 + 0.01)/0.01)+1,
// 				   TMath::Nint((0.1 + 2*0.035 + 0.01)/0.01)+1) / 
//     h_minv_4n->Integral() << endl;

  ps1->Close();
}
// -----------------------------------------------------------------------------



// -----------------------------------------------------------------------------
void plot_minv_astro()
{
  TPstyle();


  TFile *file_1n = new TFile("r3breco.600AMeV.1n.100keV.35m.root");
  TH1F *h_minv_1n = (TH1F*) file_1n->Get("Minv");
  Style(h_minv_1n, "E_{rel} (MeV)", "events");
  h_minv_1n->SetLineColor(1);
  h_minv_1n->GetXaxis()->SetRangeUser(0., 0.5);
  h_minv_1n->GetYaxis()->SetTitleOffset(1.6);

  TF1 *f1 = new TF1("f1", "gaus", 0.07, 0.13);
  f1->FixParameter(1, 0.1);
  h_minv_1n->Fit(f1, "QNR");

  gStyle->SetPaperSize(16, 16);
  TCanvas *c1 = new TCanvas("c1", "", 0, 0, 900, 450);
  TPostScript *ps1 = new TPostScript("minv_astro.eps", -113);
  ps1->Range(16, 16);

  gPad->SetLeftMargin(0.15);
  gPad->SetRightMargin(0.05);
  gPad->SetTickx();
  gPad->SetTicky();
  h_minv_1n->Draw();
  f1->Draw("same");

  char strT[100];
  sprintf(strT, "#sigma = %2.0f keV", f1->GetParameter(2)*1000.);
  TPaveLabel *l1 = new TPaveLabel(0.527, 0.782, 0.919, 0.851, strT, "NDC");
  Style(l1);
  l1->Draw();

  ps1->Close();
}
// -----------------------------------------------------------------------------


