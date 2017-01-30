#include "../../land/drawStyle.C"

void plot_s438()
{
    TPstyle();
    
    TFile *file = new TFile("/Volumes/Data/kresan/s438/digi/run106_digi.root");
    
    TH1F *h_beta = (TH1F*) file->Get("h_land_beta");
    
    TH1F *h_qdc = (TH1F*) file->Get("h_land_qdc");
    TH1F *h_qdc_cut = (TH1F*) file->Get("h_land_qdc_cut");
    Style(h_beta, "Velocity (cm/ns)", "counts");
    Style(h_qdc, "QDC", "counts");
    h_qdc->GetYaxis()->SetTitleOffset(1.9);
    h_qdc->GetXaxis()->SetRangeUser(0., 1000.);
    h_qdc->SetLineStyle(3);
    h_qdc_cut->SetLineColor(2);
    
    TH2F *h_qdcbarid = (TH2F*) file->Get("h_land_qdcbarid");
    Style(h_qdcbarid, "Bar ID", "QDC channel");
    h_qdcbarid->GetYaxis()->SetTitleOffset(1.6);
    
    TH2F* h_betabarid = (TH2F*) file->Get("h_land_betabarid");
    Style(h_betabarid, "Bar ID", "Velocity (cm/ns)");
    h_betabarid->GetZaxis()->SetRangeUser(3., 10000.);
    
    TH2F* h_yx1 = (TH2F*) file->Get("h_land_yx1");
    Style(h_yx1, "x (cm)", "y (cm)");
    TH2F* h_yx2 = (TH2F*) file->Get("h_land_yx2");
    Style(h_yx2, "x (cm)", "y (cm)");

    TH2F* h_qdctof = (TH2F*) file->Get("h_land_qdctof");
    Style(h_qdctof, "Time of Flight (ns)", "QDC channel");
    h_qdctof->GetYaxis()->SetTitleOffset(1.6);
    h_qdctof->GetXaxis()->SetRangeUser(28., 60.);

    gStyle->SetPaperSize(16, 16);
    TCanvas* c1 = new TCanvas("c1", "", 10, 10, 500, 500);
    TPostScript* ps1 = new TPostScript("plots/s438_run106_beta.eps", -113);
    ps1->Range(16, 16);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    gPad->SetGridx();
    gPad->SetGridy();
    gPad->SetLogy();
    h_beta->Draw();
    ps1->Close();
    
    gStyle->SetPaperSize(16, 16);
    TCanvas* c2 = new TCanvas("c2", "", 100, 100, 500, 500);
    TPostScript* ps2 = new TPostScript("plots/s438_run106_qdc.eps", -113);
    ps2->Range(16, 16);
    gPad->SetLeftMargin(0.19);
    gPad->SetRightMargin(0.05);
    h_qdc->Draw();
    h_qdc_cut->Draw("same");
    TLegend* leg1 = new TLegend(0.560, 0.757, 0.933, 0.884);
    Style(leg1);
    leg1->AddEntry(h_qdc, "all hits", "l");
    leg1->AddEntry(h_qdc_cut, "velocity cut", "l");
    leg1->Draw();
    ps2->Close();
    
    gStyle->SetPaperSize(16, 16);
    TCanvas* c3 = new TCanvas("c3", "", 200, 200, 500, 500);
    TPostScript* ps3 = new TPostScript("plots/s438_run106_qdcbar.eps", -113);
    ps3->Range(16, 16);
    gPad->SetLeftMargin(0.17);
    gPad->SetRightMargin(0.12);
    gPad->SetLogz();
    h_qdcbarid->Draw("colz");
    ps3->Close();

    gStyle->SetPaperSize(16, 16);
    TCanvas* c4 = new TCanvas("c4", "", 300, 300, 500, 500);
    TPostScript* ps4 = new TPostScript("plots/s438_run106_betabar.eps", -113);
    ps3->Range(16, 16);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.12);
    gPad->SetLogz();
    h_betabarid->Draw("colz");
    ps4->Close();

    gStyle->SetPaperSize(32, 16);
    TCanvas* c5 = new TCanvas("c5", "", 400, 10, 1000, 500);
    c5->Divide(2, 1);
    TPostScript* ps5 = new TPostScript("plots/s438_run106_yx.eps", -113);
    ps5->Range(32, 16);
    c5->cd(1);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.14);
    h_yx1->Draw("colz");
    TPaveLabel* l1 = new TPaveLabel(0.363, 0.911, 0.677, 0.96, "horizontal", "NDC");
    Style(l1);
    l1->Draw();
    c5->cd(2);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.14);
    h_yx2->Draw("colz");
    TPaveLabel* l2 = new TPaveLabel(0.383, 0.911, 0.657, 0.96, "vertical", "NDC");
    Style(l2);
    l2->Draw();
    c5->cd(0);
    ps5->Close();
    
    gStyle->SetPaperSize(16, 16);
    TCanvas* c6 = new TCanvas("c6", "", 500, 10, 500, 500);
    TPostScript* ps6 = new TPostScript("plots/s438_run106_qdctof.eps", -113);
    ps6->Range(16, 16);
    gPad->SetLeftMargin(0.17);
    gPad->SetRightMargin(0.12);
    gPad->SetLogz();
    h_qdctof->Draw("colz");
    ps6->Close();
}


