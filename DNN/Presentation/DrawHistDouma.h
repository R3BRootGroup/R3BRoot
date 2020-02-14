// Include the required root-headers:
#include "TMath.h"
#include "TH1D.h"
#include "TF1.h"
#include "TStyle.h"
#include "TPad.h"
#include "TPaveText.h"
#include "TCanvas.h"
#include "TString.h"
#include "TFile.h"
#include "TSystem.h"

TCanvas* DrawHistDouma(TH1D* const Raw, TString const Title, TString const xlabel, TString const ylabel, Int_t const Color, Bool_t const UseLogScale, Double_t const ScaleFactor, Double_t const max)
{
  // This program paints the histogram TH1D* Raw. 
  // This program controls every aspect of the plot
  // except the histogram data, which is taken from the input.
  // Hence ass style aspects are defined here.
  
  // Output is max, the highest point in the histogram.
    // NOTE: We do not need the ScaleFactor as an input, since it is a class member!
  
  // Written by C. A. Douma.
      
  // ======================================================================================
  // We begin with specifying some settings of the histogram:
  
  // We Add labels to the histogram:
  Raw->SetTitle(Title);
  Raw->SetXTitle(xlabel);
  Raw->SetYTitle(ylabel);
  
  // Control position of axis labels along the axis:
  Raw->GetXaxis()->CenterTitle(kTRUE);
  Raw->GetYaxis()->CenterTitle(kTRUE);
  // kFALSE will put the labels at the end of the axis.
  
  // Set ranges of the histogram:
  Double_t Xmin = Raw->GetXaxis()->GetXmin(); // X-axis
  Double_t Xmax = Raw->GetXaxis()->GetXmax();
  Raw->SetAxisRange(Xmin,Xmax,"x");
  
  Raw->SetMaximum(1.62*max);
  Raw->SetMinimum(-0.1*max);
  if (UseLogScale==kTRUE) {Raw->SetMinimum(0.3);}
  
  // Add the histogram painting options:
  Raw->SetOption("hist");
  // We have "hist", "E", "E0-6", etc. see THistPainter ROOT.
  
  // =======================================================================================
  // Then we define the current style for histogram plotting:
  
  // --------------------------------------------------------------------
  // General aspects:
  Raw->SetLineWidth(ScaleFactor*0.5); // histogram line thickness.
  gStyle->SetFrameLineWidth(1.0); // Thickness of frame 
  gStyle->SetFrameLineColor(0); // Color of frame
  gStyle->SetHistFillColor(0); // Fill color of histogram
  gStyle->SetHistLineColor(Color); // line color of histogram
  gStyle->SetHistLineWidth(ScaleFactor); // line thickness of histogram
  gStyle->SetLineWidth(ScaleFactor); // Line thickness of axis!
  gStyle->SetOptStat(0); // No stat box
  gStyle->SetOptLogx(0); // log-scale on x-axis, either 0 or 1.
  gStyle->SetOptLogy(0); // log-scale on y-axis, either 0 or 1.
  if (UseLogScale==kTRUE) {gStyle->SetOptLogy(1);}
  gStyle->SetScreenFactor(1.0); // size of the canvas on the screen.
  gStyle->SetPadBottomMargin(0.14); // margin of the histogram to the boundary of the pad on the bottom.
  gStyle->SetPadTopMargin(0.11); // margin of the histogram to the boundary of the pad on the Top.
  gStyle->SetPadLeftMargin(0.12); // margin of the histogram to the boundary of the pad on the Left.
  gStyle->SetPadRightMargin(0.08); // margin of the histogram to the boundary of the pad on the Right.
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
  gStyle->SetTitleOffset(1.10,"y"); // y-axis label horizontal position
  
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
  
  // ======================================================================================
  // Finally, we do the real painting:
  Int_t wd = (Int_t) (700.0*ScaleFactor);
  Int_t hd = (Int_t) (500.0*ScaleFactor);
  TCanvas* c = new TCanvas(Title,Title,wd,hd); // To create a fresh canvas for the painting.
  
  Raw->UseCurrentStyle(); // To use all the gStyle settings!
  Raw->Draw(); // To actually draw.
  //text->Draw("same"); // Draw the text-boxes:
  //text2->Draw("same");
  gPad->SetPad(0.0,0.0,1.0,1.0); // To set the pad size correctly.
  // Size of the pad inside the canvas. order is x1,y1,x2,y2. (x1,y1) is left bottom corner.
  // (x2,y2) is right upper corner. Coordinates are on a scale from 0.0 to 1.0. You can blow up the
  // picture beyond these limits but you risk that it falls outside the figure then.
  
  // ========================================================================================
  // Finally, we draw the zero line:
  
  TF1* zero = new TF1("zero","0.0*x",Xmin-0.1*(Xmax-Xmin),Xmax+0.1*(Xmax-Xmin));
  zero->SetLineWidth(ScaleFactor); // Function line with;
  zero->SetLineColor(16); // Function line color;
  zero->SetNpx(Raw->GetNbinsX()); // Setting the number of plotting points.
  zero->Draw("csame");
  
  // =========================================================================================
  // Finally, return the TCanvas:
  return c;
}
