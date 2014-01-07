#include "../drawStyle.C"



TString fileName = "/Users/kresan/neuland/r3bcalibr.s406.test.1500AMeV.root";



void plot_etot()
{
  TPstyle();
//   gStyle->SetOptStat(1);

  TFile *file = new TFile(fileName);
  TH1F* h_etot = (TH1F*) file->Get("Total_Energy_Reco");
  h_etot->SetBinContent(1, 0.);
  h_etot->GetXaxis()->SetRangeUser(0., 1000.);
  h_etot->Scale(1./(Double_t)h_etot->Integral());
  Style(h_etot, "total energy deposit (MeV)", "probability");

//  TFile *file_I = new TFile("data/totene3.root");
//  TH1F* h_etot_I = (TH1F*) file_I->Get("hTotalEnergy2MeV");
  TFile *file_I = new TFile("data/Dima1500.root");
  TH1F* h_etot_I = (TH1F*) file_I->Get("hTotalEnergy2");
  h_etot_I->SetLineColor(2);
  h_etot_I->SetStats(kFALSE);
  h_etot_I->SetBinContent(1, 0);
  h_etot_I->GetXaxis()->SetRangeUser(0., 1000.);
  h_etot_I->Sumw2();
  h_etot_I->SetMarkerColor(2);
  h_etot_I->SetMarkerStyle(20);
  h_etot_I->SetMarkerSize(0.8);
  h_etot_I->Scale(1./(Double_t)h_etot_I->Integral());
  Style(h_etot_I, "total energy deposit (MeV)", "probability");

  cout << h_etot->GetMean() << "   " << h_etot_I->GetMean() << endl;

  gStyle->SetPaperSize(32, 16);
  TCanvas *c1 = new TCanvas("c1", "", 10, 10, 900, 450);
  c1->Divide(2, 1);
  TPostScript *ps1 = new TPostScript("plots/s406.1500AMeV.etot.eps", -113);
  ps1->Range(32, 16);
  c1->cd(1);
  gPad->SetLeftMargin(0.15);
  gPad->SetRightMargin(0.05);
  gPad->SetTickx();
  gPad->SetTicky();
  gPad->SetLogy();
  h_etot_I->DrawClone();
  h_etot->DrawClone("same");
  TLegend *leg1 = new TLegend(0.330, 0.737, 0.916, 0.880);
  Style(leg1);
  char str[100];
  sprintf(str, "experiment: %2.1f", h_etot_I->GetMean());
  leg1->AddEntry(h_etot_I, str, "pl");
  sprintf(str, "simulation: %2.1f", h_etot->GetMean());
  leg1->AddEntry(h_etot, str, "l");
  leg1->Draw();
  c1->cd(2);
  gPad->SetLeftMargin(0.15);
  gPad->SetRightMargin(0.05);
  gPad->SetTickx();
  gPad->SetTicky();
  h_etot_I->GetXaxis()->SetRangeUser(0., 150.);
  h_etot_I->DrawClone();
  h_etot->DrawClone("same");
  c1->cd(0);
  ps1->Close();


//   TFile *ofile = new TFile("dima_etot_withoutcut.root", "RECREATE");
//   h_etot->Write();
//   ofile->Close();
}