void plot_s438_tcal()
{
    TPstyle();
    
    TFile* file = new TFile("/Volumes/Data/kresan/s438/data/run107_raw.root");
    
    TH1F* h_tcal_1 = (TH1F*) file->Get("hTcalData_201");
    Style(h_tcal_1, "TAC channel", "Counts");

    TH1F* h_time_1 = (TH1F*) file->Get("hTime_201");
    Style(h_time_1, "TAC channel", "Time (ns)");
    
    gStyle->SetPaperSize(32, 16);
    TCanvas* c1 = new TCanvas("c1", "", 10, 10, 1000, 500);
    c1->Divide(2, 1);
    TPostScript* ps1 = new TPostScript("plots/s438_run107_tcal.eps", -113);
    ps1->Range(32, 16);
    c1->cd(1);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    h_tcal_1->Draw();
    c1->cd(2);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    h_time_1->Draw();
    c1->cd(0);
    ps1->Close();
}


void plot_s438b_tdiff()
{
    TPstyle();
    
    TFile* file = new TFile("/Volumes/Data/kresan/s438b/cosmic/run331_cosmic.root");
    
    TH1F* h_tdiff_1 = (TH1F*) file->Get("h_tdiff_322");
    Style(h_tdiff_1, "t1 - t2 (ns)", "Counts");

    TH1F* h_tdiff = (TH1F*) file->Get("h_tdiff_res");
    Style(h_tdiff, "Bar ID", "Tdiff (ns)");
    TH1F* h_veff = (TH1F*) file->Get("h_veff_res");
    Style(h_veff, "Bar ID", "Veff (cm/ns)");
    
//    TFile* fileI = new TFile("igor.root");
//    TH1F* h_tdiff_I = (TH1F*) fileI->Get("h_tdiff_igor");
//    TH1F* h_veff_I = (TH1F*) fileI->Get("h_veff_igor");
    
//    h_tdiff->SetMarkerStyle(24);
//    h_veff->SetMarkerStyle(24);
    h_veff->GetYaxis()->SetRangeUser(0., 15.);
    
    gStyle->SetPaperSize(32, 16);
    TCanvas* c1 = new TCanvas("c1", "", 10, 10, 1000, 500);
    c1->Divide(2, 1);
    TPostScript* ps1 = new TPostScript("plots/s438b_run331_compare_tdiff.eps", -113);
    ps1->Range(32, 16);
    c1->cd(1);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    h_tdiff->Draw();
//    h_tdiff_I->Draw("same");
//    TLegend* leg1 = new TLegend(0.575, 0.262, 0.925, 0.374);
//    Style(leg1);
//    leg1->AddEntry(h_tdiff_I, "I. Gasparic", "l");
//    leg1->AddEntry(h_tdiff, "D. Kresan", "p");
//    leg1->Draw();
    c1->cd(2);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    h_veff->Draw();
//    h_veff_I->Draw("same");
//    leg1->Draw();
    c1->cd(0);
    ps1->Close();
    
    gStyle->SetPaperSize(16, 16);
    TCanvas* c2 = new TCanvas("c2", "", 100, 100, 500, 500);
    TPostScript* ps2 = new TPostScript("plots/s438_run107_tdiff.eps", -113);
    ps2->Range(16, 16);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    h_tdiff_1->Draw();
    ps2->Close();
}


Double_t wlk(Double_t* x, Double_t* par)
{
    Double_t y=0;
    
    Double_t par1=1500.; // +-0.2238
    Double_t par2=0.00075;//+-2.355e-05
    //y=par1*TMath::Power(x[0],par2)-(par1*TMath::Power(400.,par2)); // Michael's
    
    y=2.29083*log(x[0])-0.0870157*log(x[0])*log(x[0])-4.57824;  // mine
    
    return y;
    //return 0.;
}
void plot_walk()
{
    TF1 *f1 = new TF1("f1", wlk, 0., 1000., 1);
    f1->Draw();
}


void plot_s438b_cosmic()
{
    TPstyle();
    
    TFile *file = new TFile("/Volumes/Data/kresan/s438b/digi/run331_digi.root");
    TH2F *h1 = (TH2F*) file->Get("h_land_yx1");
    TH2F *h2 = (TH2F*) file->Get("h_land_yx2");
    TH2F *h_land_qdcbarid = (TH2F*) file->Get("h_land_qdcbarid");
    Style(h1, "X (cm)", "Y (cm)");
    Style(h2, "X (cm)", "Y (cm)");
    Style(h_land_qdcbarid, "Bar ID", "QDC channel");
    h_land_qdcbarid->GetYaxis()->SetTitleOffset(1.6);

    gStyle->SetPaperSize(32, 16);
    TCanvas* c1 = new TCanvas("c1", "", 10, 10, 1000, 500);
    c1->Divide(2, 1);
    TPostScript* ps1 = new TPostScript("plots/s438b_run331_yx.eps", -113);
    ps1->Range(32, 16);
    c1->cd(1);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.12);
    h1->Draw("colz");
    TPaveLabel* l2 = new TPaveLabel(0.363, 0.911, 0.677, 0.96, "horizontal", "NDC");
    Style(l2);
    l2->Draw();
    c1->cd(2);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.12);
    h2->Draw("colz");
    TPaveLabel* l1 = new TPaveLabel(0.383, 0.911, 0.657, 0.96, "vertical", "NDC");
    Style(l1);
    l1->Draw();
    c1->cd(0);
    ps1->Close();

    gStyle->SetPaperSize(16, 16);
    TCanvas* c2 = new TCanvas("c2", "", 400, 10, 500, 500);
    TPostScript* ps2 = new TPostScript("plots/s438b_run331_qdcbar.eps", -113);
    ps2->Range(16, 16);
    gPad->SetLeftMargin(0.17);
    gPad->SetRightMargin(0.12);
    gPad->SetLogz();
    h_land_qdcbarid->Draw("colz");
    ps2->Close();
}


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

