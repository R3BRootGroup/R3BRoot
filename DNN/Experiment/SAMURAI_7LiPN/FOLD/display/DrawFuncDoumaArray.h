// Include the required root-headers:
#include "TMath.h"
#include "TH1D.h"
#include "TH1I.h"
#include "TStyle.h"
#include "TPad.h"
#include "TPaveText.h"
#include "TCanvas.h"
#include "TString.h"
#include "TFile.h"
#include "TSystem.h"
#include "TF1.h"

TCanvas* DrawFuncDoumaArray(Int_t const nFunc, TF1* const Raw[], TString const Legenda[], TString const Title, TString const xlabel, TString const ylabel, Int_t const Npx, Double_t const ScaleFactor, Bool_t const UseLogScale)
{
  // This program paints multiple the TF1* Raw in the same canvas.
  // This program controls every aspect of the plot.
  // Be sure that all parameters are initialized
  // before calling this function.
  
  // Written by C. A. Douma.
    
  // ======================================================================================
  // We begin with specifying some settings of the TF1:

  // We add labels to the TF1:
  for (Int_t nf = 0; nf<nFunc; ++nf)
  {
    Raw[nf]->SetTitle(Title);
    Raw[nf]->GetXaxis()->SetTitle(xlabel);
    Raw[nf]->GetYaxis()->SetTitle(ylabel);
  
    // Control position of axis labels along the axis:
    Raw[nf]->GetXaxis()->CenterTitle(kTRUE);
    Raw[nf]->GetYaxis()->CenterTitle(kTRUE);
    // kFALSE will put the labels at the end of the axis.
  
    // Set ranges of the TGraph. Begin with setting the Npx:
    Raw[nf]->SetNpx(Npx);
  }

  // Get X-range.
  Double_t Xmin, Xmax, ThisXmin, ThisXmax;
  Raw[0]->GetRange(Xmin,Xmax);
  
  for (Int_t nf = 0; nf<nFunc; ++nf)
  {
      Raw[0]->GetRange(ThisXmin,ThisXmax);
      
      if (ThisXmin<Xmin) {Xmin = ThisXmin;}
      if (ThisXmax>Xmax) {Xmax = ThisXmax;}
  }
  
  Raw[0]->GetRange(ThisXmin,ThisXmax);
  
  // Get Y-range:
  Double_t Xval, Yval;
  Double_t Ymin, Ymax;
  Double_t YminL, YmaxL;
  Ymin = Raw[0]->Eval(ThisXmin);
  Ymax = Raw[0]->Eval(ThisXmax);
  YminL = Raw[0]->Eval(ThisXmin);
  YmaxL = Raw[0]->Eval(ThisXmax);
  Bool_t Positive = (YminL>0.0);
  
  for (Int_t nf = 0; nf<nFunc; ++nf)
  {
      for (Int_t k = 0; k<Npx; ++k)
      {
          Xval = ((Int_t) k);
          Xval = Xval/((Int_t) Npx);
          Xval = Xval*(Xmax-Xmin);
          Xval = Xmin + Xval;
          
          Raw[nf]->GetRange(ThisXmin,ThisXmax);
          
          if ((Xval>ThisXmin)&&(Xval<ThisXmax))
          {
              Yval = Raw[nf]->Eval(Xval);
      
              // linear min & max:
              if (Yval>Ymax) {Ymax = Yval;}
              if (Yval<Ymin) {Ymin = Yval;}
      
              // Logarithmin min & max:
              if (Positive==kTRUE)
              {
                  if (Yval>YmaxL) {YmaxL = Yval;}
                  if ((Yval<YminL)&&(YminL>0.0)) {YminL = Yval;}
              }
              else
              {
                  if (Yval<YminL) {YminL = Yval;}
                  if ((Yval>YmaxL)&&(YmaxL<0.0)) {YmaxL = Yval;}
              }
          }
      }
  }
  
  // Set the range of the function:
  Double_t Xlength = Xmax - Xmin;
  Double_t Ylength = Ymax - Ymin;
  
  for (Int_t nf = 0; nf<nFunc; ++nf)
  {
      if (UseLogScale==kTRUE)
      {
          if (Positive==kTRUE)
          {
              Raw[nf]->SetMaximum(YmaxL*2.0);
              Raw[nf]->SetMinimum(YminL/2.0);
          }
          else
          {
              Raw[nf]->SetMaximum(YmaxL/2.0);
              Raw[nf]->SetMinimum(YminL*2.0);
          }
      }
      else
      {
          Raw[nf]->SetMinimum(Ymin - 0.2*Ylength);
          Raw[nf]->SetMaximum(Ymax + 0.2*Ylength);
      }
      
      Raw[nf]->SetRange(Xmin,Xmax);
  }
  
  // Srt Logarithmic scale:
  if (UseLogScale==kTRUE)
  {
      gStyle->SetOptLogy(1);
  }
  else
  {
      gStyle->SetOptLogy(0); // decide AGAINST log-scale.
  }
  
  // Color options are down at the bottom.
  
  // =======================================================================================
  // Then we define the current style for histogram plotting:
  
  // --------------------------------------------------------------------
  // General aspects:
  gStyle->SetFrameLineWidth(1.0); // Thickness of frame 
  gStyle->SetFrameLineColor(0); // Color of frame
  gStyle->SetLineWidth(ScaleFactor); // Line thickness of axis!
  gStyle->SetOptStat(0); // No stat box
  gStyle->SetOptLogx(0); // log-scale on x-axis, either 0 or 1.
  gStyle->SetScreenFactor(1.0); // size of the canvas on the screen.
  gStyle->SetPadBottomMargin(0.14); // margin of the graph to the boundary of the pad on the bottom.
  gStyle->SetPadTopMargin(0.11); // margin of the graph to the boundary of the pad on the Top.
  gStyle->SetPadLeftMargin(0.12); // margin of the graph to the boundary of the pad on the Left.
  gStyle->SetPadRightMargin(0.05); // margin of the graph to the boundary of the pad on the Right.
  
  gStyle->SetLineColor(1);
  
  // --------------------------------------------------------------------
  // Controlling the Title.
  
  gStyle->SetTitleFont(1); // Title font type
  gStyle->SetTitleTextColor(1); // Title font color
  gStyle->SetTitleW(1.0); // Title font size horizontal
  gStyle->SetTitleH(0.1); // Title font size vertical; real font size is an interplay between them!  gPad->SetPad(0.0,-0.05,1.0,1.0);
  gStyle->SetTitleY(0.99); // Title vertical position
  gStyle->SetTitleX(0.5); // Title horizontal position
  
  // --------------------------------------------------------------------
  // Controlling the X-axis is done in a similar manner. Now "x" tells that it is about the x-axis!
  // NB: root calls the numerical scale a label. The text table is called a title in root!
  
  gStyle->SetAxisColor(12,"x"); // x-axis own color
  gStyle->SetTickLength(0.03,"x"); // x-axis length of the numerical scale markers
  gStyle->SetNdivisions(1010,"x"); // amount of scale markers. n = n1+100*n2+10000*n3 primary, secondary & third divisions. default = 510.
  
  gStyle->SetLabelFont(2,"x"); // x-axis numerical scale font type
  gStyle->SetLabelColor(12,"x"); // x-axis numerical scale font color
  gStyle->SetLabelSize(0.04,"x"); // x-axis numerical scale font size
  gStyle->SetLabelOffset(0.01,"x"); // x-axis numerical scale vertical position
  
  gStyle->SetTitleFont(1,"x"); // x-axis label font type
  gStyle->SetTitleColor(1,"x"); // x-axis label font color
  gStyle->SetTitleSize(0.05,"x"); // x-axis label font size
  gStyle->SetTitleOffset(1.1,"x"); // x-axis label vertical position
  
  // --------------------------------------------------------------------
  // Controlling the Y-axis is done in a similar manner. Now "y" tells that it is about the x-axis!
  
  gStyle->SetAxisColor(12,"y"); // y-axis own color
  gStyle->SetTickLength(0.03,"y"); // y-axis length of the numerical scale markers
  gStyle->SetNdivisions(410,"y"); // amount of scale markers. n = n1+100*n2+10000*n3 primary, secondary & third divisions. default = 510.
  
  gStyle->SetLabelFont(2,"y"); // y-axis numerical scale2.0,0.0 font type
  gStyle->SetLabelColor(12,"y"); // y-axis numerical scale font color
  gStyle->SetLabelSize(0.04,"y"); // y-axis numerical scale font size
  gStyle->SetLabelOffset(0.01,"y"); // y-axis numerical scale vertical position
  
  gStyle->SetTitleFont(1,"y"); // y-axis label font type
  gStyle->SetTitleColor(1,"y"); // y-axis label font color
  gStyle->SetTitleSize(0.05,"y"); // y-axis label font size
  gStyle->SetTitleOffset(1.10,"y"); // y-axis label horizontal position
  
  // =======================================================================================
  // We add some text inside the picture: 

  // Use the legenda:
  TPaveText** Legtext = new TPaveText*[nFunc];
  Double_t Yposition = 1.0;
  
  if (UseLogScale==kFALSE) {Yposition = Ymax + 0.1*Ylength;}
  else {Yposition = YmaxL*1.5;}
  
  for (Int_t nf = 0; nf<nFunc; ++nf)
  {
      if (UseLogScale==kFALSE)
      {
          Yposition = Yposition - 0.05*Ylength;
      }
      else
      {
          Yposition = Yposition*TMath::Power(Ymin/Ymax,0.05);
      }
      
      Legtext[nf] = new TPaveText(Xmin+Xlength*0.8,Yposition,Xmin+Xlength*0.81,Yposition,"NB");
      Legtext[nf]->SetFillColor(0);
      Legtext[nf]->AddText(Legenda[nf]);
      Legtext[nf]->SetTextColor(nf+1);
      Legtext[nf]->SetTextSize(0.04);
      Legtext[nf]->SetTextFont(1);
      Legtext[nf]->SetTextAngle(0.0);
  }
  
  // ======================================================================================
  // Finally, we do the real painting:
  Int_t wd = (Int_t) (700.0*ScaleFactor);
  Int_t hd = (Int_t) (500.0*ScaleFactor);
  TCanvas* c = new TCanvas(Title,Title,wd,hd); // To create a fresh canvas for the painting.
  if (UseLogScale==kTRUE) {gPad->SetLogy(1);}
  
  // Do actual painting:
  for (Int_t nf = 0; nf<nFunc; ++nf)
  {
      Raw[nf]->UseCurrentStyle();
      Raw[nf]->SetLineColor(nf+1); 
      Raw[nf]->SetLineWidth(ScaleFactor); 
      if (nf==0) {Raw[nf]->Draw("c");}
      else {Raw[nf]->Draw("csame");}
      Legtext[nf]->Draw("same");
      
      if (nf==0)
      {
          Raw[nf]->GetXaxis()->SetTitle(xlabel);
          Raw[nf]->GetYaxis()->SetTitle(ylabel);
          Raw[nf]->GetXaxis()->CenterTitle(kTRUE);
          Raw[nf]->GetYaxis()->CenterTitle(kTRUE);
      }
  }
  
  gPad->SetPad(0.0,0.0,1.0,1.0); // To set the pad size correctly.
  // Size of the pad inside the canvas. order is x1,y1,x2,y2. (x1,y1) is left bottom corner.
  // (x2,y2) is right upper corner. Coordinates are on a scale from 0.0 to 1.0. You can blow up the
  // picture beyond these limits but you risk that it falls outside the figure then.
  //gPad->GetXaxis()->SetTitle(xlabel);
  
  // ========================================================================================
  // In the end, return the TCanvas:
  return c;
}
