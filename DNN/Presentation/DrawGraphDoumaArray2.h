// Include the required root-headers:
#include "TMath.h"
#include "TH1D.h"
#include "TH1I.h"
#include "TGraphErrors.h"
#include "TStyle.h"
#include "TPad.h"
#include "TPaveText.h"
#include "TCanvas.h"
#include "TString.h"
#include "TSystem.h"
#include "TFile.h"

TCanvas* DrawGraphDoumaArray2(Int_t const Npads, Int_t const Nhor, Int_t const Nvert, TGraphErrors** const Raw, TString const Title[], TString const xlabel, TString const ylabel, Double_t const ScaleFactor, Int_t const Color, TString const PlotStyle = "AP")
{
  // This program paints a collections of TGraphErrors. A 1D array should be supplied.
  // The index should have length Npads and represents the number of distinct
  // figures you want. Nhor and Nvert are the
  // number of horizontal and vertical figures on the canvas. Title is an array with
  // length Npads and represents the title of each figure. xlabal and ylabel represent
  // the text at the axis that you want. Finally, the boolian print states if we should
  // print this pictures yes/no. close states whether we should colse our graphs
  // afterward yes/no. Since the output is the graph, the function returns a void.
  // GraphColor is an array of size Ngraphs that contains the colors that the graphs
  // should have. Legenda is an array of TStrings that will be used to print a 
  // legenda: one for each graph within a figure. The colors will match with the graphs.
  
  // Written by C. A. Douma.
    
  // ======================================================================================
  // Now each figure should have its own maximum and minimum. Hence per figure we should
  // search for this.
  
  // So first declare some variables that we want:
  TString st = "";
  TString kstr = "";
  Double_t* Xmin = new Double_t [Npads];
  Double_t* Xmax = new Double_t [Npads];
  Double_t* Ymin = new Double_t [Npads];
  Double_t* Ymax = new Double_t [Npads];
  Double_t length = 0.0;
  
  // Extract the Xmin & max & Ymin & max from the graphs:
  for (Int_t k = 0; k<Npads; ++k)
  {
      Xmax[k] = 11.0;
      Xmin[k] = 0.0;
      Ymax[k] = 110.0;
      Ymin[k] = 0.0;
  }
  
  // Set ranges on the graphs & more:
  for (Int_t k = 0; k<Npads; ++k)
  {
      // We Add labels to the TGraph:
      Raw[k]->SetTitle(Title[k]);
      (Raw[k]->GetXaxis())->SetTitle(xlabel);
      (Raw[k]->GetYaxis())->SetTitle(ylabel);
  
      // Control position of axis labels along the axis:
      Raw[k]->GetXaxis()->CenterTitle(kTRUE);
      Raw[k]->GetYaxis()->CenterTitle(kTRUE);
      // kFALSE will put the labels at the end of the axis.
  
      // Set ranges of the TGraph:
      length = Xmax[k] - Xmin[k];
      (Raw[k]->GetXaxis())->SetLimits(Xmin[k],Xmax[k]);
  
      length = Ymax[k] - Ymin[k];
      Raw[k]->SetMinimum(Ymin[k]);
      Raw[k]->SetMaximum(Ymax[k]);
      (Raw[k]->GetYaxis())->SetLimits(Ymin[k],Ymax[k]);
  }
  
  // We will set our color and plotting options down at the bottom.
  
  // =======================================================================================
  // Then we define the current style for graph plotting:
  
  // --------------------------------------------------------------------
  // General aspects:
  gStyle->SetFrameLineWidth(1.0); // Thickness of frame 
  gStyle->SetFrameLineColor(0); // Color of frame
  gStyle->SetLineWidth(ScaleFactor); // Line thickness of axis!
  gStyle->SetOptStat(0); // No stat box
  gStyle->SetOptLogx(0); // log-scale on x-axis, either 0 or 1.
  gStyle->SetOptLogy(0); // log-scale on y-axis, either 0 or 1.
  gStyle->SetScreenFactor(1.0); // size of the canvas on the screen.
  gStyle->SetPadBottomMargin(0.14); // margin of the graph to the boundary of the pad on the bottom.
  gStyle->SetPadTopMargin(0.11); // margin of the graph to the boundary of the pad on the Top.
  gStyle->SetPadLeftMargin(0.17); // margin of the graph to the boundary of the pad on the Left.
  gStyle->SetPadRightMargin(0.01); // margin of the graph to the boundary of the pad on the Right.
  // 0=white 1=black 2=red 3=green 4=blue 5=yellow 6=magenta 7=cyaan 8=darkgreen 9=purple, there are lots of other options...
  // NOTE: If you want to change the size of a figure within a sub-pad, change these SetPadMargins.
  
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
  gStyle->SetLabelOffset(0.01,"x"); // x-axis numerical scale vertical positionthe same as the histogram!
  
  gStyle->SetTitleFont(1,"x"); // x-axis label font type
  gStyle->SetTitleColor(1,"x"); // x-axis label font colorRaw->LabelsOption("v");
  gStyle->SetTitleSize(0.05,"x"); // x-axis label font size
  gStyle->SetTitleOffset(1.0,"x"); // x-axis label vertical position
  
  // --------------------------------------------------------------------
  // Controlling the Y-axis is done in a similar manner. Now "y" tells that it is about the y-axis!
  
  gStyle->SetAxisColor(12,"y"); // y-axis own color
  gStyle->SetTickLength(0.03,"y"); // y-axis length of the numerical scale markers
  gStyle->SetNdivisions(1010,"y"); // amount of scale markers. n = n1+100*n2+10000*n3 primary, secondary & third divisions. default = 510.
  
  gStyle->SetLabelFont(2,"y"); // y-axis numerical scale2.0,0.0 font type
  gStyle->SetLabelColor(12,"y"); // y-axis numerical scale font color
  gStyle->SetLabelSize(0.04,"y"); // y-axis numerical scale font size
  gStyle->SetLabelOffset(0.005,"y"); // y-axis numerical scale vertical position
  
  gStyle->SetTitleFont(1,"y"); // y-axis label font type
  gStyle->SetTitleColor(1,"y"); // y-axis label font color
  gStyle->SetTitleSize(0.05,"y"); // y-axis label font size
  gStyle->SetTitleOffset(1.75,"y"); // y-axis label vertical position
  
  // =======================================================================================
  // We do not add some text to the picture here. We do that later.
  // ======================================================================================
  // Finally, we do the real painting:
  Int_t wd = (Int_t) (700.0*ScaleFactor);
  Int_t hd = (Int_t) (500.0*ScaleFactor);
  
  // In this case we have to be Careful. At first we need to determine how many
  // canvasses we need:
  Int_t NCanvas = 0;
  Int_t GraphCount = Npads;
  while (GraphCount>0)
  {
    GraphCount = GraphCount - (Nvert*Nhor);
    NCanvas = NCanvas + 1;
  }
  
  // Now the number of canvasses we need is given. Make it constant:
  Int_t const CanvasArray_ii = NCanvas;
  Int_t const CanvasArray_i = CanvasArray_ii;
  Int_t const CanvasArray = CanvasArray_i;
  
  // Declare Canvas Array:
  TCanvas** Pages = new TCanvas*[CanvasArray];
  Int_t index = 0;
  Int_t padnr = 1;
  
  // Now start painting. Notice that the divided Canvas is 
  // first filled horizontally and then vertically!
  for (Int_t k=0; k<CanvasArray; ++k)
  {
    kstr = st.Itoa(k,10);
    Pages[k] = new TCanvas(Title[0] + "_Canvas_" + kstr,Title[0] + "_Canvas_" + kstr,wd,hd);
    Pages[k]->Divide(Nhor,Nvert);
    padnr = 1;
    
    for (Int_t khor = 0; khor<Nhor; ++khor)
    {
      for (Int_t kvert = 0; kvert<Nvert; ++kvert)
      {
	padnr = 1 + khor + Nhor*kvert;
        index = (k*(Nhor*Nvert)) + padnr - 1;
	
	Pages[k]->cd(padnr);
     //   if (UseLogScale==kTRUE) {gPad->SetLogy(1);}
	
	// Now draw all graphs at once:
        if (index<Npads) {Raw[index]->UseCurrentStyle();}
        if (index<Npads) {Raw[index]->SetLineWidth(ScaleFactor);}
        if (index<Npads) {Raw[index]->SetLineColor(Color);}
        if (index<Npads) {Raw[index]->Draw(PlotStyle.Data());}
      }
    }
    Pages[k]->SetPad(0.0,0.0,1.0,1.0);
    // This function selects the margins of the entire canvas
    // to the boundaries. If you want to controle the margins
    // between the individual histograms, use the
    // gStyle->SetPadMargin functions higher up in this macro.
    
  }
  
  // That should do it!
  
  // =========================================================================================
  
  // Return the canvasses:
    
  return Pages[0];
  
  // Done!
  
}
