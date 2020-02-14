// Include this class header:
#include "DNNDataHist.h"

// Default Constructor:
DNNDataHist::DNNDataHist()
{
    // Locations:
    ThePath = "./";
    FileName = "stupid.root";
    HistName = "hist";
    PhysHeight = 0.0;
    
    // Prime hists:
    Hist_INCL_INCL = 0;
    Hist_INCL_INCL_2nd = 0;
    Hist_INCL_BERT = 0;
    Hist_BERT_INCL = 0;
    Hist_BERT_BERT = 0;
    
    // Computed Hists:
    DataHist = 0;
    StatHist = 0;
    PhysHist = 0;
    
    // NOTE: Default scales
    INCL_INCL_Scale = 1.0;
    INCL_INCL_2nd_Scale = 1.0;
    INCL_BERT_Scale = 1.0;
    BERT_INCL_Scale = 1.0;
    BERT_BERT_Scale = 1.0;
}
    
// Destructor:
DNNDataHist::~DNNDataHist()
{
    // Prime hists:
    delete Hist_INCL_INCL;
    delete Hist_INCL_INCL_2nd;
    delete Hist_INCL_BERT;
    delete Hist_BERT_INCL;
    delete Hist_BERT_BERT;
    
    // Computed Hists:
    delete DataHist;
    delete StatHist;
    delete PhysHist;
}

// Member function definitions:
void DNNDataHist::CollectPrimeHists()
{
    // Compose full file names:
    TString FileName_INCL_INCL     = ThePath + "/INCLXX/"    + "/INCLXX_Validation/" + FileName;
    TString FileName_INCL_INCL_2nd = ThePath + "/2ndINCLXX/" + "/INCLXX_Validation/" + FileName;
    TString FileName_INCL_BERT     = ThePath + "/INCLXX/"    + "/BERT_Validation/"   + FileName;
    TString FileName_BERT_INCL     = ThePath + "/BERT/"      + "/INCLXX_Validation/" + FileName;
    TString FileName_BERT_BERT     = ThePath + "/BERT/"      + "/BERT_Validation/"   + FileName;
    
    // Connect to the actual files:
    TFile* f_INCL_INCL     = new TFile(FileName_INCL_INCL.Data(),"read");
    TFile* f_INCL_INCL_2nd = new TFile(FileName_INCL_INCL_2nd.Data(),"read");
    TFile* f_INCL_BERT     = new TFile(FileName_INCL_BERT.Data(),"read");
    TFile* f_BERT_INCL     = new TFile(FileName_BERT_INCL.Data(),"read");
    TFile* f_BERT_BERT     = new TFile(FileName_BERT_BERT.Data(),"read");
    
    // Retrieve the histograms:
    Hist_INCL_INCL = (TH1D*)     f_INCL_INCL->Get(HistName.Data());
    Hist_INCL_INCL_2nd = (TH1D*) f_INCL_INCL_2nd->Get(HistName.Data());
    Hist_INCL_BERT = (TH1D*)     f_INCL_BERT->Get(HistName.Data());
    Hist_BERT_INCL = (TH1D*)     f_BERT_INCL->Get(HistName.Data());
    Hist_BERT_BERT = (TH1D*)     f_BERT_BERT->Get(HistName.Data());
    
    // Check that all histograms exist and have correct dimensions:
    Bool_t Existence = kTRUE;
         if ((Hist_INCL_INCL==0)||(Hist_INCL_INCL==nullptr))         {Existence = kFALSE; cout << "### ERROR: Hist_INCL_INCL could not be found!\n";}
    else if ((Hist_INCL_INCL_2nd==0)||(Hist_INCL_INCL_2nd==nullptr)) {Existence = kFALSE; cout << "### ERROR: Hist_INCL_INCL_2nd could not be found!\n";}
    else if ((Hist_INCL_BERT==0)||(Hist_INCL_BERT==nullptr))         {Existence = kFALSE; cout << "### ERROR: Hist_INCL_BERT could not be found!\n";}
    else if ((Hist_BERT_INCL==0)||(Hist_BERT_INCL==nullptr))         {Existence = kFALSE; cout << "### ERROR: Hist_BERT_INCL could not be found!\n";}
    else if ((Hist_BERT_BERT==0)||(Hist_BERT_BERT==nullptr))         {Existence = kFALSE; cout << "### ERROR: Hist_BERT_BERT could not be found!\n";}
    
    if (Existence==kTRUE)
    {
        Int_t Nbins = Hist_INCL_INCL->GetNbinsX();
        Double_t Xmin = Hist_INCL_INCL->GetXaxis()->GetXmin();
        Double_t Xmax = Hist_INCL_INCL->GetXaxis()->GetXmax();
        
        if ((Nbins!=Hist_INCL_INCL_2nd->GetNbinsX())||(Xmin!=Hist_INCL_INCL_2nd->GetXaxis()->GetXmin())||(Xmax!=Hist_INCL_INCL->GetXaxis()->GetXmax())) {cout << "### ERROR: Hist_INCL_INCL_2nd dimensions did not agree with Hist_INCL_INCL!\n";}
        if ((Nbins!=Hist_INCL_BERT->GetNbinsX())||(Xmin!=Hist_INCL_BERT->GetXaxis()->GetXmin())||(Xmax!=Hist_INCL_INCL->GetXaxis()->GetXmax())) {cout << "### ERROR: Hist_INCL_BERT dimensions did not agree with Hist_INCL_INCL!\n";}
        if ((Nbins!=Hist_BERT_INCL->GetNbinsX())||(Xmin!=Hist_BERT_INCL->GetXaxis()->GetXmin())||(Xmax!=Hist_INCL_INCL->GetXaxis()->GetXmax())) {cout << "### ERROR: Hist_BERT_INCL dimensions did not agree with Hist_INCL_INCL!\n";}
        if ((Nbins!=Hist_BERT_BERT->GetNbinsX())||(Xmin!=Hist_BERT_BERT->GetXaxis()->GetXmin())||(Xmax!=Hist_INCL_INCL->GetXaxis()->GetXmax())) {cout << "### ERROR: Hist_BERT_BERT dimensions did not agree with Hist_INCL_INCL!\n";}
    }
}

