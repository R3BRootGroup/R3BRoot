#include "DrawHistDouma.h"

void Make4nInvHist()
{
    // NOTE: Makes a 4n Invariant Mass spectrum plot.
    TString Case = "30dp_1000MeV";
    Bool_t PlotScoring = kFALSE;
    Bool_t PlotDDNMult = kFALSE;
    
    Double_t* ScaleDef = new Double_t[5];
    if (Case=="30dp_1000MeV") {ScaleDef[0] = 0.847; ScaleDef[1] = 0.844; ScaleDef[2] = 0.842; ScaleDef[3] = 0.845; ScaleDef[4] = 0.841;}
    if (Case=="30dp_600MeV")  {ScaleDef[0] = 0.829; ScaleDef[1] = 0.830; ScaleDef[2] = 0.823; ScaleDef[3] = 0.832; ScaleDef[4] = 0.822;}
    if (Case=="30dp_200MeV")  {ScaleDef[0] = 0.804; ScaleDef[1] = 0.803; ScaleDef[2] = 0.748; ScaleDef[3] = 0.803; ScaleDef[4] = 0.746;}
    if (Case=="16dp_1000MeV") {ScaleDef[0] = 0.587; ScaleDef[1] = 0.588; ScaleDef[2] = 0.580; ScaleDef[3] = 0.586; ScaleDef[4] = 0.583;}
    if (Case=="16dp_600MeV")  {ScaleDef[0] = 0.525; ScaleDef[1] = 0.529; ScaleDef[2] = 0.510; ScaleDef[3] = 0.529; ScaleDef[4] = 0.509;}
    if (Case=="16dp_200MeV")  {ScaleDef[0] = 0.461; ScaleDef[1] = 0.461; ScaleDef[2] = 0.389; ScaleDef[3] = 0.462; ScaleDef[4] = 0.389;}
    if (Case=="8dp_1000MeV")  {ScaleDef[0] = 0.199; ScaleDef[1] = 0.197; ScaleDef[2] = 0.198; ScaleDef[3] = 0.198; ScaleDef[4] = 0.199;}
    if (Case=="8dp_600MeV")   {ScaleDef[0] = 0.156; ScaleDef[1] = 0.157; ScaleDef[2] = 0.151; ScaleDef[3] = 0.158; ScaleDef[4] = 0.150;}
    if (Case=="8dp_200MeV")   {ScaleDef[0] = 0.117; ScaleDef[1] = 0.118; ScaleDef[2] = 0.086; ScaleDef[3] = 0.117; ScaleDef[4] = 0.087;}

    // Do perfect histograms:
    DNNDataHist* PerfectHist = new DNNDataHist();
    PerfectHist->SetPath("/ML/Scenario_Summary/"+Case+"/");
    PerfectHist->SetFileName("TetraNeutron_InvMass.root");
    PerfectHist->SetHistName("NeutronTracks_PerfectMethod_Signals_MassHistogram");
    PerfectHist->CollectPrimeHists();
    Double_t INCL_INCL_Scale     = 0.2*1e6*ScaleDef[0]/PerfectHist->GetPrimeHistContent("INCL_INCL");     cout << "### ATTENTION: INCL_INCL_Scale     = " << INCL_INCL_Scale << "\n";
    Double_t INCL_INCL_2nd_Scale = 0.2*1e6*ScaleDef[1]/PerfectHist->GetPrimeHistContent("INCL_INCL_2nd"); cout << "### ATTENTION: INCL_INCL_2nd_Scale = " << INCL_INCL_2nd_Scale << "\n";
    Double_t INCL_BERT_Scale     = 0.2*1e6*ScaleDef[2]/PerfectHist->GetPrimeHistContent("INCL_BERT");     cout << "### ATTENTION: INCL_BERT_Scale     = " << INCL_BERT_Scale << "\n";
    Double_t BERT_INCL_Scale     = 0.2*1e6*ScaleDef[3]/PerfectHist->GetPrimeHistContent("BERT_INCL");     cout << "### ATTENTION: BERT_INCL_Scale     = " << BERT_INCL_Scale << "\n";
    Double_t BERT_BERT_Scale     = 0.2*1e6*ScaleDef[4]/PerfectHist->GetPrimeHistContent("INCL_BERT");     cout << "### ATTENTION: BERT_BERT_Scale     = " << BERT_BERT_Scale << "\n";
    PerfectHist->Scale_INCL_INCL(INCL_INCL_Scale);
    PerfectHist->Scale_INCL_INCL_2nd(INCL_INCL_2nd_Scale);
    PerfectHist->Scale_INCL_BERT(INCL_BERT_Scale);
    PerfectHist->Scale_BERT_INCL(BERT_INCL_Scale);
    PerfectHist->Scale_BERT_BERT(BERT_BERT_Scale);
    PerfectHist->ComputeNewHists();
    PerfectHist->RemoveEndBins();
    Double_t PerfectMax = PerfectHist->GetDataHistMax();
    PerfectHist->SetPhysHeight(PerfectMax*1.35);
    
    // Do DNN histograms:
    DNNDataHist* DNNHist = new DNNDataHist();
    DNNHist->SetPath("/ML/Scenario_Summary/"+Case+"/");
    DNNHist->SetFileName("TetraNeutron_InvMass.root");
    DNNHist->SetHistName("NeutronTracks_DNNScoringPlus_MassHistogram");
    DNNHist->Scale_INCL_INCL(1.0);
    DNNHist->Scale_INCL_INCL_2nd(1.0);
    DNNHist->Scale_INCL_BERT(1.0);
    DNNHist->Scale_BERT_INCL(1.0);
    DNNHist->Scale_BERT_BERT(1.0);
    DNNHist->CollectPrimeHists();
    DNNHist->ComputeNewHists();
    DNNHist->RemoveEndBins();
    Double_t DNNMax = DNNHist->GetDataHistMax();
    DNNHist->SetPhysHeight(PerfectMax*1.17);
    
    // Do Scoring+ histograms:
    DNNDataHist* ScoringHist = new DNNDataHist();
    ScoringHist->SetPath("/ML/Scenario_Summary/"+Case+"/");
    ScoringHist->SetFileName("TetraNeutron_InvMass.root");
    ScoringHist->SetHistName("NeutronTracks_ScoringPlus_Clusters_DNNMult_MassHistogram");
    ScoringHist->Scale_INCL_INCL(1.0);
    ScoringHist->Scale_INCL_INCL_2nd(1.0);
    ScoringHist->Scale_INCL_BERT(1.0);
    ScoringHist->Scale_BERT_INCL(1.0);
    ScoringHist->Scale_BERT_BERT(1.0);
    ScoringHist->CollectPrimeHists();
    ScoringHist->ComputeNewHists();
    ScoringHist->RemoveEndBins();
    Double_t ScoringMax = ScoringHist->GetDataHistMax();
    ScoringHist->SetPhysHeight(PerfectMax*1.20);
    
    // Do DNN Mult & R-value Sort histograms:
    DNNDataHist* MultHist = new DNNDataHist();
    MultHist->SetPath("/ML/Scenario_Summary/"+Case+"/");
    MultHist->SetFileName("TetraNeutron_InvMass.root");
    MultHist->SetHistName("NeutronTracks_TradMed_Clusters_DNNMult_MassHistogram");
    MultHist->Scale_INCL_INCL(1.0);
    MultHist->Scale_INCL_INCL_2nd(1.0);
    MultHist->Scale_INCL_BERT(1.0);
    MultHist->Scale_BERT_INCL(1.0);
    MultHist->Scale_BERT_BERT(1.0);
    MultHist->CollectPrimeHists();
    MultHist->ComputeNewHists();
    MultHist->RemoveEndBins();
    Double_t MultMax = MultHist->GetDataHistMax();
    MultHist->SetPhysHeight(PerfectMax*1.12);
    
    // Do TDR histograms:
    DNNDataHist* TDRHist = new DNNDataHist();
    TDRHist->SetPath("/ML/Scenario_Summary/"+Case+"/");
    TDRHist->SetFileName("TetraNeutron_InvMass.root");
    TDRHist->SetHistName("NeutronTracks_TradMed_Clusters_CutsMult_MassHistogram");
    TDRHist->Scale_INCL_INCL(1.0);
    TDRHist->Scale_INCL_INCL_2nd(1.0);
    TDRHist->Scale_INCL_BERT(1.0);
    TDRHist->Scale_BERT_INCL(1.0);
    TDRHist->Scale_BERT_BERT(1.0);
    TDRHist->CollectPrimeHists();
    TDRHist->ComputeNewHists();
    TDRHist->RemoveEndBins();
    Double_t TDRMax = TDRHist->GetDataHistMax();
    TDRHist->SetPhysHeight(PerfectMax*1.05);
    
    // --------------------------------------------------------------------------------------
    
    // Make the proper picture:
    Double_t ScaleFactor = 4.0;
    gROOT->SetBatch(kTRUE);
    TCanvas* c1 = DrawHistDouma(PerfectHist->GetDataHist(),"4n Invariant Mass Spectrum","Invariant Mass Difference [MeV]","Counts",0,kFALSE,ScaleFactor,PerfectMax);
    
    // Add Perfect Histograms to the picture:
    PerfectHist->GetPhysHist()->SetFillColor(kGray+1);
    PerfectHist->GetPhysHist()->SetFillStyle(3000);
    PerfectHist->GetPhysHist()->Draw("E3same");
    
    PerfectHist->GetStatHist()->SetFillColor(kGray+1);
    PerfectHist->GetStatHist()->SetFillStyle(3000);
    PerfectHist->GetStatHist()->Draw("E3same");
    
    PerfectHist->GetDataHist()->SetLineColor(kBlack);
    PerfectHist->GetDataHist()->SetLineWidth(ScaleFactor);
    PerfectHist->GetDataHist()->Draw("Histsame");
    
    // Add DNN Histograms to the picture:
    DNNHist->GetPhysHist()->SetFillColor(kRed-9);
    DNNHist->GetPhysHist()->SetFillStyle(3000);
    DNNHist->GetPhysHist()->Draw("E3same");
    
    DNNHist->GetStatHist()->SetFillColor(kRed-9);
    DNNHist->GetStatHist()->SetFillStyle(3000);
    DNNHist->GetStatHist()->Draw("E3same");
    
    DNNHist->GetDataHist()->SetLineColor(kRed);
    DNNHist->GetDataHist()->SetLineWidth(ScaleFactor);
    DNNHist->GetDataHist()->Draw("Histsame");
    
    // Add Scoring+ Histograms to the picture:
    ScoringHist->GetPhysHist()->SetFillColor(kMagenta-9);
    ScoringHist->GetPhysHist()->SetFillStyle(3000);
    if (PlotScoring==kTRUE) {ScoringHist->GetPhysHist()->Draw("E3same");}
    
    ScoringHist->GetStatHist()->SetFillColor(kMagenta-9);
    ScoringHist->GetStatHist()->SetFillStyle(3000);
    if (PlotScoring==kTRUE) {ScoringHist->GetStatHist()->Draw("E3same");}
    
    ScoringHist->GetDataHist()->SetLineColor(kMagenta);
    ScoringHist->GetDataHist()->SetLineWidth(ScaleFactor);
    if (PlotScoring==kTRUE) {ScoringHist->GetDataHist()->Draw("Histsame");}
    
    // Add DNN Multiplicity & R-value sorting Histograms to the picture:
    MultHist->GetPhysHist()->SetFillColor(kCyan);
    MultHist->GetPhysHist()->SetFillStyle(3000);
    if (PlotDDNMult==kTRUE) {MultHist->GetPhysHist()->Draw("E3same");}
    
    MultHist->GetStatHist()->SetFillColor(kCyan);
    MultHist->GetStatHist()->SetFillStyle(3000);
    if (PlotDDNMult==kTRUE) {MultHist->GetStatHist()->Draw("E3same");}
    
    MultHist->GetDataHist()->SetLineColor(kCyan+2);
    MultHist->GetDataHist()->SetLineWidth(ScaleFactor);
    if (PlotDDNMult==kTRUE) {MultHist->GetDataHist()->Draw("Histsame");}
    
    // Add TDR Histograms to the picture:
    TDRHist->GetPhysHist()->SetFillColor(kBlue-9);
    TDRHist->GetPhysHist()->SetFillStyle(3000);
    TDRHist->GetPhysHist()->Draw("E3same");
    
    TDRHist->GetStatHist()->SetFillColor(kBlue-9);
    TDRHist->GetStatHist()->SetFillStyle(3000);
    TDRHist->GetStatHist()->Draw("E3same");
    
    TDRHist->GetDataHist()->SetLineColor(kBlue);
    TDRHist->GetDataHist()->SetLineWidth(ScaleFactor);
    TDRHist->GetDataHist()->Draw("Histsame");
    
    // Redraw histograms in proper order:
    PerfectHist->GetStatHist()->Draw("E3same");
    DNNHist->GetStatHist()->Draw("E3same");
    if (PlotScoring==kTRUE) {ScoringHist->GetStatHist()->Draw("E3same");}
    if (PlotDDNMult==kTRUE) {MultHist->GetStatHist()->Draw("E3same");}
    TDRHist->GetStatHist()->Draw("E3same");
    
    PerfectHist->GetDataHist()->Draw("Histsame");
    DNNHist->GetDataHist()->Draw("Histsame");
    if (PlotScoring==kTRUE) {ScoringHist->GetDataHist()->Draw("Histsame");}
    if (PlotDDNMult==kTRUE) {MultHist->GetDataHist()->Draw("Histsame");}
    TDRHist->GetDataHist()->Draw("Histsame");
    
    // --------------------------------------------------------------------------------------
    
    // Build a legenda:
    Double_t Xmin = PerfectHist->GetDataHist()->GetXaxis()->GetXmin();
    Double_t Xmax = PerfectHist->GetDataHist()->GetXaxis()->GetXmax();
    Double_t Xlength = Xmax - Xmin;
    
    Double_t Ymin = PerfectHist->GetDataHist()->GetMinimum();
    Double_t Ymax = PerfectHist->GetDataHist()->GetMaximum();
    Double_t Ylength = Ymax - Ymin;
    Double_t StartFrac = (PerfectMax-Ymin)/Ylength;
    
    Double_t StepValue = -0.1;
    
    TPaveText* text1 = new TPaveText(Xmin+Xlength*0.75,Ymin+Ylength*(StartFrac+StepValue-0.01),Xmin+Xlength*0.76,Ymin+Ylength*(StartFrac+StepValue),"NB");
    text1->SetFillColor(0); 
    text1->AddText("Tracking Algorithm"); 
    text1->SetTextColor(kBlack); 
    text1->SetTextSize(0.035); 
    text1->SetTextFont(1);
    text1->SetTextAngle(0.0); 
    text1->Draw("same");
    StepValue = StepValue - 0.05;
    
    TPaveText* text2 = new TPaveText(Xmin+Xlength*0.75,Ymin+Ylength*(StartFrac+StepValue-0.01),Xmin+Xlength*0.76,Ymin+Ylength*(StartFrac+StepValue),"NB");
    text2->SetFillColor(0); 
    text2->AddText("DNNScoring Method"); 
    text2->SetTextColor(kRed); 
    text2->SetTextSize(0.035); 
    text2->SetTextFont(1);
    text2->SetTextAngle(0.0); 
    text2->Draw("same");
    StepValue = StepValue - 0.05;
    
    if (PlotScoring==kTRUE)
    {
        TPaveText* text3 = new TPaveText(Xmin+Xlength*0.75,Ymin+Ylength*(StartFrac+StepValue-0.01),Xmin+Xlength*0.76,Ymin+Ylength*(StartFrac+StepValue),"NB");
        text3->SetFillColor(0); 
        text3->AddText("Scoring+ Method"); 
        text3->SetTextColor(kMagenta); 
        text3->SetTextSize(0.035); 
        text3->SetTextFont(1);
        text3->SetTextAngle(0.0); 
        text3->Draw("same");
        StepValue = StepValue - 0.05;
    }
    
    if (PlotDDNMult==kTRUE)
    {
        TPaveText* text4 = new TPaveText(Xmin+Xlength*0.75,Ymin+Ylength*(StartFrac+StepValue-0.01),Xmin+Xlength*0.76,Ymin+Ylength*(StartFrac+StepValue),"NB");
        text4->SetFillColor(0); 
        text4->AddText("R-sorting & DNN Mult."); 
        text4->SetTextColor(kCyan+2); 
        text4->SetTextSize(0.035); 
        text4->SetTextFont(1);
        text4->SetTextAngle(0.0); 
        text4->Draw("same");
        StepValue = StepValue - 0.05;
    }
    
    TPaveText* text5 = new TPaveText(Xmin+Xlength*0.75,Ymin+Ylength*(StartFrac+StepValue-0.01),Xmin+Xlength*0.76,Ymin+Ylength*(StartFrac+StepValue),"NB");
    text5->SetFillColor(0); 
    text5->AddText("R-sorting & TDR Mult."); 
    text5->SetTextColor(kBlue); 
    text5->SetTextSize(0.035); 
    text5->SetTextFont(1);
    text5->SetTextAngle(0.0); 
    text5->Draw("same");
    StepValue = StepValue - 0.05;
    
    // Save and Close:
    c1->SaveAs("./"+Case+"_4nHists.png");
    c1->Close();    
}