void plot_paddle()
{
  TPstyle();

  TFile *file = new TFile(fileName);
  TH1F* h_ep = (TH1F*) file->Get("PaddleEnergy");
  Style(h_ep, "energy deposit in a paddle (MeV)", "probability");
  h_ep->Scale(1./(Double_t)h_ep->Integral());
  h_ep->GetYaxis()->SetTitleOffset(1.8);

//   TFile *file_old = new TFile("dima_epaddle_withoutcut.root");
//   TH1F* h_old = (TH1F*) file_old->Get("PaddleEnergy");
//   h_old->Scale(1./(Double_t)h_old->Integral());
//   h_old->SetLineColor(1);

//  TFile *file_I = new TFile("data/single_energy.root");
//  TH1F* h_ep_I = (TH1F*) file_I->Get("single");
  TFile *file_I = new TFile("data/Dima1500.root");
  TH1F* h_ep_I = (TH1F*) file_I->Get("hEnergyB2");
  h_ep_I->SetLineColor(2);
  h_ep_I->SetLineWidth(2);
  h_ep_I->SetMarkerColor(2);
  h_ep_I->SetMarkerSize(0.8);
  h_ep_I->SetMarkerStyle(20);
  h_ep_I->SetStats(kFALSE);
  h_ep_I->Sumw2();
  h_ep_I->Scale(1./(Double_t)h_ep_I->Integral());
  Style(h_ep_I, "energy deposit in a paddle (MeV)", "probability");

  gStyle->SetPaperSize(32, 16);
  TCanvas *c1 = new TCanvas("c1", "", 10, 10, 900, 450);
  c1->Divide(2, 1);
  TPostScript *ps1 = new TPostScript("plots/s406.1500AMeV.paddle.eps", -113);
  ps1->Range(32, 16);
  c1->cd(1);
  gPad->SetLeftMargin(0.18);
  gPad->SetRightMargin(0.05);
  gPad->SetTickx();
  gPad->SetTicky();
  h_ep->Draw();
  h_ep_I->Draw("pe1same");
  h_ep->Draw("same");
//   h_old->Draw("same");
  TLegend *leg1 = new TLegend(0.312, 0.718, 0.919, 0.880);
  Style(leg1);
  char str[100];
  sprintf(str, "experiment: %2.1f", h_ep_I->GetMean());
  leg1->AddEntry(h_ep_I, str, "pl");
  sprintf(str, "simulation: %2.1f", h_ep->GetMean());
  leg1->AddEntry(h_ep, str, "l");
  leg1->Draw();
  c1->cd(2);
  gPad->SetLeftMargin(0.18);
  gPad->SetRightMargin(0.05);
  gPad->SetTickx();
  gPad->SetTicky();
  gPad->SetLogy();
  h_ep->Draw();
  h_ep_I->Draw("pe1same");
  h_ep->Draw("same");
  c1->cd(0);
  ps1->Close();

  cout << h_ep->GetMean() << " +- " << h_ep->GetMeanError() << endl
  << h_ep_I->GetMean() << " +- " << h_ep_I->GetMeanError() << endl;

//   TFile *ofile = new TFile("dima_epaddle_withoutcut.root", "RECREATE");
//   h_ep->Write();
//   ofile->Close();
}



void plot_first_paddle()
{
  TPstyle();

  TFile *file = new TFile(fileName);
  TH1F* h_ep = (TH1F*) file->Get("FirstTvsBar");
  Style(h_ep, "first paddle", "probability");
  h_ep->Scale(1./(Double_t)h_ep->Integral());

  TFile *file_I = new TFile("data/first_hit.root");
  TH2F* h_ep_I = (TH2F*) file_I->Get("hFirstTvsBar_t_px");
  h_ep_I->SetLineColor(2);
  h_ep_I->SetLineWidth(2);
  h_ep_I->SetLineStyle(2);
  h_ep_I->SetStats(kFALSE);
  h_ep_I->Scale(1./(Double_t)h_ep_I->Integral());
  Style(h_ep_I, "first paddle", "probability");

  gStyle->SetPaperSize(16, 16);
  TCanvas *c1 = new TCanvas("c1", "", 10, 10, 450, 450);
  TPostScript *ps1 = new TPostScript("plots/s406.first.paddle.eps", -113);
  ps1->Range(16, 16);
  gPad->SetLeftMargin(0.18);
  gPad->SetRightMargin(0.05);
  gPad->SetTickx();
  gPad->SetTicky();
  h_ep_I->Draw();
  h_ep->Draw("same");
  TLegend *leg1 = new TLegend(0.352, 0.718, 0.919, 0.880);
  Style(leg1);
  char str[100];
  sprintf(str, "experiment: %2.1f", h_ep_I->GetMean());
  leg1->AddEntry(h_ep_I, str, "l");
  sprintf(str, "simulation: %2.1f", h_ep->GetMean());
  leg1->AddEntry(h_ep, str, "l");
  leg1->Draw();
  ps1->Close();

  cout << h_ep->GetMean() << "  " << h_ep_I->GetMean() << endl;
}



