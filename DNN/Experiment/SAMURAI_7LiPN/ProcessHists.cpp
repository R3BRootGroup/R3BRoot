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

// Use standard namespace:
using namespace std;

// Include own headers:
#include "HistSubtraction.h"
#include "DrawHistDouma.h"

// Define the main function:
void ProcessHists()
{   
    // Define Boolians:
    Bool_t IsSimulation = kFALSE;
    Bool_t Is110MeV = kTRUE;
    
    // Define OutputPath:
    TString OutputPath = "/media/christiaan/DATA/DNN_Data/SAMURAI_110MeV/SAMURAI_110MeV_INCLXX/";
    if (Is110MeV==kTRUE) {OutputPath.ReplaceAll("250MeV","110MeV");}
    if (OutputPath.Contains("/DNN_Training/")==kTRUE) {OutputPath.ReplaceAll("/DNN_Training/","");}
    if (OutputPath.Contains("/DNN_Validation/")==kTRUE) {OutputPath.ReplaceAll("/DNN_Validation/","");}
    if (OutputPath.Contains("/DNN_Experiment/")==kTRUE) {OutputPath.ReplaceAll("/DNN_Experiment/","");}
    if (OutputPath.Contains("/DNN_Experiment_Signal/")==kTRUE) {OutputPath.ReplaceAll("/DNN_Experiment_Signal/","");}
    if (OutputPath.Contains("/DNN_Experiment_Background/")==kTRUE) {OutputPath.ReplaceAll("/DNN_Experiment_Background/","");}
    
    // Define FilePaths:
    TString BackFileName = OutputPath + "/DNN_Experiment_Background/ExcitationEnergies.root";
    TString SignalFileName = OutputPath + "/DNN_Experiment_Signal/ExcitationEnergies.root";
    
    // Connect to files:
    if (IsSimulation==kTRUE) 
    {
        SignalFileName = OutputPath + "/DNN_Validation/ExcitationEnergies.root";
        BackFileName = OutputPath + "/DNN_DoesNotExist/ExcitationEnergies.root";
    }
    
    TFile* BackFile = new TFile(BackFileName.Data(),"read");
    TFile* SignalFile = new TFile(SignalFileName.Data(),"read");
    
    // Define the required histograms:
    std::vector<TString> OldHistNames;
    std::vector<TString> NewHistNames;
    std::vector<TH1D*> SignalHists;
    std::vector<TH1D*> BackHists;
    std::vector<TH1D*> NewHists;
    
    OldHistNames.push_back("NeutronTracks_SingleReconstruction_NeuLAND_Excitation_Energy_Total");          NewHistNames.push_back("SingleTOF_Total");               // 0
    OldHistNames.push_back("NeutronTracks_SingleReconstruction_NeuLAND_Excitation_Energy_MultClus_Total"); NewHistNames.push_back("SingleTOF_MultClus");            // 1
    OldHistNames.push_back("NeutronTracks_DNNScoringPlus_Excitation_Energy_MultClus_Total");               NewHistNames.push_back("2xDNN_MultClus");                // 2
    OldHistNames.push_back("NeutronTracks_DNNScoringPlus_SingleTOF_Excitation_Energy_MultClus_Total");     NewHistNames.push_back("MultDNN_SingleTOF_MultClus");    // 3
    OldHistNames.push_back("NeutronTracks_ScoringPlus_Clusters_DNNMult_Excitation_Energy_MultClus_Total"); NewHistNames.push_back("MultDNN_ScoringPlus_MultClus");  // 4
    OldHistNames.push_back("NeutronTracks_TradMed_Clusters_CutsMult_Excitation_Energy_MultClus_Total");    NewHistNames.push_back("2xTDR_MultClus");                // 5
    OldHistNames.push_back("NeutronTracks_TradMed_Clusters_DNNMult_Excitation_Energy_MultClus_Total");     NewHistNames.push_back("MultDNN_TDRStep2_MultClus");     // 6
    OldHistNames.push_back("NeutronTracks_PerfectMethod_Signals_Excitation_Energy_Total");                 NewHistNames.push_back("Perfect_Total");                 // 7
    OldHistNames.push_back("NeutronTracks_PerfectMethod_Signals_Excitation_Energy_MultClus_Total");        NewHistNames.push_back("Perfect_MultClus");              // 8
    
    // Retrieve the old histograms:
    SignalHists.resize(OldHistNames.size());
    BackHists.resize(OldHistNames.size());
    NewHists.resize(OldHistNames.size());
    
    for (Int_t k = 0; k<OldHistNames.size(); ++k)
    {
        SignalHists.at(k) = (TH1D*) SignalFile->Get(OldHistNames.at(k).Data());
        if (IsSimulation==kTRUE) {BackHists.at(k) = 0;}
        else {BackHists.at(k) = (TH1D*) BackFile->Get(OldHistNames.at(k).Data());}
    }
    
    // Define subtraction properties:
    Double_t BackScale;
    if (Is110MeV==kTRUE) {Double_t BackScale = 4461118.0/793763.0;}  // 110 MeV;    
    else {BackScale = 4371700.0/1344424.0;} // 250 MeV;
    Double_t LowerBound = -20.0;
    Double_t UpperBound = 100.0;
    if (Is110MeV==kTRUE) {UpperBound = 90.0;}
    
    // Do the subtraction:
    for (Int_t k = 0; k<OldHistNames.size(); ++k)
    {
        NewHists.at(k) = HistSubtraction(NewHistNames.at(k),SignalHists.at(k),BackHists.at(k),BackScale,LowerBound,UpperBound);
    }
    
    // Next, make some plots:
    gROOT->SetBatch(kTRUE);
    Double_t Xmin = LowerBound;
    Double_t Xmax = UpperBound;
    Double_t Ymin = 0.0;
    Double_t Ymax = 1.0;
    
    // ----------------------------------------------------------------------------------
    
    TCanvas* cSingle = DrawHistDouma(NewHists.at(0),"Min. TOF reconstruction <Baseline>","Excitation Energy [MeV]","Counts",1,kFALSE);
    NewHists.at(1)->SetLineColor(2);
    NewHists.at(1)->SetLineWidth(4.0);
    NewHists.at(1)->Draw("histsame");
    NewHists.at(7)->SetLineColor(3);
    NewHists.at(7)->SetLineWidth(4.0);
    NewHists.at(7)->Draw("histsame");
    Ymin = NewHists.at(0)->GetMinimum();
    Ymax = NewHists.at(0)->GetMaximum();
    
    TPaveText* text1 = new TPaveText(Xmin+(Xmax-Xmin)*0.75,Ymin+(Ymax-Ymin)*0.9,Xmin+(Xmax-Xmin)*0.76,Ymin+(Ymax-Ymin)*0.91,"NB");
    text1->SetFillColor(0); 
    text1->AddText("Mult=1 & minTOF Step2; all events"); 
    text1->SetTextColor(1); 
    text1->SetTextSize(0.04); 
    text1->SetTextFont(1);
    text1->SetTextAngle(0.0); 
    text1->Draw("same");
    
    TPaveText* text2 = new TPaveText(Xmin+(Xmax-Xmin)*0.75,Ymin+(Ymax-Ymin)*0.85,Xmin+(Xmax-Xmin)*0.76,Ymin+(Ymax-Ymin)*0.86,"NB");
    text2->SetFillColor(0); 
    text2->AddText("Mult=1 & minTOF Step2; nClusters>1"); 
    text2->SetTextColor(2); 
    text2->SetTextSize(0.04); 
    text2->SetTextFont(1);
    text2->SetTextAngle(0.0); 
    text2->Draw("same");
    
    TPaveText* text14 = new TPaveText(Xmin+(Xmax-Xmin)*0.75,Ymin+(Ymax-Ymin)*0.80,Xmin+(Xmax-Xmin)*0.76,Ymin+(Ymax-Ymin)*0.81,"NB");
    text14->SetFillColor(0); 
    text14->AddText("Mult=1 & Perfect Rec(Sim); all events"); 
    text14->SetTextColor(3); 
    text14->SetTextSize(0.04); 
    text14->SetTextFont(1);
    text14->SetTextAngle(0.0); 
    text14->Draw("same");
    
    cSingle->SaveAs(OutputPath + "/MinTOF.png");
    cSingle->Close();
    
    // ----------------------------------------------------------------------------------
    
    TCanvas* cFullComp = DrawHistDouma(NewHists.at(1),"Full DNN/TDR Comparison","Excitation Energy [MeV]","Counts",1,kFALSE);
    NewHists.at(2)->SetLineColor(2);
    NewHists.at(2)->SetLineWidth(4.0);
    NewHists.at(2)->Draw("histsame");
    NewHists.at(5)->SetLineColor(4);
    NewHists.at(5)->SetLineWidth(4.0);
    NewHists.at(5)->Draw("histsame");
    NewHists.at(8)->SetLineColor(3);
    NewHists.at(8)->SetLineWidth(4.0);
    NewHists.at(8)->Draw("histsame");
    Ymin = NewHists.at(1)->GetMinimum();
    Ymax = NewHists.at(1)->GetMaximum();
    
    TPaveText* text3 = new TPaveText(Xmin+(Xmax-Xmin)*0.75,Ymin+(Ymax-Ymin)*0.90,Xmin+(Xmax-Xmin)*0.76,Ymin+(Ymax-Ymin)*0.91,"NB");
    text3->SetFillColor(0); 
    text3->AddText("Mult=1 & minTOF Step2; nClusters>1"); 
    text3->SetTextColor(1); 
    text3->SetTextSize(0.04); 
    text3->SetTextFont(1);
    text3->SetTextAngle(0.0); 
    text3->Draw("same");
    
    TPaveText* text4 = new TPaveText(Xmin+(Xmax-Xmin)*0.75,Ymin+(Ymax-Ymin)*0.85,Xmin+(Xmax-Xmin)*0.76,Ymin+(Ymax-Ymin)*0.86,"NB");
    text4->SetFillColor(0); 
    text4->AddText("DNN Mult & DNN Step2; nClusters>1"); 
    text4->SetTextColor(2); 
    text4->SetTextSize(0.04); 
    text4->SetTextFont(1);
    text4->SetTextAngle(0.0); 
    text4->Draw("same");
    
    TPaveText* text5 = new TPaveText(Xmin+(Xmax-Xmin)*0.75,Ymin+(Ymax-Ymin)*0.80,Xmin+(Xmax-Xmin)*0.76,Ymin+(Ymax-Ymin)*0.81,"NB");
    text5->SetFillColor(0); 
    text5->AddText("TDR Mult & TDR Step2; nClusters>1"); 
    text5->SetTextColor(4); 
    text5->SetTextSize(0.04); 
    text5->SetTextFont(1);
    text5->SetTextAngle(0.0); 
    text5->Draw("same");
    
    TPaveText* text15 = new TPaveText(Xmin+(Xmax-Xmin)*0.75,Ymin+(Ymax-Ymin)*0.75,Xmin+(Xmax-Xmin)*0.76,Ymin+(Ymax-Ymin)*0.76,"NB");
    text15->SetFillColor(0); 
    text15->AddText("Mult=1 & Perfect Rec(Sim); nClusters>1"); 
    text15->SetTextColor(3); 
    text15->SetTextSize(0.04); 
    text15->SetTextFont(1);
    text15->SetTextAngle(0.0); 
    text15->Draw("same");
    
    cFullComp->SaveAs(OutputPath + "/FullDNNComp.png");
    cFullComp->Close();
    
    // ----------------------------------------------------------------------------------
    
    TCanvas* cMultComp = DrawHistDouma(NewHists.at(1),"Multiplicity DNN/TDR Comparison","Excitation Energy [MeV]","Counts",1,kFALSE);
    NewHists.at(6)->SetLineColor(2);
    NewHists.at(6)->SetLineWidth(4.0);
    NewHists.at(6)->Draw("histsame");
    NewHists.at(5)->SetLineColor(4);
    NewHists.at(5)->SetLineWidth(4.0);
    NewHists.at(5)->Draw("histsame");
    NewHists.at(8)->SetLineColor(3);
    NewHists.at(8)->SetLineWidth(4.0);
    NewHists.at(8)->Draw("histsame");
    Ymin = NewHists.at(1)->GetMinimum();
    Ymax = NewHists.at(1)->GetMaximum();
    
    TPaveText* text6 = new TPaveText(Xmin+(Xmax-Xmin)*0.75,Ymin+(Ymax-Ymin)*0.90,Xmin+(Xmax-Xmin)*0.76,Ymin+(Ymax-Ymin)*0.91,"NB");
    text6->SetFillColor(0); 
    text6->AddText("Mult=1 & minTOF Step2; nClusters>1"); 
    text6->SetTextColor(1); 
    text6->SetTextSize(0.04); 
    text6->SetTextFont(1);
    text6->SetTextAngle(0.0); 
    text6->Draw("same");
    
    TPaveText* text7 = new TPaveText(Xmin+(Xmax-Xmin)*0.75,Ymin+(Ymax-Ymin)*0.85,Xmin+(Xmax-Xmin)*0.76,Ymin+(Ymax-Ymin)*0.86,"NB");
    text7->SetFillColor(0); 
    text7->AddText("DNN Mult & TDR Step2; nClusters>1"); 
    text7->SetTextColor(2); 
    text7->SetTextSize(0.04); 
    text7->SetTextFont(1);
    text7->SetTextAngle(0.0); 
    text7->Draw("same");
    
    TPaveText* text8 = new TPaveText(Xmin+(Xmax-Xmin)*0.75,Ymin+(Ymax-Ymin)*0.80,Xmin+(Xmax-Xmin)*0.76,Ymin+(Ymax-Ymin)*0.81,"NB");
    text8->SetFillColor(0); 
    text8->AddText("TDR Mult & TDR Step2; nClusters>1"); 
    text8->SetTextColor(4); 
    text8->SetTextSize(0.04); 
    text8->SetTextFont(1);
    text8->SetTextAngle(0.0); 
    text8->Draw("same");
    
    TPaveText* text16 = new TPaveText(Xmin+(Xmax-Xmin)*0.75,Ymin+(Ymax-Ymin)*0.75,Xmin+(Xmax-Xmin)*0.76,Ymin+(Ymax-Ymin)*0.76,"NB");
    text16->SetFillColor(0); 
    text16->AddText("Mult=1 & Perfect Rec(Sim); nClusters>1"); 
    text16->SetTextColor(3); 
    text16->SetTextSize(0.04); 
    text16->SetTextFont(1);
    text16->SetTextAngle(0.0); 
    text16->Draw("same");
    
    cMultComp->SaveAs(OutputPath + "/MultDNNComp.png");
    cMultComp->Close();
    
    // ----------------------------------------------------------------------------------
    
    TCanvas* cStep2Comp = DrawHistDouma(NewHists.at(3),"Step2 DNN/TDR Comparison","Excitation Energy [MeV]","Counts",1,kFALSE);
    NewHists.at(3)->SetLineColor(1);
    NewHists.at(3)->SetLineWidth(4.0);
    NewHists.at(3)->Draw("histsame");
    NewHists.at(2)->SetLineColor(2);
    NewHists.at(2)->SetLineWidth(4.0);
    NewHists.at(2)->Draw("histsame");
    NewHists.at(6)->SetLineColor(4);
    NewHists.at(6)->SetLineWidth(4.0);
    NewHists.at(6)->Draw("histsame");
    Ymin = NewHists.at(3)->GetMinimum();
    Ymax = NewHists.at(3)->GetMaximum();
    
    TPaveText* text9 = new TPaveText(Xmin+(Xmax-Xmin)*0.75,Ymin+(Ymax-Ymin)*0.90,Xmin+(Xmax-Xmin)*0.76,Ymin+(Ymax-Ymin)*0.91,"NB");
    text9->SetFillColor(0); 
    text9->AddText("DNN Mult & minTOF Step2; nClusters>1"); 
    text9->SetTextColor(1); 
    text9->SetTextSize(0.04); 
    text9->SetTextFont(1);
    text9->SetTextAngle(0.0); 
    text9->Draw("same");
    
    TPaveText* text10 = new TPaveText(Xmin+(Xmax-Xmin)*0.75,Ymin+(Ymax-Ymin)*0.85,Xmin+(Xmax-Xmin)*0.76,Ymin+(Ymax-Ymin)*0.86,"NB");
    text10->SetFillColor(0); 
    text10->AddText("DNN Mult & DNN Step2; nClusters>1"); 
    text10->SetTextColor(2); 
    text10->SetTextSize(0.04); 
    text10->SetTextFont(1);
    text10->SetTextAngle(0.0); 
    text10->Draw("same");
    
    TPaveText* text11 = new TPaveText(Xmin+(Xmax-Xmin)*0.75,Ymin+(Ymax-Ymin)*0.80,Xmin+(Xmax-Xmin)*0.76,Ymin+(Ymax-Ymin)*0.81,"NB");
    text11->SetFillColor(0); 
    text11->AddText("DNN Mult & TDR Step2; nClusters>1"); 
    text11->SetTextColor(4); 
    text11->SetTextSize(0.04); 
    text11->SetTextFont(1);
    text11->SetTextAngle(0.0); 
    text11->Draw("same");
    
    cStep2Comp->SaveAs(OutputPath + "/Step2DNNComp.png");
    cStep2Comp->Close();
    
    // ----------------------------------------------------------------------------------
    
    TCanvas* cComp = DrawHistDouma(NewHists.at(2),"DNN/TDR Comparison","Excitation Energy [MeV]","Counts",2,kFALSE);
    NewHists.at(5)->SetLineColor(4);
    NewHists.at(5)->SetLineWidth(4.0);
    NewHists.at(5)->Draw("histsame");
    Ymin = NewHists.at(2)->GetMinimum();
    Ymax = NewHists.at(2)->GetMaximum();
    
    TPaveText* text12 = new TPaveText(Xmin+(Xmax-Xmin)*0.75,Ymin+(Ymax-Ymin)*0.85,Xmin+(Xmax-Xmin)*0.76,Ymin+(Ymax-Ymin)*0.86,"NB");
    text12->SetFillColor(0); 
    text12->AddText("DNN Mult & DNN Step2; nClusters>1"); 
    text12->SetTextColor(2); 
    text12->SetTextSize(0.04); 
    text12->SetTextFont(1);
    text12->SetTextAngle(0.0); 
    text12->Draw("same");
    
    TPaveText* text13 = new TPaveText(Xmin+(Xmax-Xmin)*0.75,Ymin+(Ymax-Ymin)*0.80,Xmin+(Xmax-Xmin)*0.76,Ymin+(Ymax-Ymin)*0.81,"NB");
    text13->SetFillColor(0); 
    text13->AddText("TDR Mult & TDR Step2; nClusters>1"); 
    text13->SetTextColor(4); 
    text13->SetTextSize(0.04); 
    text13->SetTextFont(1);
    text13->SetTextAngle(0.0); 
    text13->Draw("same");
    
    cComp->SaveAs(OutputPath + "/DNNComp.png");
    cComp->Close();
    
    // ----------------------------------------------------------------------------------

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
