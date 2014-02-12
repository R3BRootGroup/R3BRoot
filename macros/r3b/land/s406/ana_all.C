
#include "../drawStyle.C"

void plot_paddle()
{
  TPstyle();
  
  TString names[] = {"/Users/kresan/neuland/r3bcalibr.s406.255AMeV.root",
    "/Users/kresan/neuland/r3bcalibr.s406.498AMeV.root",
    "/Users/kresan/neuland/r3bcalibr.s406.799AMeV.root",
    "/Users/kresan/neuland/r3bcalibr.s406.1014AMeV.root",
    "/Users/kresan/neuland/r3bcalibr.s406.1500AMeV.root"
  };
  TString en[] = {"255 AMeV", "498 AMeV", "799 AMeV", "1014 AMeV", "1500 AMeV"};
  TString eno[] = {"_255", "_498", "_799", "_1014", "_1500"};
  
  TFile *file[5];
  TH1F *h1[5];
  TH1F *hm[5];
  Double_t nev[5];
  Double_t binsize;
  for(Int_t i = 0; i < 5; i++) {
    file[i] = new TFile(names[i]);
    h1[i] = (TH1F*) file[i]->Get("PaddleEnergy");
    hm[i] = (TH1F*) file[i]->Get("Multiplicity1");
    nev[i] = hm[i]->Integral(2, hm[i]->GetXaxis()->GetNbins());

    if(0 == i) {
      binsize = (Double_t)(h1[i]->GetXaxis()->GetXmax() - h1[i]->GetXaxis()->GetXmin()) / (Double_t)h1[i]->GetXaxis()->GetNbins();
    }
    h1[i]->Scale(1./binsize);

    h1[i]->SetLineColor(i+1);
    h1[i]->Scale(1./nev[i]);
    
    cout << "Unity test " << i << " value: " << h1[i]->Integral(1, h1[i]->GetXaxis()->GetNbins()) << endl;
  }
  
  cout << binsize << " MeV" << endl;
  
  Style(h1[4], "Energy per paddle (MeV)", "probability #times N_{mean}");
  h1[4]->SetLineColor(6);
//  h1[4]->Scale(2.2);
  
  
  TFile *ifile = new TFile("data/all_histos.root");
  TH1F *himult[4];
  TH1F *hien[4];
  TString ien[] = {"300", "500", "800", "1500"};
  Double_t inev[4];
  for(Int_t i = 0; i < 4; i++) {
    himult[i] = (TH1F*) ifile->Get((TString("hMulti")+ien[i]).Data());
    hien[i] = (TH1F*) ifile->Get((TString("hEnergy")+ien[i]).Data());
    inev[i] = himult[i]->Integral(2, himult[i]->GetXaxis()->GetNbins());
    hien[i]->Sumw2();
    hien[i]->Scale(1./binsize);
    hien[i]->Scale(1./inev[i]);
    hien[i]->SetMarkerStyle(24);
    cout << inev[i] << endl;
  }
  hien[0]->SetLineColor(1);
  hien[1]->SetLineColor(2);
  hien[2]->SetLineColor(3);
  hien[3]->SetLineColor(6);

  hien[0]->SetMarkerColor(1);
  hien[1]->SetMarkerColor(2);
  hien[2]->SetMarkerColor(3);
  hien[3]->SetMarkerColor(6);

  
  
  gStyle->SetPaperSize(16, 16);
  TCanvas *c1 = new TCanvas("c1", "", 10, 10, 450, 450);
  TPostScript *ps1 = new TPostScript("plots/all_paddle.eps", -113);
  ps1->Range(16, 16);
  gPad->SetLeftMargin(0.15);
  gPad->SetRightMargin(0.05);
  gPad->SetTickx();
  gPad->SetTicky();
  h1[4]->GetXaxis()->SetRangeUser(0., 60.);
  h1[4]->DrawClone();
  TLegend *leg1 = new TLegend(0.486, 0.432, 0.912, 0.872);
  Style(leg1);
  leg1->AddEntry(h1[4], en[4], "l");
  for(Int_t i = 3; i >= 0; i--) {
    if(3 != i) {
      h1[i]->DrawClone("same");
      leg1->AddEntry(h1[i], en[i], "l");
    }
  }
  leg1->Draw();
  for(Int_t i = 0; i < 4; i++) {
    hien[i]->Draw("pe1same");
  }
  ps1->Close();

  
  
  gStyle->SetPaperSize(16, 16);
  TCanvas *c2 = new TCanvas("c2", "", 10, 10, 450, 450);
  TPostScript *ps2 = new TPostScript("plots/all_paddle_log.eps", -113);
  ps2->Range(16, 16);
  gPad->SetLeftMargin(0.15);
  gPad->SetRightMargin(0.05);
  gPad->SetTickx();
  gPad->SetTicky();
  gPad->SetLogy();
  h1[4]->GetXaxis()->SetRangeUser(0., 80.);
  h1[4]->Draw();
  for(Int_t i = 3; i >= 0; i--) {
    if(3 != i) {
      h1[i]->Draw("same");
    }
  }
  for(Int_t i = 0; i < 4; i++) {
    hien[i]->Draw("pe1same");
  }
  ps2->Close();

  
  TFile *ofile = new TFile("s406.sim.all.root", "RECREATE");
  for(Int_t i = 0; i < 5; i++) {
    h1[i]->SetName((TString(h1[i]->GetName())+eno[i]).Data());
    h1[i]->Write();
  }
  ofile->Close();
}