void plot_first_energy()
{
  TPstyle();
  
  TFile *file = new TFile(fileName);
  TH1F* h_ep = (TH1F*) file->Get("FirstE");
  Style(h_ep, "energy deposit in first paddle (MeV)", "probability");
  h_ep->Scale(1./(Double_t)h_ep->Integral());
  h_ep->GetXaxis()->SetRangeUser(0., 100.);
  
  TFile *file_I = new TFile("data/first_hit_energy.root");
  TH2F* h_ep_I = (TH2F*) file_I->Get("hFirstE_t");
  h_ep_I->SetLineColor(2);
  h_ep_I->SetLineWidth(2);
  h_ep_I->SetMarkerColor(2);
  h_ep_I->SetMarkerSize(0.8);
  h_ep_I->SetMarkerStyle(20);
  h_ep_I->SetStats(kFALSE);
  h_ep_I->Sumw2();
  h_ep_I->Scale(1./(Double_t)h_ep_I->Integral());
  Style(h_ep_I, "energy deposit in first paddle (MeV)", "probability");
  h_ep_I->GetXaxis()->SetRangeUser(0., 100.);
  
  gStyle->SetPaperSize(16, 16);
  TCanvas *c1 = new TCanvas("c1", "", 10, 10, 450, 450);
  TPostScript *ps1 = new TPostScript("plots/s406.first.energy.eps", -113);
  ps1->Range(16, 16);
  gPad->SetLeftMargin(0.18);
  gPad->SetRightMargin(0.05);
  gPad->SetTickx();
  gPad->SetTicky();
  gPad->SetLogy();
  h_ep_I->Draw("pe1");
//  h_ep_I->Draw("pe1same");
  h_ep->Draw("same");
  TLegend *leg1 = new TLegend(0.213, 0.149, 0.818, 0.313);
  Style(leg1);
  char str[100];
  sprintf(str, "experiment: %2.1f", h_ep_I->GetMean());
  leg1->AddEntry(h_ep_I, str, "pl");
  sprintf(str, "simulation: %2.1f", h_ep->GetMean());
  leg1->AddEntry(h_ep, str, "l");
  leg1->Draw();
  ps1->Close();
  
  cout << h_ep->GetMean() << "  " << h_ep_I->GetMean() << endl;
}



void plot_thresh()
{
  TPstyle();

  TFile *file = new TFile(fileName);
  TH2F* h_thr = (TH2F*)file->Get("Thresh");
  Style(h_thr, "paddle number", "energy deposit in a paddle (MeV)");
  h_thr->GetYaxis()->SetRangeUser(0., 60.);
  h_thr->Scale(1./h_thr->Integral());
  h_thr->GetZaxis()->SetRangeUser(0., 0.18e-3);

  TFile *file_I = new TFile("data/energy_vs_bar.root");
  TH2F* h_thr_I = (TH2F*)file_I->Get("hEnergy_all_cut");
  Style(h_thr_I, "paddle number", "energy deposit in a paddle (MeV)");
  h_thr_I->Scale(1./h_thr_I->Integral());
  h_thr_I->GetZaxis()->SetRangeUser(0., 0.18e-3);

  gStyle->SetPaperSize(32, 16);
  TCanvas *c1 = new TCanvas("c1", "", 10, 10, 900, 450);
  c1->Divide(2, 1);
  TPostScript *ps1 = new TPostScript("plots/s406.thresh.eps", -113);
  ps1->Range(32, 16);
  c1->cd(1);
  gPad->SetLeftMargin(0.15);
  gPad->SetRightMargin(0.05);
  gPad->SetTickx();
  gPad->SetTicky();
  h_thr->Draw("col");
  c1->cd(2);
  gPad->SetLeftMargin(0.15);
  gPad->SetRightMargin(0.05);
  gPad->SetTickx();
  gPad->SetTicky();
  h_thr_I->Draw("col");
//   cout << h_thr->GetYaxis()->GetNbins() << "   " << h_thr_I->GetYaxis()->GetNbins() << endl;
  c1->cd(0);
  ps1->Close();
}



