// Include the required root-headers:
#include "TMath.h"
#include "TH1D.h"
#include "TH1I.h"
#include "TF1.h"
#include "TStyle.h"
#include "TPad.h"
#include "TPaveText.h"
#include "TCanvas.h"
#include "TString.h"
#include "TFile.h"
#include "TSystem.h"

TCanvas** CrossSections_7LiPN::DrawHistDoumaArray(Int_t const Nhist, Int_t const Nhor, Int_t const Nvert, TH1D* const Raw[], TString const Title[], TString const xlabel, TString const ylabel, Int_t const color, Double_t const ScaleFactor, Bool_t const GlobalMax)
{
  // This program paints an array of histograms. It will create canvasses with Nhor rows and Nvert columns. In each column we will
  // have a single plot. Nhist is the total amount of histograms we receive. If there are more histograms then one can fit
  // on the canvas, a new canvas will open up. Raw is a C++ array of histograms that we will plot. Title, xlabel, ylabel
  // are the labels we will put on the plot.
  
  // This program controls every style aspect of the plot.
  
  // Outputs are the TCanvas.
  
  // Written by C. A. Douma.
    
  // ======================================================================================
  // We begin with declaring some variables:
  TString st = "";
  TString kstr = "";
  Double_t Xmin = Raw[0]->GetXaxis()->GetXmin();
  Double_t Xmax = Raw[0]->GetXaxis()->GetXmax();
  Double_t XcMax= 0.0;
  Double_t XcMin = 0.0;
  Double_t max = Raw[0]->GetMaximum();
  Double_t min = Raw[0]->GetMinimum();
  Double_t cmax = 0.0;
  Double_t cmin = 0.0;
  Int_t npx = 1;
  Int_t cnpx = 1;

  // Then perform searches for the global min and max:
  for (Int_t k=0; k<Nhist; ++k)
  {
    XcMin = Raw[k]->GetXaxis()->GetXmin();
    XcMax = Raw[k]->GetXaxis()->GetXmax();
    cmax = Raw[k]->GetMaximum();
    cmin = Raw[k]->GetMinimum();
    cnpx = Raw[k]->GetNbinsX();
    
    if (XcMax>Xmax) {Xmax = XcMax;}
    if (XcMin<Xmin) {Xmin = XcMin;}
    if (cmax>max) {max = cmax;}
    if (cmin<min) {min = cmin;}
    if (cnpx>npx) {npx = cnpx;}

  }
  
  // Then perform histogram settings:
  for (Int_t k=0; k<Nhist; ++k)
  {
  // Define kstring:
  kstr = st.Itoa(k+1,10);
    
  // We Add labels to the histogram:
  Raw[k]->SetTitle(Title[k]);
  Raw[k]->SetXTitle(xlabel);
  Raw[k]->SetYTitle(ylabel);
  
  // Control position of axis labels along the axis:
  Raw[k]->GetXaxis()->CenterTitle(kTRUE);
  Raw[k]->GetYaxis()->CenterTitle(kTRUE);
  // kFALSE will put the labels at the end of the axis.
  
  // Set range of X-axis:
  Raw[k]->SetAxisRange(Xmin,Xmax,"x");
  
  // set functional ranges:
  if (GlobalMax==kFALSE)
  {
    Raw[k]->SetMaximum(1.25*Raw[k]->GetMaximum());
    Raw[k]->SetMinimum(-0.05*Raw[k]->GetMaximum());
  }
  else
  {
      Raw[k]->SetMaximum(1.25*max);
      Raw[k]->SetMinimum(-0.05*max);
  }
  
  //Raw[k]->SetMinimum(0.3); // for log scale.
  
  // Add the histogram painting options:
  Raw[k]->SetOption("E0");
  // We have "hist", "E", "E0-6", etc. see THistPainter ROOT.
  }
  
  // =======================================================================================
  // Then we define the current style for histogram plotting:
  
  // --------------------------------------------------------------------
  // General aspects:
  for (Int_t k = 0; k<Nhist; ++k)
  {
      Raw[k]->SetLineWidth(ScaleFactor); // histogram line thickness.
  }
  
  gStyle->SetFrameLineWidth(1.0); // Thickness of frame 
  gStyle->SetFrameLineColor(0); // Color of frame
  gStyle->SetHistFillColor(0); // Fill color of histogram
  gStyle->SetHistLineColor(color); // line color of histogram
  gStyle->SetHistLineWidth(1.5); // line thickness of histogram
  gStyle->SetLineWidth(ScaleFactor); // Line thickness of axis!
  gStyle->SetOptStat(0); // No stat box
  gStyle->SetOptLogx(0); // log-scale on x-axis, either 0 or 1.
  gStyle->SetOptLogy(0); // log-scale on y-axis, either 0 or 1.
  gStyle->SetScreenFactor(1.0); // size of the canvas on the screen.
  gStyle->SetPadBottomMargin(0.14); // margin of the histogram to the boundary of the pad on the bottom.
  gStyle->SetPadTopMargin(0.14); // margin of the histogram to the boundary of the pad on the Top.
  gStyle->SetPadLeftMargin(0.14); // margin of the histogram to the boundary of the pad on the Left.
  gStyle->SetPadRightMargin(0.05); // margin of the histogram to the boundary of the pad on the Right.
  // 0=white 1=black 2=red 3=green 4=blue 5=yellow 6=magenta 7=cyaan 8=darkgreen 9=purple, there are lots of other options...
  
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
  gStyle->SetTitleOffset(1.30,"y"); // y-axis label horizontal position
  
  // =======================================================================================
  // We add some text inside the picture: 
  
  // First text box.
  TPaveText* text = new TPaveText(Xmin+(Xmax-Xmin)*0.95,1.16*max,Xmin+(Xmax-Xmin)*0.96,1.17*max,"NB"); // NB says that no borders are drawn. 
  // Creates an empty text box between (x1,y1) and (x2,y2). Input arguments are in the order x1,y1,x2,y2.
  // Coordinates are the same as the histogram!
  // x1 and y1 are left lower corner and x2 and y2 are right upper corner of the box.
  // Now we have the empty text field. Program the field now:
  text->SetFillColor(0); // Color of the box.
  text->AddText("Written by"); // Add text to the box. Text is printed at the center of the box.
  text->SetTextColor(16); // Define text color
  text->SetTextSize(0.03); // Define text size
  text->SetTextFont(1); // Define text font
  text->SetTextAngle(0.0); // Defines the angle at which the text is displayed.
  
  // Second text box:
  TPaveText* text2 = new TPaveText(Xmin+(Xmax-Xmin)*0.95,1.09*max,Xmin+(Xmax-Xmin)*0.96,1.10*max,"NB"); // NB says that no borders are drawn. 
  text2->SetFillColor(0); 
  text2->AddText("C. A. Douma"); 
  text2->SetTextColor(16); 
  text2->SetTextSize(0.03); 
  text2->SetTextFont(1);
  text2->SetTextAngle(0.0); 
  
  // ========================================================================================
  // Design the zero line:
  
  TF1* zero = new TF1("zero","0.0*x",Xmin-0.1*(Xmax-Xmin),Xmax+0.1*(Xmax-Xmin));
  zero->SetLineWidth(ScaleFactor); // Function line with;
  zero->SetLineColor(16); // Function line color;
  zero->SetNpx(npx); // Setting the number of plotting points.
  
  // ======================================================================================
  // Finally, we do the real painting:
  Int_t wd = (Int_t) (700.0*ScaleFactor);
  Int_t hd = (Int_t) (500.0*ScaleFactor);
  
  // In this case we have to be Careful. At first we need to determine how many
  // canvasses we need:
  Int_t NCanvas = 0;
  Int_t HistCount = Nhist;
  while (HistCount>0)
  {
    HistCount = HistCount - (Nvert*Nhor);
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
        if (index<Nhist) {Raw[index]->UseCurrentStyle();}
        if (index<Nhist) {Raw[index]->Draw();}
        if (index<Nhist) {zero->Draw("csame");}
        if (index<Nhist) {/*text->Draw("same");*/}
        if (index<Nhist) {/*text2->Draw("csame");*/}
      }
    }
    Pages[k]->SetPad(0.0,0.0,1.0,1.0);
    // This function selects the margins of the entire canvas
    // to the boundaries. If you want to controle the margins
    // between the individual histograms, use the
    // gStyle->SetPadMargin functions higher up in this macro.
    
  }
  
  // That should do it! return the canvasses:
  return Pages;
  
  // ========================================================================================
  
}
