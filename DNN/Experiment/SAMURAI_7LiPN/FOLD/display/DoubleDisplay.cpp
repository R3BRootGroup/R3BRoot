// include required C++ headers:
#include <iostream>
#include <fstream>

// include required ROOT-headers:
#include "TFile.h"
#include "TSystem.h"
#include "TMath.h"
#include "TString.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TROOT.h"
#include "TMathText.h"

// include own headers:
#include "DrawFuncDouma.h"
#include "DrawFuncDoumaArray.h"

// Include TObject-headers:
#include "../../../../Objects/ObjDouble.h"

// use standard namespace:
using namespace std;

// Main function definition:
void DoubleDisplay(TString const FileName)
{
    // Suppress Pictures:
    gROOT->SetBatch(kTRUE);
    Double_t ScaleFactor = 2.1;
    
    // Load the ROOT-file:
    TString const FileName_Total = "./ROOT/" + FileName + ".root";
    FileStat_t stom;
    Int_t Test = gSystem->GetPathInfo(FileName_Total,stom);
    
    if (Test!=0)
    {
        cout << "### ERROR: File " + FileName_Total + " does not exist!\n\n";
    }
    else
    {
        TFile* f = new TFile(FileName_Total,"read");
        
        // Next, load the functions:
        TF1* NOT_Smeared = (TF1*) f->Get("DWBA_Function_0");
        TF1* Smeared = (TF1*) f->Get("DWBA_Function_Smeared");
        
        // Search for minima & maxima:
        Double_t Xstart,Xend;
        NOT_Smeared->GetRange(Xstart,Xend);
        Double_t Xstep = (Xend-Xstart)/200.0;
        Double_t Max = 0.0;
        Double_t Min = 1e99;
        
        for (Double_t Xev = Xstart; Xev<Xend; Xev = Xev + Xstep)
        {
            if (NOT_Smeared->Eval(Xev)>=Max) {Max = NOT_Smeared->Eval(Xev);}
            if (NOT_Smeared->Eval(Xev)<=Min) {Min = NOT_Smeared->Eval(Xev);}
        }
        
        NOT_Smeared->SetMinimum(Min);
        NOT_Smeared->SetMaximum(Max);
        
        // Then, make a nice picture:
        TCanvas* c1 = DrawFuncDouma(NOT_Smeared,"","Scattering Angle [deg]","Differential Cross Section [mb/sr]",2,200,ScaleFactor,kTRUE);
        Smeared->SetLineColor(4);
        Smeared->SetLineWidth(ScaleFactor);
        Smeared->SetNpx(200);
        Smeared->Draw("csame");
        
        // Next, make a legenda:
        Double_t Xmin,Xmax,Xlength,Ymin,Ymax,Ypos;
        NOT_Smeared->GetRange(Xmin,Xmax);
        Xlength = TMath::Abs(Xmax-Xmin);
        Ymin = NOT_Smeared->GetMinimum();
        Ymax = NOT_Smeared->GetMaximum();
        Ypos = (2.0*Ymax-Ymin/2.0)*0.90;
        cout << " Ymax = " << Ymax*2.0 << " | Ymin = " << Ymin/2.0 << "\n";
        
        TPaveText* text1 = new TPaveText(Xmin+Xlength*0.75,Ymin+Ypos,Xmin+Xlength*0.76,Ymin+Ypos+0.01,"NB");
        Ypos = Ypos*TMath::Power((Ymin/Ymax)/4.0,0.06);
        text1->SetFillColor(0); 
        text1->AddText("Unsmeared DWBA result"); 
        text1->SetTextColor(2);
        text1->SetTextSize(0.05); 
        text1->SetTextFont(1); 
        text1->SetTextAngle(0.0); 
        text1->Draw("same");
        
        TPaveText* text2 = new TPaveText(Xmin+Xlength*0.75,Ymin+Ypos,Xmin+Xlength*0.76,Ymin+Ypos+0.01,"NB");
        Ypos = Ypos*TMath::Power((Ymin/Ymax)/4.0,0.06);
        text2->SetFillColor(0); 
        text2->AddText("Smeared DWBA result"); 
        text2->SetTextColor(4);
        text2->SetTextSize(0.05); 
        text2->SetTextFont(1); 
        text2->SetTextAngle(0.0);
        text2->Draw("same");
        
        // Save and close:
        c1->SaveAs("./picture/" + FileName + ".png");
        c1->Close();
        f->Close();
    
        // Done.
    }
}

// Define the main for stand-alone compilation:
#ifndef __CINT__
#include <TApplication.h>
int main(int argc, char** argv)
{
    TString FileName = argv[1];
    TApplication* TheApp = new TApplication("DoubleDisplay",&argc,argv);
    DoubleDisplay(FileName);
    // TheApp->Run();
  return 0;
}
#endif
