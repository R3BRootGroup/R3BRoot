
#include "../../land/drawStyle.C"


void view_module(TString runNumber, Int_t iBar, Int_t iSide)
{
    TString inDir = "/Volumes/Data/kresan/s438b/data/";
    
    const Int_t fNofPMTs = 800;
    
    TString fileName = inDir + runNumber + "_raw_land.root";
    TFile *file = new TFile(fileName);
    char str[100];
    Int_t index = (Double_t)fNofPMTs / 2. * (iSide - 1) + iBar - 1;
    sprintf(str, "hTime_%d", index);
    TH1F *h1 = (TH1F*) file->Get(str);
    
    TPstyle();
    Style(h1, "TAC channel", "Time (ns)");
    h1->GetYaxis()->SetNdivisions(510);
    TCanvas *c1 = new TCanvas("c1", "", 10, 10, 500, 500);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    h1->Draw();
    sprintf(str, "Bar: %3d, Side: %d", iBar, iSide);
    TPaveText *t1 = new TPaveText(0.192, 0.780, 0.623, 0.875, "NDC");
    t1->AddText(str);
    Style(t1);
    t1->Draw();
}


