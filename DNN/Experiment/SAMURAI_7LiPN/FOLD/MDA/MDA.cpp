// include required C++ headers:
#include <iostream>
#include <fstream>

// include required ROOT-headers:
#include "TFile.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TMath.h"
#include "TString.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TH1D.h"
#include "TGraphErrors.h"
#include "TPaveText.h"
#include "TFitResultPtr.h"
#include "TFitResult.h"
#include "TLorentzVector.h"

// Declaration of required global variables:
TF1* DWBA_IAS_Function;
Bool_t Is250 = kTRUE;
TString ExpCrossFile = "/media/christiaan/DATA/DNN_Data/SAMURAI_250MeV_INCLXX/TheCrossSections_250MeV_Experiment.root";
TString MethodString = "NeutronTracks_TradMed_Clusters_CutsMult";
// TString MethodString = "NeutronTracks_DNNScoringPlus";
// TString MethodString = "NeutronTracks_SingleReconstruction_NeuLAND";

// include own headers:
#include "DrawGraphDouma.h"
#include "RoundOff.h"

// use standard namespace:
using namespace std;

// Include TObject-headers:
#include "../../../../Objects/ObjDouble.h"

// Definition of the fitting function:
Double_t FitFunction(Double_t* x, Double_t* par)
{
    // Initialize the answer:
    Double_t Answer = 0.0;
    
    // Add the terms:
    Answer = Answer + par[0]*(DWBA_IAS_Function->Eval(x[0]));
    
    // Return the total:
    return Answer;
}

