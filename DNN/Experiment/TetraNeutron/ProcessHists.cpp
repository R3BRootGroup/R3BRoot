// Include C++ headers:
#include <iostream>
#include <vector>

// Include ROOT headers
#include "TROOT.h"
#include "TString.h"
#include "TFile.h"
#include "TSystem.h"
#include "TMath.h"
#include "TH1D.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"

// Use standard namespace:
using namespace std;

// Include own headers:
#include "DrawHistDouma.h"

// Define the main function:
void ProcessHists()
{   
    // Define boolians:
    Bool_t PlotGausses = kFALSE;
    Bool_t PlotScoringPlus = kFALSE;
    
    // Define OutputPath:
    TString OutputPath = "/home/machinelearning/Desktop/Data/NeuLAND_30dp_600MeV/30dp_600MeV_BERT/DNN_Validation/";
    if (OutputPath.Contains("/DNN_Training/")==kTRUE) {OutputPath.ReplaceAll("/DNN_Training/","");}
    if (OutputPath.Contains("/DNN_Validation/")==kTRUE) {OutputPath.ReplaceAll("/DNN_Validation/","");}
    if (OutputPath.Contains("/DNN_Validation_BERT/")==kTRUE) {OutputPath.ReplaceAll("/DNN_Validation_BERT/","");}
    if (OutputPath.Contains("/DNN_Validation_INCLXX/")==kTRUE) {OutputPath.ReplaceAll("/DNN_Validation_INCLXX/","");}
    if (OutputPath.Contains("/DNN_Experiment/")==kTRUE) {OutputPath.ReplaceAll("/DNN_Experiment/","");}
    if (OutputPath.Contains("/DNN_Experiment_Signal/")==kTRUE) {OutputPath.ReplaceAll("/DNN_Experiment_Signal/","");}
    if (OutputPath.Contains("/DNN_Experiment_Background/")==kTRUE) {OutputPath.ReplaceAll("/DNN_Experiment_Background/","");}
    OutputPath = OutputPath + "/DNN_Validation/";
    
    // Define FilePaths:
    TString FileName = OutputPath + "/TetraNeutron_InvMass.root";
    TFile* TheFile = new TFile(FileName.Data(),"read");
    
    // Define the required histograms:
    std::vector<TString> HistNames;
    std::vector<TH1D*> TheHists;
    
    HistNames.push_back("NeutronTracks_DNNScoringPlus_MassHistogram");
    HistNames.push_back("NeutronTracks_ScoringPlus_Clusters_DNNMult_MassHistogram");
    HistNames.push_back("NeutronTracks_TradMed_Clusters_CutsMult_MassHistogram");
    HistNames.push_back("NeutronTracks_TradMed_Clusters_DNNMult_MassHistogram");
    HistNames.push_back("NeutronTracks_PerfectMethod_Signals_MassHistogram");
    
    // Retrieve the histograms:
    TheHists.resize(HistNames.size());
    
    for (Int_t k = 0; k<HistNames.size(); ++k)
    {
        TheHists.at(k) = (TH1D*) TheFile->Get(HistNames.at(k).Data());
    }
    
    // Fit Gaussians:
    TF1** FitGauss = new TF1*[TheHists.size()];
    TFitResultPtr* FitStatus = new TFitResultPtr[TheHists.size()];
    TString HistMethod = "";
    
    // Fit on each of the histograms:
    for (Int_t k = 0; k<TheHists.size(); ++k)
    {
        if (k==0) {HistMethod = "2xDNN method";}
        if (k==1) {HistMethod = "DNN mult & Scoring+ rec";}
        if (k==2) {HistMethod = "2xTDR (cuts) method";}
        if (k==3) {HistMethod = "DNN mult & TDR rec";}
        if (k==4) {HistMethod = "Perfect (MC) rec";}
        
        FitGauss[k] = new TF1("FitGauss","[0]*TMath::Exp(-0.5*((x-[1])/[2])*((x-[1])/[2]))",0.2,0.55);
        FitGauss[k]->SetParameter(0,TheHists.at(k)->GetMaximum());
        FitGauss[k]->SetParameter(1,0.45);
        FitGauss[k]->SetParameter(2,0.1);
        FitStatus[k] = TheHists.at(k)->Fit(FitGauss[k],"RS0Q");
        cout << "Histogram Nr. " << k << ": <" << HistMethod << "> Mean = " 
             << FitStatus[k]->Parameter(1) << " +/- " << FitStatus[k]->ParError(1) 
             << " [MeV] Chi^2 = " << FitStatus[k]->Chi2() << " & NDF = " << FitGauss[k]->GetNDF() << "\n";
        FitGauss[k]->SetNpx(100);
        FitGauss[k]->SetLineWidth(8.0);
    }
        
    // Next, make some plots:
    gROOT->SetBatch(kTRUE);
    TCanvas* c1 = DrawHistDouma(TheHists.at(4),"Tetraneutron Reconstruction Performance","Invariant Mass Difference [MeV]","Counts",1,kFALSE);
    Double_t Xmin = TheHists.at(4)->GetXaxis()->GetXmin();
    Double_t Xmax = TheHists.at(4)->GetXaxis()->GetXmax();
    Double_t Ymin = TheHists.at(4)->GetMinimum();
    Double_t Ymax = TheHists.at(4)->GetMaximum();
    if (PlotGausses==kTRUE) {FitGauss[4]->SetLineColor(kGray+2); FitGauss[4]->Draw("csame");}
    
    // Add other plots:
    TheHists.at(0)->SetLineColor(2);   // DNN result
    TheHists.at(0)->SetLineWidth(4.0);
    TheHists.at(0)->Draw("histsame");
    if (PlotGausses==kTRUE) {FitGauss[0]->SetLineColor(kRed+2); FitGauss[0]->Draw("csame");}
    
    TheHists.at(2)->SetLineColor(4);   // TDR result
    TheHists.at(2)->SetLineWidth(4.0);
    TheHists.at(2)->Draw("histsame");
    if (PlotGausses==kTRUE) {FitGauss[2]->SetLineColor(kBlue+2); FitGauss[2]->Draw("csame");}
    
    TheHists.at(3)->SetLineColor(kCyan); // TDR result with DNN mult.
    TheHists.at(3)->SetLineWidth(4.0);
    TheHists.at(3)->Draw("histsame");
    if (PlotGausses==kTRUE) {FitGauss[3]->SetLineColor(kCyan+2); FitGauss[3]->Draw("csame");}
    
    if (PlotScoringPlus==kTRUE)
    {
        TheHists.at(1)->SetLineColor(kMagenta); // Scoring+ result with DNN mult.
        TheHists.at(1)->SetLineWidth(4.0);
        TheHists.at(1)->Draw("histsame");
        if (PlotGausses==kTRUE) {FitGauss[1]->SetLineColor(kMagenta+2); FitGauss[1]->Draw("csame");}
    }
    
    TPaveText* text1 = new TPaveText(Xmin+(Xmax-Xmin)*0.75,Ymin+(Ymax-Ymin)*0.9,Xmin+(Xmax-Xmin)*0.76,Ymin+(Ymax-Ymin)*0.91,"NB");
    text1->SetFillColor(0); 
    text1->AddText("Perfect Reconstruction (MC)"); 
    text1->SetTextColor(1); 
    text1->SetTextSize(0.04); 
    text1->SetTextFont(1);
    text1->SetTextAngle(0.0); 
    text1->Draw("same");
    
    TPaveText* text2 = new TPaveText(Xmin+(Xmax-Xmin)*0.75,Ymin+(Ymax-Ymin)*0.85,Xmin+(Xmax-Xmin)*0.76,Ymin+(Ymax-Ymin)*0.86,"NB");
    text2->SetFillColor(0); 
    text2->AddText("2xDNN reconstruction"); 
    text2->SetTextColor(2); 
    text2->SetTextSize(0.04); 
    text2->SetTextFont(1);
    text2->SetTextAngle(0.0); 
    text2->Draw("same");
    
    TPaveText* text3 = new TPaveText(Xmin+(Xmax-Xmin)*0.75,Ymin+(Ymax-Ymin)*0.80,Xmin+(Xmax-Xmin)*0.76,Ymin+(Ymax-Ymin)*0.81,"NB");
    text3->SetFillColor(0); 
    text3->AddText("TDR reconstruction"); 
    text3->SetTextColor(4); 
    text3->SetTextSize(0.04); 
    text3->SetTextFont(1);
    text3->SetTextAngle(0.0); 
    text3->Draw("same");
    
    TPaveText* text4 = new TPaveText(Xmin+(Xmax-Xmin)*0.75,Ymin+(Ymax-Ymin)*0.75,Xmin+(Xmax-Xmin)*0.76,Ymin+(Ymax-Ymin)*0.76,"NB");
    text4->SetFillColor(0); 
    text4->AddText("TDR rec. with DNN mult."); 
    text4->SetTextColor(kCyan); 
    text4->SetTextSize(0.04); 
    text4->SetTextFont(1);
    text4->SetTextAngle(0.0); 
    text4->Draw("same");
    
    if (PlotScoringPlus==kTRUE)
    {
        TPaveText* text5 = new TPaveText(Xmin+(Xmax-Xmin)*0.75,Ymin+(Ymax-Ymin)*0.70,Xmin+(Xmax-Xmin)*0.76,Ymin+(Ymax-Ymin)*0.71,"NB");
        text5->SetFillColor(0); 
        text5->AddText("Scoring+ rec. with DNN mult."); 
        text5->SetTextColor(kMagenta); 
        text5->SetTextSize(0.04); 
        text5->SetTextFont(1);
        text5->SetTextAngle(0.0); 
        text5->Draw("same");
    }
    
    c1->SaveAs(OutputPath + "/TetraNeutron.png");
    c1->Close();
}

// Define the main for stand-alone compilation:
#ifndef __CINT__
#include <TApplication.h>
int main(int argc, char** argv)
{
  TApplication* TheApp = new TApplication("ProcessHists",&argc,argv);
  ProcessHists();
  // TheApp->Run();
  return 0;
}
#endif
