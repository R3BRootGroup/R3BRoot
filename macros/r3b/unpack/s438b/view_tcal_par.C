
#include "../../land/drawStyle.C"


void view_module_neuland(TString runNumber, Int_t iPlane, Int_t iBar, Int_t iSide)
{
    TString inDir = "/u/mheil/data/neuland_tamex/par/";
    
    const Int_t fNofBars = 50;
    
    TString fileName = inDir + "params_online_mapped_neuland.root";
    TFile *parFile = new TFile(fileName);
    R3BTCalPar *par = (R3BTCalPar*) parFile->Get("LandTCalPar");
    
    fileName = "/u/mheil/data/neuland_tamex/tcal/online_mapped_neuland.root";
    TFile *file = new TFile(fileName);
    char str[100];
    Int_t index = iPlane * fNofBars*4 + (iBar-1)*4 + (iSide)*2;
    sprintf(str, "LandTCalPar_tcaldata_%d", index);
    TH1F *hdata = (TH1F*) file->Get(str);
    sprintf(str, "LandTCalPar_time_%d", index);
    TH1F *h1 = (TH1F*) file->Get(str);

    R3BTCalModulePar *mpar;
    for(Int_t k = 0; k < par->GetNumModulePar(); k++)
    {
        mpar = par->GetModuleParAt(k);
        if(mpar->GetModuleId() == index)
        {
            break;
        }
    }

    TPstyle();
    Style(h1, "TAC channel", "Time (ns)");
    h1->GetYaxis()->SetNdivisions(510);
    TCanvas *c1 = new TCanvas("c1", "", 10, 10, 1400, 700);
    c1->Divide(2, 1);
    c1->cd(1);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    h1->Draw();
    sprintf(str, "Bar: %3d, Side: %d", iBar, iSide);
    TPaveText *text1 = new TPaveText(0.192, 0.780, 0.623, 0.875, "NDC");
    text1->AddText(str);
    Style(text1);
    text1->Draw();
    
    for(Int_t i = 0; i < mpar->GetNofChannels(); i++)
    {
        Double_t x1 = mpar->GetBinLowAt(i);
        Double_t x2 = mpar->GetBinUpAt(i);
        Double_t t1 = mpar->GetOffsetAt(i);
        Double_t t2 = t1 + mpar->GetSlopeAt(i)*(x2 - x1);
        TLine *line = new TLine(x1, t1, x2, t2);
        line->SetLineColor(2);
        line->Draw();
    }
    
    
    c1->cd(2);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    TH1F *hd = new TH1F("hd", "", 4096, 0.5, 4096.5);
    Style(hd, "TAC channel", "Time difference (ns)");
    for(Int_t i = 1; i <= 4096; i++)
    {
        Double_t time = mpar->GetTimeTacquila(i);
        if(time > -1000.)
        {
            hd->SetBinContent(i, h1->GetBinContent(i) - time);
        }
    }
    hd->Draw();
    c1->cd(0);
    
    
    TCanvas *c2 = new TCanvas("c2", "", 100, 10, 500, 500);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    Style(hdata, "TAC channel", "Entries");
    hdata->Draw();
}



void view_module_los(TString runNumber, Int_t channel)
{
    TString inDir = "/Users/kresan/data/s438b/data/";
    
    TString fileName = inDir + "params_" + runNumber + "_raw_land.root";
    TFile *parFile = new TFile(fileName);
    R3BTCalPar *par = (R3BTCalPar*) parFile->Get("LosTCalPar");
    
    fileName = inDir + runNumber + "_raw_land.root";
    TFile *file = new TFile(fileName);
    char str[100];
    sprintf(str, "LosTCalPar_tcaldata_%d", channel);
    TH1F *hdata = (TH1F*) file->Get(str);
    sprintf(str, "LosTCalPar_time_%d", channel);
    TH1F *h1 = (TH1F*) file->Get(str);
    
    R3BTCalModulePar *mpar;
    for(Int_t k = 0; k < par->GetNumModulePar(); k++)
    {
        mpar = par->GetModuleParAt(k);
        if(mpar->GetModuleId() == channel)
        {
            break;
        }
    }
    
    TPstyle();
    Style(h1, "TAC channel", "Time (ns)");
    h1->GetYaxis()->SetNdivisions(510);
    TCanvas *c1 = new TCanvas("c1", "", 10, 10, 1400, 700);
    c1->Divide(2, 1);
    c1->cd(1);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    h1->Draw();
    sprintf(str, "Channel: %2d", channel);
    TPaveText *text1 = new TPaveText(0.580, 0.780, 0.909, 0.875, "NDC");
    text1->AddText(str);
    Style(text1);
    text1->Draw();
    
    TH1F *hcal = new TH1F("hcal", "", 4096, 0.5, 4096.5);
    for(Int_t i = 0; i < mpar->GetNofChannels(); i++)
    {
        Int_t x1 = mpar->GetBinLowAt(i);
        Double_t t1 = mpar->GetOffsetAt(i);
        hcal->SetBinContent(x1, t1);
    }
    hcal->SetLineColor(2);
    hcal->SetFillColor(2);
    hcal->SetMarkerStyle(24);
    hcal->SetMarkerColor(2);
    hcal->Draw("psame");
    h1->Draw("same");
    
    
    c1->cd(2);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    TH1F *hd = new TH1F("hd", "", 4096, 0.5, 4096.5);
    Style(hd, "TAC channel", "Time difference (ns)");
    for(Int_t i = 1; i <= 4096; i++)
    {
        Double_t time = mpar->GetTimeVFTX(i);
        if(time > -1000.)
        {
            hd->SetBinContent(i, h1->GetBinContent(i) - time);
        }
    }
    hd->Draw();
    c1->cd(0);
    
    
    TCanvas *c2 = new TCanvas("c2", "", 100, 10, 500, 500);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.05);
    Style(hdata, "TAC channel", "Entries");
    hdata->Draw();
}