// Definition of main function:
void MDA()
{
    // Suppress pictures coming up:
    gROOT->SetBatch(kTRUE);
    
    // Begin by obtaining the DWBA function:
    TString DWBA_Func_FileName = "../display/ROOT/Li7_IAS_250.root";
    if (Is250==kFALSE) {DWBA_Func_FileName = "../display/ROOT/Li7_IAS_110.root";}
    TFile* DWBA_Func_File = new TFile(DWBA_Func_FileName.Data(),"read");
    DWBA_IAS_Function = (TF1*) DWBA_Func_File->Get("DWBA_Function_0");
    
    // Next, extract the experimental data:
    TFile* ExpFile = new TFile(ExpCrossFile.Data(),"read");
    TString CrossString = MethodString + "_CrossSection";
    TGraphErrors* Data = (TGraphErrors*) ExpFile->Get(CrossString.Data());
    
    TString BandString = MethodString + "_SystPlots";
    TH1D* PlotBand = (TH1D*) ExpFile->Get(BandString.Data());
    
    TString ErrorString = MethodString + "_SystErrors";
    TH1D* ErrorBand = (TH1D*) ExpFile->Get(ErrorString.Data());
    
    // Extract both of the zero-points:
    Double_t Normal_ZeroPoint = 1.0;
    Double_t Q0_ZeroPoint = 1.0;
    TString DWBA_q0Func_FileName = DWBA_Func_FileName.ReplaceAll(".root","_q0.root");
    TFile* DWBA_q0Func_File = new TFile(DWBA_q0Func_FileName.Data(),"read");
    ObjDouble* ZeroPoint = new ObjDouble("Theoretical_ZeroPoint");
    ObjDouble* ZeroPoint_q0 = new ObjDouble("Theoretical_ZeroPoint");
    ZeroPoint->ReadFromFile(DWBA_Func_File);
    ZeroPoint_q0->ReadFromFile(DWBA_q0Func_File);
    Normal_ZeroPoint = ZeroPoint->GetNumber();
    Q0_ZeroPoint = ZeroPoint_q0->GetNumber();
    Double_t Extrapolation_Ratio = Q0_ZeroPoint/Normal_ZeroPoint;
    
    // Initialize the fitting function:
    Double_t DataMax = Data->GetMaximum();
    Double_t par0_Guess = DataMax/Normal_ZeroPoint;
    TF1* FitFunc = new TF1("FitFunc",FitFunction,0.0,6.0,1);
    FitFunc->SetParameter(0,par0_Guess);
    
    // Also, make a graph when the errors are replaced with the 
    // physics list errors:
    Int_t nPoints = ErrorBand->GetNbinsX();
    Double_t* Xvalue = new Double_t[nPoints];
    Double_t* Xerror = new Double_t[nPoints];
    Double_t* Yvalue = new Double_t[nPoints];
    Double_t* Yerror = new Double_t[nPoints];
    Double_t ThisX;
    Double_t ThisY;
    
    for (Int_t k = 0; k<nPoints; ++k)
    {
        Data->GetPoint(k,ThisX,ThisY);
        Xvalue[k] = ThisX;
        Yvalue[k] = ThisY;
        Xerror[k] = Data->GetErrorX(k);
        Yerror[k] = ErrorBand->GetBinError(k+1);
    }
    
    TGraphErrors* Data_PhysList = new TGraphErrors(nPoints,&Xvalue[0],&Yvalue[0],&Xerror[0],&Yerror[0]);
    
    // Make the fit:
    TFitResultPtr FitStatus = Data->Fit(FitFunc,"RSBQ0");
    
    // Obtain the q0 cross section:
    FitFunc->SetParameter(0,FitStatus->Parameter(0));
    Double_t CrossSection_q0 = FitFunc->Eval(0.0);
    Double_t CrossSection_q0_Error = FitFunc->Eval(0.0)*FitStatus->ParError(0)/FitStatus->Parameter(0);
    CrossSection_q0 = CrossSection_q0*Extrapolation_Ratio;
    CrossSection_q0_Error = CrossSection_q0_Error*CrossSection_q0_Error*Extrapolation_Ratio*Extrapolation_Ratio + CrossSection_q0*CrossSection_q0*Extrapolation_Ratio*Extrapolation_Ratio*1e-4;
    CrossSection_q0_Error = TMath::Sqrt(CrossSection_q0_Error);
    
    // Same for Physics List errors:
    TFitResultPtr FitStatus_PList = Data_PhysList->Fit(FitFunc,"RSBQ0");
    
    // Obtain the q0 cross section:
    FitFunc->SetParameter(0,FitStatus_PList->Parameter(0));
    Double_t CrossSection_q0_PList = FitFunc->Eval(0.0);
    Double_t CrossSection_q0_Error_PList = FitFunc->Eval(0.0)*FitStatus_PList->ParError(0)/FitStatus_PList->Parameter(0);
    CrossSection_q0_PList = CrossSection_q0_PList*Extrapolation_Ratio;
    CrossSection_q0_Error_PList = CrossSection_q0_Error_PList*CrossSection_q0_Error_PList*Extrapolation_Ratio*Extrapolation_Ratio + CrossSection_q0_PList*CrossSection_q0_PList*Extrapolation_Ratio*Extrapolation_Ratio*1e-4;
    CrossSection_q0_Error_PList = TMath::Sqrt(CrossSection_q0_Error_PList);
    
    // Add the difference with the previous fit:
    CrossSection_q0_Error_PList = CrossSection_q0_Error_PList + TMath::Abs(CrossSection_q0_PList - CrossSection_q0);
    
    // Make a picture:
    TString MethodTag = "";
    if (MethodString=="NeutronTracks_TradMed_Clusters_CutsMult") {MethodTag = "TDR";}
    if (MethodString=="NeutronTracks_DNNScoringPlus") {MethodTag = "DNN";}
    if (MethodString=="NeutronTracks_SingleReconstruction_NeuLAND") {MethodTag = "MinTOF";}
    
    TCanvas* c1 = DrawGraphDouma(Data,"7Li(p,n)7Be(gs) Exp. Cross Section "+MethodTag,"Neutron CM Scattering angle [deg]","Differential Cross Section [Counts/sr]",1,4.0,kTRUE,"AP");
    FitFunc->SetParameter(0,FitStatus->Parameter(0));
    FitFunc->SetNpx(600);
    FitFunc->SetLineWidth(10.0);
    FitFunc->SetLineColor(4);
    FitFunc->Draw("same");
    
    PlotBand->SetLineColor(kGray+2);
    PlotBand->SetFillColor(kGray+2);
    PlotBand->Draw("E3same");
    
    // Show output:
    Double_t chi2 = FitStatus->Chi2();
    Int_t NDF = FitFunc->GetNDF();
    Double_t redchi2 = chi2/((Int_t) NDF);
    Double_t FitProb = TMath::Prob(chi2,NDF);
    
    cout << "\n\nRESULTS:\n\n"
         << "Reduced chi^2              = " << redchi2 << "\n"
         << "NDF                        = " << NDF << "\n"
         << "Fit Probability            = " << FitProb << "\n"
         << "Extrapolation Ratio        = " << Extrapolation_Ratio << "\n"
         << "(q=0) Diff Cross Section   = " << CrossSection_q0 << " [Counts/sr]\n"
         << "(q=0) Diff Cross Sec. Err. = " << CrossSection_q0_Error << " [Counts/sr]\n"
         << "(q=0) Diff Cross Sec. Err. = " << 100.0*CrossSection_q0_Error/CrossSection_q0 << " [percent]\n"
         << "(q=0) Cross Sec. Syst Err. = " << 100.0*CrossSection_q0_Error_PList/CrossSection_q0 << " [percent]\n"
         << "\n\n";
         
    // Build legenda:
    Double_t Xmin = 0.0;     
    Double_t Xmax = 5.0;
    Double_t Ymin = 0.3;
    Double_t Ymax = Data->GetMaximum();
    Double_t Xlength = Xmax-Xmin;
    Double_t Ylength = Ymax-Ymin;
    TString st = "";
         
    TPaveText* text1 = new TPaveText(Xmin+Xlength*0.80,Ymin+Ylength*0.99,Xmin+Xlength*0.81,Ymin+Ylength*1.00,"NB");
    text1->SetFillColor(0); 
    text1->SetTextColor(4); 
    text1->SetTextSize(0.035); 
    text1->SetTextFont(1); 
    text1->SetTextAngle(0.0);
    text1->AddText("Red. chi^2 = "+RoundOff(redchi2,4));
    text1->Draw("same");
    Ylength = Ylength*0.91;
    
    TPaveText* text2 = new TPaveText(Xmin+Xlength*0.80,Ymin+Ylength*0.99,Xmin+Xlength*0.81,Ymin+Ylength*1.00,"NB");
    text2->SetFillColor(0); 
    text2->SetTextColor(4); 
    text2->SetTextSize(0.035); 
    text2->SetTextFont(1); 
    text2->SetTextAngle(0.0);
    text2->AddText("NDF = " + st.Itoa(NDF,10));
    text2->Draw("same");
    Ylength = Ylength*0.91;
    
    TPaveText* text3 = new TPaveText(Xmin+Xlength*0.80,Ymin+Ylength*0.99,Xmin+Xlength*0.81,Ymin+Ylength*1.00,"NB");
    text3->SetFillColor(0); 
    text3->SetTextColor(4); 
    text3->SetTextSize(0.035); 
    text3->SetTextFont(1); 
    text3->SetTextAngle(0.0);
    text3->AddText("Fit Prob. = "+RoundOff(FitProb,4));
    text3->Draw("same");
    Ylength = Ylength*0.91;
    
    TPaveText* text4 = new TPaveText(Xmin+Xlength*0.80,Ymin+Ylength*0.99,Xmin+Xlength*0.81,Ymin+Ylength*1.00,"NB");
    text4->SetFillColor(0); 
    text4->SetTextColor(4); 
    text4->SetTextSize(0.035); 
    text4->SetTextFont(1); 
    text4->SetTextAngle(0.0);
    text4->AddText("Extpol. ratio = "+RoundOff(Extrapolation_Ratio,4));
    text4->Draw("same");
    Ylength = Ylength*0.91;
    
    TPaveText* text5 = new TPaveText(Xmin+Xlength*0.80,Ymin+Ylength*0.99,Xmin+Xlength*0.81,Ymin+Ylength*1.00,"NB");
    text5->SetFillColor(0); 
    text5->SetTextColor(4); 
    text5->SetTextSize(0.035); 
    text5->SetTextFont(1); 
    text5->SetTextAngle(0.0);
    text5->AddText("(q=0) Cross Section [#/sr] = "+RoundOff(CrossSection_q0,4));
    text5->Draw("same");
    Ylength = Ylength*0.91;
    
    TPaveText* text6 = new TPaveText(Xmin+Xlength*0.80,Ymin+Ylength*0.99,Xmin+Xlength*0.81,Ymin+Ylength*1.00,"NB");
    text6->SetFillColor(0); 
    text6->SetTextColor(4); 
    text6->SetTextSize(0.035); 
    text6->SetTextFont(1); 
    text6->SetTextAngle(0.0);
    text6->AddText("(q=0) Cross Sect. Stat. Err. [%] = "+RoundOff(100.0*CrossSection_q0_Error/CrossSection_q0,4));
    text6->Draw("same");
    Ylength = Ylength*0.91;
    
    TPaveText* text7 = new TPaveText(Xmin+Xlength*0.80,Ymin+Ylength*0.99,Xmin+Xlength*0.81,Ymin+Ylength*1.00,"NB");
    text7->SetFillColor(0); 
    text7->SetTextColor(4); 
    text7->SetTextSize(0.035); 
    text7->SetTextFont(1); 
    text7->SetTextAngle(0.0);
    text7->AddText("(q=0) Cross Sect. Syst. Err. [%] = "+RoundOff(100.0*CrossSection_q0_Error_PList/CrossSection_q0,4));
    text7->Draw("same");
    Ylength = Ylength*0.91;
    
    // Save & Close:
    c1->SaveAs("./ExpCrossSection_" + MethodTag + ".png");
    c1->Close();
}

// Define the main for stand-alone compilation:
#ifndef __CINT__
#include <TApplication.h>
int main(int argc, char** argv)
{
    TApplication* TheApp = new TApplication("MDA",&argc,argv);
    MDA();
    // TheApp->Run();
    
    // return:
    return 0;
}
#endif