void DNNDataHist::ComputeNewHists()
{
    // Create the new histograms:
    Int_t Nbins = Hist_INCL_INCL->GetNbinsX();
    Double_t Xmin = Hist_INCL_INCL->GetXaxis()->GetXmin();
    Double_t Xmax = Hist_INCL_INCL->GetXaxis()->GetXmax();
    
    DataHist = new TH1D(HistName + "_DataHist",HistName + "_DataHist",Nbins,Xmin,Xmax);
    StatHist = new TH1D(HistName + "_StatHist",HistName + "_StatHist",Nbins,Xmin,Xmax);
    PhysHist = new TH1D(HistName + "_PhysHist",HistName + "_PhysHist",Nbins,Xmin,Xmax);
    
    // Transmit bin-content one-by-one:
    DNNDataPoint ThePoint;
    
    for (Int_t k = 0; k<(Nbins+2); ++k)
    {
        // Put the bins into the point:
        ThePoint.Set_INCLXX_INCLXX(Hist_INCL_INCL->GetBinContent(k)*INCL_INCL_Scale,TMath::Sqrt(TMath::Abs(Hist_INCL_INCL->GetBinContent(k)))*INCL_INCL_Scale);
        ThePoint.Set_INCLXX_INCLXX_2nd(Hist_INCL_INCL_2nd->GetBinContent(k)*INCL_INCL_2nd_Scale,TMath::Sqrt(TMath::Abs(Hist_INCL_INCL_2nd->GetBinContent(k)))*INCL_INCL_2nd_Scale);
        ThePoint.Set_INCLXX_BERT(Hist_INCL_BERT->GetBinContent(k)*INCL_BERT_Scale,TMath::Sqrt(TMath::Abs(Hist_INCL_BERT->GetBinContent(k)))*INCL_BERT_Scale);
        ThePoint.Set_BERT_INCLXX(Hist_BERT_INCL->GetBinContent(k)*BERT_INCL_Scale,TMath::Sqrt(TMath::Abs(Hist_BERT_INCL->GetBinContent(k)))*BERT_INCL_Scale);
        ThePoint.Set_BERT_BERT(Hist_BERT_BERT->GetBinContent(k)*BERT_BERT_Scale,TMath::Sqrt(TMath::Abs(Hist_BERT_BERT->GetBinContent(k)))*BERT_BERT_Scale);
        
        // Then, compute the averages:
        ThePoint.ComputeAvgPoint();
        
        // Then, put the outcome into the new histograms:
        DataHist->SetBinContent(k,ThePoint.GetAvgPoint());
        DataHist->SetBinError(k,ThePoint.GetStatError());
        
        StatHist->SetBinContent(k,ThePoint.GetAvgPoint());
        StatHist->SetBinError(k,ThePoint.GetStatError());
        
        PhysHist->SetBinContent(k,PhysHeight);
        PhysHist->SetBinError(k,ThePoint.GetPhysError());
        
        // Done.
    }
}

