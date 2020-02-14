#include "DrawHistDouma2.h"

void Make4nRadiusHists()
{
    // NOTE: Makes a 4n Invariant Mass spectrum plot.
    TString Case = "30dp_600MeV";
    Bool_t PlotScoring = kFALSE;
    Bool_t PlotDDNMult = kFALSE;
    
    Double_t* ScaleDef = new Double_t[5];
    ScaleDef[0] = 0.829; ScaleDef[1] = 0.830; ScaleDef[2] = 0.823; ScaleDef[3] = 0.832; ScaleDef[4] = 0.822;
    
    // Collect histograms:
    TString ThePath = "/media/christiaan/DATA/Scenario_Summary/30dp_600MeV/2ndINCLXX/ScatterRadius/";
    
    TFile* f_10cm = new TFile(ThePath + "/10cm/" + "TetraNeutron_InvMass.root","read");
    TH1D*  h_10cm = (TH1D*) f_10cm->Get("NeutronTracks_PerfectMethod_Signals_MassHistogram");
    
    TFile* f_20cm = new TFile(ThePath + "/20cm/" + "TetraNeutron_InvMass.root","read");
    TH1D*  h_20cm = (TH1D*) f_20cm->Get("NeutronTracks_PerfectMethod_Signals_MassHistogram");
    
    TFile* f_30cm = new TFile(ThePath + "/30cm/" + "TetraNeutron_InvMass.root","read");
    TH1D*  h_30cm = (TH1D*) f_30cm->Get("NeutronTracks_PerfectMethod_Signals_MassHistogram");
    
    TFile* f_40cm = new TFile(ThePath + "/40cm/" + "TetraNeutron_InvMass.root","read");
    TH1D*  h_40cm = (TH1D*) f_40cm->Get("NeutronTracks_PerfectMethod_Signals_MassHistogram");
    
    TFile* f_50cm = new TFile(ThePath + "/50cm/" + "TetraNeutron_InvMass.root","read");
    TH1D*  h_50cm = (TH1D*) f_50cm->Get("NeutronTracks_PerfectMethod_Signals_MassHistogram");
    
    TFile* f_75cm = new TFile(ThePath + "/75cm/" + "TetraNeutron_InvMass.root","read");
    TH1D*  h_75cm = (TH1D*) f_75cm->Get("NeutronTracks_PerfectMethod_Signals_MassHistogram");
    
    TFile* f_100cm = new TFile(ThePath + "/100cm/" + "TetraNeutron_InvMass.root","read");
    TH1D*  h_100cm = (TH1D*) f_100cm->Get("NeutronTracks_PerfectMethod_Signals_MassHistogram");
    
    TFile* f_150cm = new TFile(ThePath + "/150cm/" + "TetraNeutron_InvMass.root","read");
    TH1D*  h_150cm = (TH1D*) f_150cm->Get("NeutronTracks_PerfectMethod_Signals_MassHistogram");
    
    TFile* f_200cm = new TFile(ThePath + "/200cm/" + "TetraNeutron_InvMass.root","read");
    TH1D*  h_200cm = (TH1D*) f_200cm->Get("NeutronTracks_PerfectMethod_Signals_MassHistogram");
    
    TFile* f_Infty = new TFile(ThePath + "/Infty/" + "TetraNeutron_InvMass.root","read");
    TH1D*  h_Infty = (TH1D*) f_Infty->Get("NeutronTracks_PerfectMethod_Signals_MassHistogram");
    
    // Collect Analysis histograms:
    TH1D*  h_DNN = (TH1D*) f_Infty->Get("NeutronTracks_DNNScoringPlus_MassHistogram");
    TH1D*  h_TDR = (TH1D*) f_Infty->Get("NeutronTracks_TradMed_Clusters_CutsMult_MassHistogram");
    TH1D*  h_Mult = (TH1D*) f_Infty->Get("NeutronTracks_TradMed_Clusters_DNNMult_MassHistogram");
    TH1D*  h_Scoring = (TH1D*) f_Infty->Get("NeutronTracks_ScoringPlus_Clusters_DNNMult_MassHistogram");
    
    // --------------------------------------------------------------------------------------
    
    // Make the proper picture:
    Double_t ScaleFactor = 4.0;
    gROOT->SetBatch(kTRUE);
    Double_t TheMax = h_Infty->GetMaximum();
    TCanvas* c1 = DrawHistDouma2(h_Infty,"4n Invariant Mass Spectrum","Invariant Mass Difference [MeV]","Counts",0,kFALSE,ScaleFactor,TheMax);
    
    // Paint all the different histograms:
    h_Infty->SetLineWidth(ScaleFactor*1.5);
    h_Infty->SetLineColor(1);
    h_Infty->Draw("histsame");
    
    h_200cm->SetLineWidth(ScaleFactor*0.8);
    h_200cm->SetLineColor(1);
    h_200cm->Draw("histsame");
    
    h_150cm->SetLineWidth(ScaleFactor*0.8);
    h_150cm->SetLineColor(1);
    h_150cm->Draw("histsame");
    
    h_100cm->SetLineWidth(ScaleFactor*0.8);
    h_100cm->SetLineColor(1);
    h_100cm->Draw("histsame");
    
    h_75cm->SetLineWidth(ScaleFactor*0.8);
    h_75cm->SetLineColor(1);
    h_75cm->Draw("histsame");
    
    h_50cm->SetLineWidth(ScaleFactor*0.8);
    h_50cm->SetLineColor(1);
    h_50cm->Draw("histsame");
    
    h_40cm->SetLineWidth(ScaleFactor*0.8);
    h_40cm->SetLineColor(1);
    h_40cm->Draw("histsame");
    
    h_30cm->SetLineWidth(ScaleFactor*0.8);
    h_30cm->SetLineColor(1);
    h_30cm->Draw("histsame");
    
    h_20cm->SetLineWidth(ScaleFactor*0.8);
    h_20cm->SetLineColor(1);
    h_20cm->Draw("histsame");
    
    h_10cm->SetLineWidth(ScaleFactor*0.8);
    h_10cm->SetLineColor(1);
    h_10cm->Draw("histsame");
    
    h_DNN->SetLineWidth(ScaleFactor*1.5);
    h_DNN->SetLineColor(2);
    h_DNN->Draw("histsame");
    
    h_TDR->SetLineWidth(ScaleFactor*1.5);
    h_TDR->SetLineColor(4);
    h_TDR->Draw("histsame");
    
    h_Mult->SetLineWidth(ScaleFactor*1.5);
    h_Mult->SetLineColor(kCyan+2);
    if (PlotDDNMult==kTRUE) {h_Mult->Draw("histsame");}
    
    h_Scoring->SetLineWidth(ScaleFactor*1.5);
    h_Scoring->SetLineColor(kMagenta);
    if (PlotScoring==kTRUE) {h_Scoring->Draw("histsame");}
    
    // Save and Close:
    c1->SaveAs("./30dp_600MeV_4nRadiusHists.png");
    c1->Close();    
}

