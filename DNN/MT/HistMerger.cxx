// Include this class header:
#include "HistMerger.h"

// Default Constructor:
HistMerger::HistMerger()
{
    // Parameter definitions:
    InputFiles.clear();
    OutputFile = "./out.root";
    TheHistograms.clear();
    HistNames.clear();
    OutputHistograms.clear();
    InFiles = 0;
}
    
// Destructor:
HistMerger::~HistMerger() 
{
    // Parameter destructions:
    InputFiles.clear();
    TheHistograms.clear();
    HistNames.clear();
    OutputHistograms.clear();
    delete InFiles;
}

// Member functions:
void HistMerger::LoadHistograms()
{
    Int_t nFiles = InputFiles.size();
    Int_t nHists = HistNames.size();
    
    InFiles = new TFile*[nFiles];
    TheHistograms.resize(nFiles);
    
    for (Int_t kfile = 0; kfile<nFiles; ++kfile)
    {
        InFiles[kfile] = new TFile(InputFiles.at(kfile).Data(),"read");
        TheHistograms.at(kfile).resize(nHists);
        
        for (Int_t khist = 0; khist<nHists; ++khist)
        {
            TheHistograms.at(kfile).at(khist) = (TH1D*) InFiles[kfile]->Get(HistNames.at(khist).Data());
        }
    }
}

void HistMerger::AddHistograms()
{
    // Declare what we need:
    Int_t nBins;
    Double_t Xmin;
    Double_t Xmax;
    Double_t ThisBinContent;
    Double_t TotalBinContent;
    
    // Create output histograms:
    Int_t nHists = HistNames.size();
    OutputHistograms.resize(nHists);
    
    for (Int_t khist = 0; khist<nHists; ++khist)
    {
        if ((TheHistograms.at(0).at(khist)!=0)&&(TheHistograms.at(0).at(khist)!=nullptr))
        {
            nBins = TheHistograms.at(0).at(khist)->GetNbinsX();
            Xmin = TheHistograms.at(0).at(khist)->GetXaxis()->GetXmin();
            Xmax = TheHistograms.at(0).at(khist)->GetXaxis()->GetXmax();
            
            OutputHistograms.at(khist) = new TH1D((HistNames.at(khist)+"_Merged").Data(),HistNames.at(khist).Data(),nBins,Xmin,Xmax);
        }
    }
    
    // Perform addition:
    Int_t nFiles = InputFiles.size();
    
    for (Int_t kfile = 0; kfile<nFiles; ++kfile)
    {
        for (Int_t khist = 0; khist<nHists; ++khist)
        {
            if ((TheHistograms.at(kfile).at(khist)!=0)&&(TheHistograms.at(kfile).at(khist)!=nullptr))
            {
                nBins = TheHistograms.at(kfile).at(khist)->GetNbinsX();
                
                for (Int_t kbin = 0; kbin<(nBins+2); ++kbin)
                {
                    ThisBinContent = TheHistograms.at(kfile).at(khist)->GetBinContent(kbin);
                    TotalBinContent = OutputHistograms.at(khist)->GetBinContent(kbin);
                    TotalBinContent = TotalBinContent + ThisBinContent;
                    OutputHistograms.at(khist)->SetBinContent(kbin,TotalBinContent);
                }
            }
        }
    }
    
    // Done.
}

void HistMerger::WriteHistograms()
{
    TFile* OutFile = new TFile(OutputFile.Data(),"update");
    Int_t nHists = HistNames.size();
    Int_t nFiles = InputFiles.size();
    
    for (Int_t khist = 0; khist<nHists; ++khist)
    {
        OutputHistograms.at(khist)->Write(HistNames.at(khist).Data(),2);
    }
    
    OutFile->Close();
    delete OutFile;
    
    for (Int_t khist = 0; khist<nHists; ++khist)
    {
        delete OutputHistograms.at(khist);
    }
    
    for (Int_t kfile = 0; kfile<nFiles; ++kfile)
    {
        InFiles[kfile]->Close();
    }
}

void HistMerger::MergeHistograms()
{
    if ((InputFiles.size()>0)&&(HistNames.size()>0))
    {
        LoadHistograms();
        AddHistograms();
        WriteHistograms();
    }
}
        

// Generate ROOT dictionary:
ClassImp(HistMerger);