Double_t DNNDataHist::GetPrimeHistContent(TString const Type)
{
    // Type selection:
    TH1D* HistPointer;
    if (Type=="INCL_INCL") {HistPointer = Hist_INCL_INCL;}
    if (Type=="INCL_INCL_2nd") {HistPointer = Hist_INCL_INCL_2nd;}
    if (Type=="INCL_BERT") {HistPointer = Hist_INCL_BERT;}
    if (Type=="BERT_INCL") {HistPointer = Hist_BERT_INCL;}
    if (Type=="BERT_BERT") {HistPointer = Hist_BERT_BERT;}
    
    // Integral calculation:
    Double_t Hist_Integral = 0.0;
    Int_t Nbins = HistPointer->GetNbinsX();
    Double_t Xmin = HistPointer->GetXaxis()->GetXmin();
    Double_t Xmax = HistPointer->GetXaxis()->GetXmax();
    
    for (Int_t k = 0; k<(Nbins+2); ++k)
    {
        Hist_Integral = Hist_Integral + HistPointer->GetBinContent(k);
    }
    
    // Return the Answer:
    return Hist_Integral;
}

Double_t DNNDataHist::GetDataHistMax()
{
    // Maximum calculation:
    Double_t Hist_Max = 0.0;
    Int_t Nbins = DataHist->GetNbinsX();
    Double_t Xmin = DataHist->GetXaxis()->GetXmin();
    Double_t Xmax = DataHist->GetXaxis()->GetXmax();
    
    for (Int_t k = 0; k<(Nbins+2); ++k)
    {
        if (DataHist->GetBinContent(k)>Hist_Max)
        {
            Hist_Max = DataHist->GetBinContent(k);
        }
    }
    
    // Return the Answer:
    return Hist_Max;
}

void DNNDataHist::RemoveEndBins()
{
    Int_t Nbins = DataHist->GetNbinsX();
    Double_t Xmin = DataHist->GetXaxis()->GetXmin();
    Double_t Xmax = DataHist->GetXaxis()->GetXmax();
    
    DataHist->SetBinContent(0,0.0);
    StatHist->SetBinContent(0,0.0);
    PhysHist->SetBinContent(0,0.0);
    
    DataHist->SetBinError(0,0.0);
    StatHist->SetBinError(0,0.0);
    PhysHist->SetBinError(0,0.0);
    
    DataHist->SetBinContent(Nbins+1,0.0);
    StatHist->SetBinContent(Nbins+1,0.0);
    PhysHist->SetBinContent(Nbins+1,0.0);
    
    DataHist->SetBinError(Nbins+1,0.0);
    StatHist->SetBinError(Nbins+1,0.0);
    PhysHist->SetBinError(Nbins+1,0.0);
}

void DNNDataHist::SetPhysHeight(Double_t const pos)
{
    PhysHeight = pos;
    
    if ((PhysHist!=0)&&(PhysHist!=nullptr))
    {
        Int_t Nbins = PhysHist->GetNbinsX();
        Double_t Xmin = PhysHist->GetXaxis()->GetXmin();
        Double_t Xmax = PhysHist->GetXaxis()->GetXmax();
        
        for (Int_t k = 0; k<(Nbins+2); ++k)
        {
            PhysHist->SetBinContent(k,PhysHeight);
        }
    }
}        
    
// Generate ROOT dictionary:
ClassImp(DNNDataHist);