void plot_mult()
{
  TPstyle();

  TFile *file = new TFile(fileName);
  TH1F* h_m = (TH1F*) file->Get("Multiplicity1");
  Style(h_m, "multiplicity", "probability");
  h_m->SetBinContent(1, 0);
//  h_m->SetBinContent(2, 0);
  h_m->Scale(1./(Double_t)h_m->Integral());

//  TFile *file_I = new TFile("data/hdata.root");
//  TH1F* h_m_I = (TH1F*) file_I->Get("hMulti125");
  TFile *file_I = new TFile("data/Dima1500.root");
  TH1F* h_m_I = (TH1F*) file_I->Get("hMulti2");
  h_m_I->SetLineColor(2);
  h_m_I->SetStats(kFALSE);
  h_m_I->SetBinContent(1, 0);
//  h_m_I->SetBinContent(2, 0);
  h_m_I->Sumw2();
  h_m_I->SetMarkerColor(2);
  h_m_I->SetMarkerStyle(20);
  h_m_I->SetMarkerSize(0.8);
  h_m_I->Scale(1./(Double_t)h_m_I->Integral());
  Style(h_m_I, "multiplicity", "probability");
  h_m_I->GetXaxis()->SetRangeUser(0., 50.);

  gStyle->SetPaperSize(32, 16);
  TCanvas *c1 = new TCanvas("c1", "", 10, 10, 900, 450);
  c1->Divide(2, 1);
  TPostScript *ps1 = new TPostScript("plots/s406.1500AMeV.mult.eps", -113);
  ps1->Range(32, 16);
  c1->cd(1);
  gPad->SetLeftMargin(0.15);
  gPad->SetRightMargin(0.05);
  gPad->SetTickx();
  gPad->SetTicky();
  h_m_I->Draw();
  h_m->Draw("same");
  TLegend *leg1 = new TLegend(0.309, 0.698, 0.914, 0.860);
  Style(leg1);
  char str[100];
  sprintf(str, "experiment: %2.1f", h_m_I->GetMean());
  leg1->AddEntry(h_m_I, str, "pl");
  sprintf(str, "simulation: %2.1f", h_m->GetMean());
  leg1->AddEntry(h_m, str, "l");
  leg1->Draw();
  c1->cd(2);
  gPad->SetLeftMargin(0.15);
  gPad->SetRightMargin(0.05);
  gPad->SetTickx();
  gPad->SetTicky();
  gPad->SetLogy();
  h_m_I->Draw();
  h_m->Draw("same");
  c1->cd(0);
  ps1->Close();

  cout << h_m->GetMean() << "  " << h_m_I->GetMean() << endl;

//   TFile *ofile = new TFile("dima_mult_withoutcut.root", "RECREATE");
//   h_m->Write();
//   ofile->Close();
}



void plot_3D()
{
  TPstyle();


  TFile *file = new TFile(fileName);
  TTree *tree = (TTree*) file->Get("cbmsim");
  TClonesArray *hits = new TClonesArray("R3BLandDigi");
  TClonesArray *clusters = new TClonesArray("R3BNeuLandCluster");
  tree->SetBranchAddress("LandDigi", &hits);
  tree->SetBranchAddress("NeuLandCluster", &clusters);


  TH2F *h1 = new TH2F("h1", "", 20, 1120., 1220., 12, -30., 30.);
  Style(h1, "z (cm)", "x (cm)");


  gStyle->SetPaperSize(16, 16);
  TCanvas *c1 = new TCanvas("c1", "", 10, 10, 500, 500);
  gPad->SetLeftMargin(0.15);
  gPad->SetRightMargin(0.15);


  Int_t nhits;
  R3BLandDigi *hit;
  Int_t ncl;
  R3BNeuLandCluster *cl;
  TVector3 startPos, stopPos;
  TArrow *lines[1000];
  TArc *arc[1000];
  for(Int_t iev = 0; iev < 1000; iev++) {
    h1->Reset();

    tree->GetEntry(iev);

    nhits = hits->GetEntriesFast();
    if(nhits < 20) continue;

    for(Int_t i = 0; i < nhits; i++) {
      hit = (R3BLandDigi*) hits->At(i);
      h1->Fill(hit->GetZZ(), hit->GetXX(), hit->GetYY());
    }

    ncl = clusters->GetEntriesFast();
    for(Int_t i = 0; i < ncl; i++) {
      cl = (R3BNeuLandCluster*) clusters->At(i);
      cl->StartPosition(startPos);
      cl->StopPosition(stopPos);
      lines[i] = new TArrow(startPos.Z(), startPos.X(),
			   stopPos.Z(), stopPos.X());
      lines[i]->SetLineColor(1);
      lines[i]->SetLineWidth(3);

      arc[i] = NULL;
      if(cl->GetSize() < 2) {
	arc[i] = new TArc(startPos.Z(), startPos.X(), 2);
      }
    }

    char str[100];
    sprintf(str, "plots/evd/clusters_old_%1d.eps", iev);
    TPostScript *ps1 = new TPostScript(str, -113);
    ps1->Range(16, 16);

    h1->Draw("colz");

    for(Int_t i = 0; i < ncl; i++) {
      if(arc[i]) {
	arc[i]->Draw();
      } else {
	lines[i]->Draw();
      }
    }

    c1->Update();
    ps1->Close();
    delete ps1;

    cout << iev << endl;
    char c;
    cin >> c;
  }
}