void plot_mult()
{
  TPstyle();
  
  TString names[] = {"/Users/kresan/neuland/r3bcalibr.s406.255AMeV.root",
    "/Users/kresan/neuland/r3bcalibr.s406.498AMeV.root",
    "/Users/kresan/neuland/r3bcalibr.s406.799AMeV.root",
    "/Users/kresan/neuland/r3bcalibr.s406.1014AMeV.root",
    "/Users/kresan/neuland/r3bcalibr.s406.1500AMeV.root"
  };
  TString en[] = {"255 AMeV", "498 AMeV", "799 AMeV", "1014 AMeV", "1500 AMeV"};
  TString eno[] = {"_255", "_498", "_799", "_1014", "_1500"};
  
  TFile *file[5];
  TH1F *h1[5];
  Double_t nev[5];
  for(Int_t i = 0; i < 5; i++) {
    file[i] = new TFile(names[i]);
    h1[i] = (TH1F*) file[i]->Get("Multiplicity1");
    nev[i] = h1[i]->Integral(2, 100);
    h1[i]->SetLineColor(i+1);
    h1[i]->SetBinContent(1, 0);
    h1[i]->Scale(1./nev[i]);
  }
  
  Style(h1[0], "Multiplicity", "probability");
  h1[4]->SetLineColor(6);
//  h1[4]->Scale(2.2);
  
  
  TFile *ifile = new TFile("data/all_histos.root");
  TH1F *himult[4];
  TH1F *hien[4];
  TString ien[] = {"300", "500", "800", "1500"};
  Double_t inev[4];
  for(Int_t i = 0; i < 4; i++) {
    himult[i] = (TH1F*) ifile->Get((TString("hMulti")+ien[i]).Data());
    hien[i] = (TH1F*) ifile->Get((TString("hMulti")+ien[i]).Data());
    inev[i] = himult[i]->GetEntries();
    hien[i]->Sumw2();
    hien[i]->Scale(1./inev[i]);
//    hien[i]->SetMarkerSize(1.2);
    hien[i]->SetMarkerStyle(20);
    hien[i]->SetStats(kFALSE);
    hien[i]->SetBinContent(1, 0);
    cout << inev[i] << endl;
  }
  hien[0]->SetLineColor(1);
  hien[1]->SetLineColor(2);
  hien[2]->SetLineColor(3);
  hien[3]->SetLineColor(6);
  hien[0]->SetMarkerColor(1);
  hien[1]->SetMarkerColor(2);
  hien[2]->SetMarkerColor(3);
  hien[3]->SetMarkerColor(6);
  
  
  
  gStyle->SetPaperSize(16, 16);
  TCanvas *c1 = new TCanvas("c1", "", 10, 10, 450, 450);
  TPostScript *ps1 = new TPostScript("plots/all_mult.eps", -113);
  ps1->Range(16, 16);
  gPad->SetLeftMargin(0.15);
  gPad->SetRightMargin(0.05);
  gPad->SetTickx();
  gPad->SetTicky();
  h1[0]->GetXaxis()->SetRangeUser(0., 40.);
  h1[0]->DrawClone();
  TLegend *leg1 = new TLegend(0.486, 0.432, 0.912, 0.872);
  Style(leg1);
  for(Int_t i = 4; i >= 1; i--) {
    if(3 != i) {
      h1[i]->DrawClone("same");
      leg1->AddEntry(h1[i], en[i], "l");
    }
  }
  leg1->AddEntry(h1[0], en[0], "l");
  leg1->Draw();
  for(Int_t i = 0; i < 4; i++) {
    hien[i]->Draw("pe1same");
  }
  ps1->Close();

  
  gStyle->SetPaperSize(16, 16);
  TCanvas *c2 = new TCanvas("c2", "", 10, 10, 450, 450);
  TPostScript *ps2 = new TPostScript("plots/all_mult_log.eps", -113);
  ps2->Range(16, 16);
  gPad->SetLeftMargin(0.15);
  gPad->SetRightMargin(0.05);
  gPad->SetTickx();
  gPad->SetTicky();
  gPad->SetLogy();
  h1[0]->GetXaxis()->SetRangeUser(0., 50.);
  h1[0]->Draw();
  for(Int_t i = 4; i >= 1; i--) {
    if(3 != i) {
      h1[i]->Draw("same");
    }
  }
  for(Int_t i = 0; i < 4; i++) {
    hien[i]->Draw("pe1same");
  }
  ps2->Close();
  
//  TFile *ofile = new TFile("s406.sim.all.root", "RECREATE");
//  for(Int_t i = 0; i < 5; i++) {
//    h1[i]->SetName((TString(h1[i]->GetName())+eno[i]).Data());
//    h1[i]->Write();
//  }
//  ofile->Close();
}





void plot_etot()
{
  TPstyle();
  
  TString names[] = {"/Users/kresan/neuland/r3bcalibr.s406.255AMeV.root",
    "/Users/kresan/neuland/r3bcalibr.s406.498AMeV.root",
    "/Users/kresan/neuland/r3bcalibr.s406.799AMeV.root",
    "/Users/kresan/neuland/r3bcalibr.s406.1014AMeV.root",
    "/Users/kresan/neuland/r3bcalibr.s406.1500AMeV.root"
  };
  TString en[] = {"255 AMeV", "498 AMeV", "799 AMeV", "1014 AMeV", "1500 AMeV"};
  TString eno[] = {"_255", "_498", "_799", "_1014", "_1500"};
  
  TFile *file[5];
  TH1F *h1[5];
  TH1F *hm[5];
  Double_t nev[5];
  Double_t binsize;
  for(Int_t i = 0; i < 5; i++) {
    file[i] = new TFile(names[i]);
    h1[i] = (TH1F*) file[i]->Get("Total_Energy_Reco");
    hm[i] = (TH1F*) file[i]->Get("Multiplicity1");
    nev[i] = hm[i]->Integral(2, 100);
    h1[i]->SetLineColor(i+1);
    
    h1[i]->Scale(1./nev[i]);
    if(0 == i) {
      binsize = (Double_t)(h1[i]->GetXaxis()->GetXmax() - h1[i]->GetXaxis()->GetXmin()) / (Double_t)h1[i]->GetXaxis()->GetNbins();
    }
    h1[i]->Scale(1./binsize);
    h1[i]->SetBinContent(1, 0);
    
    cout << "Unity test " << i << " value: " << h1[i]->Integral(1, h1[i]->GetXaxis()->GetNbins()) << endl;
  }
  
  cout << binsize << " MeV" << endl;
  
  Style(h1[4], "Total energy (MeV)", "probability");
  h1[4]->SetLineColor(6);
  //  h1[4]->Scale(2.2);
  
  
  TFile *ifile = new TFile("data/all_histos.root");
  TH1F *himult[4];
  TH1F *hien[4];
  TString ien[] = {"300", "500", "800", "1500"};
  Double_t inev[4];
  for(Int_t i = 0; i < 4; i++) {
    himult[i] = (TH1F*) ifile->Get((TString("hMulti")+ien[i]).Data());
    hien[i] = (TH1F*) ifile->Get((TString("hTotalEnergy")+ien[i]).Data());
    inev[i] = himult[i]->Integral(2, 100);
    hien[i]->Sumw2();
    hien[i]->Scale(1./inev[i]);
    hien[i]->Scale(1./binsize);
    hien[i]->SetBinContent(1, 0);
    hien[i]->SetMarkerStyle(24);
    hien[i]->SetLineStyle(2);
    cout << inev[i] << endl;
  }
  hien[0]->SetLineColor(1);
  hien[1]->SetLineColor(2);
  hien[2]->SetLineColor(3);
  hien[3]->SetLineColor(6);
  
  hien[0]->SetMarkerColor(1);
  hien[1]->SetMarkerColor(2);
  hien[2]->SetMarkerColor(3);
  hien[3]->SetMarkerColor(6);
  
  
  
  gStyle->SetPaperSize(16, 16);
  TCanvas *c1 = new TCanvas("c1", "", 10, 10, 450, 450);
  TPostScript *ps1 = new TPostScript("plots/all_etot_log.eps", -113);
  ps1->Range(16, 16);
  gPad->SetLeftMargin(0.15);
  gPad->SetRightMargin(0.05);
  gPad->SetTickx();
  gPad->SetTicky();
  gPad->SetLogy();
  h1[4]->GetXaxis()->SetRangeUser(0., 700.);
  h1[4]->DrawClone();
  for(Int_t i = 0; i < 4; i++) {
    hien[i]->Draw("pe1same");
  }
  ps1->Close();

  
  gStyle->SetPaperSize(16, 16);
  TCanvas *c2 = new TCanvas("c2", "", 10, 10, 450, 450);
  TPostScript *ps2 = new TPostScript("plots/all_etot.eps", -113);
  ps2->Range(16, 16);
  gPad->SetLeftMargin(0.15);
  gPad->SetRightMargin(0.05);
  gPad->SetTickx();
  gPad->SetTicky();
  h1[4]->GetXaxis()->SetRangeUser(0., 150.);
  h1[4]->GetYaxis()->SetRangeUser(0., 0.022);
  h1[4]->Draw();
  for(Int_t i = 3; i >= 0; i--) {
    if(3 != i) {
      h1[i]->Draw("same");
    }
  }
  for(Int_t i = 0; i < 4; i++) {
    hien[i]->Draw("pe1same");
  }
  TLegend *leg1 = new TLegend(0.486, 0.432, 0.912, 0.872);
  Style(leg1);
  leg1->AddEntry(h1[4], en[4], "l");
  for(Int_t i = 3; i >= 0; i--) {
    if(3 != i) {
      h1[i]->DrawClone("same");
      leg1->AddEntry(h1[i], en[i], "l");
    }
  }
  leg1->Draw();
  ps2->Close();
  
  TFile *ofile = new TFile("s406.sim.all.root", "RECREATE");
  for(Int_t i = 0; i < 5; i++) {
    h1[i]->SetName((TString(h1[i]->GetName())+eno[i]).Data());
    h1[i]->Write();
  }
  ofile->Close();
}





