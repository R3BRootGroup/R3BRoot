// Every CXX-file includes its own header:
#include "ScoringClass.h"

// Include plotting functions:
#include "DrawHistDouma.h"
#include "PlotScorer.h"

// Default Constructor definition:
ScoringClass::ScoringClass()
{
    Key = "Key";
    ScorerName = "";
    FullSpectrum = 0;
    PrimarySpectrum = 0;
    OutputFile = 0;
    IsCluster = kFALSE;
    Plot_UseLogScale = kFALSE;
    
    Errors.clear();
    PrintErrors = kTRUE;
    
    ScaleFactor = 4.0;
}

// Destructor definition:
ScoringClass::~ScoringClass() 
{
    delete FullSpectrum;
    delete PrimarySpectrum;
    
    Errors.clear();
}

// Member function definitions:
// Set-functions:
void ScoringClass::SetName(TString const Name) {ScorerName = Name;}
void ScoringClass::SetKey(TString const ThisKey) {Key = ThisKey;}
void ScoringClass::MarkScorerForClusters() {IsCluster = kTRUE;}
void ScoringClass::MarkScorerForSignals() {IsCluster = kFALSE;}
void ScoringClass::SetPlotLogScale() {Plot_UseLogScale = kTRUE;}
void ScoringClass::SetPlotLinearScale() {Plot_UseLogScale = kFALSE;}
Bool_t ScoringClass::IsMarkedAsCluster() {return IsCluster;}
Bool_t ScoringClass::IsPlotLogScale() {return Plot_UseLogScale;}

Double_t ScoringClass::GetMeanShift()
{
    Double_t Xmin = FullSpectrum->GetXaxis()->GetXmin();
    Double_t Xmax = FullSpectrum->GetXaxis()->GetXmax();
    Double_t MeanShift = (Xmax+Xmin)/2.0;
    return MeanShift;
}

Double_t ScoringClass::GetScaleFactor()
{
    Double_t Xmin = FullSpectrum->GetXaxis()->GetXmin();
    Double_t Xmax = FullSpectrum->GetXaxis()->GetXmax();
    Double_t TheScaleFactor = TMath::Abs(Xmax-Xmin)/2.0;
    return TheScaleFactor;
}

void ScoringClass::AssignOutputFile(TFile* f)
{
    // Begin by testing that the file you supplied is not a null-pointer:
    if ((f==0)||(f==nullptr))
    {
        ErrorMessage("You tried to assign an output-file that contains a null-pointer!");
    }
    else
    {
        // Next, see if the file is open
        if (f->IsOpen()==kFALSE)
        {
            ErrorMessage("You tried to assign an output-file that is not opened!");
        }
        else
        {
            // Retrieve current file option:
            TString FileOption = (TString) f->GetOption();
            
            if (FileOption=="READ")
            {
                ErrorMessage("You tried to assign an output-file that is read-only!");
            }
            else
            {
                // Then, we are OK:
                OutputFile = f;
            }
        }
    }
}

void ScoringClass::UpdateScorer(Double_t const Value, Bool_t const IsPrimary)
{
    // Easy: just fill histograms:
    FullSpectrum->Fill(Value);
    
    if (IsPrimary==kTRUE)
    {
        PrimarySpectrum->Fill(Value);
    }
}    
        
// Get-functions:
TString ScoringClass::GetName()
{
    return ScorerName;
}

TFile* ScoringClass::GetOutputFile()
{
    return OutputFile;
}

Double_t ScoringClass::GetHistScore(Double_t const Value, Bool_t const IsPrimary)
{
    // Begin by extracting the bin of Value.
    Int_t CurrentBin = -1;
    Int_t Nbins = FullSpectrum->GetNbinsX();
    Double_t Xmin = FullSpectrum->GetXaxis()->GetXmin();
    Double_t Xmax = FullSpectrum->GetXaxis()->GetXmax();
    Double_t Step = TMath::Abs(Xmax-Xmin)/((Int_t) Nbins);
    
    if ((Value>Xmin)&&(Value<Xmax))
    {
        if (Nbins<=1)
        {
            CurrentBin = 1;
        }
        else
        {
            for (Int_t k = 0; k<(Nbins-1); ++k)
            {
                if ((Value>(((Int_t) k)*Step+Xmin))&&(Value<(((Int_t) (k+1))*Step+Xmin)))
                {
                    CurrentBin = k+1;
                }
            }
        }
    }
    
    // Next, find out the answer:
    Double_t Answer = 0.0;
    
    if (CurrentBin>0)
    {
        if (IsPrimary==kTRUE)
        {
            Answer = PrimarySpectrum->GetBinContent(CurrentBin);
        }
        else
        {
            Answer = FullSpectrum->GetBinContent(CurrentBin);
        }
    }
    
    // Ans, return it:
    return Answer;
}
        
// Options for extracting a score:
Double_t ScoringClass::GetScoreDivision(Double_t const Value)
{
    // Declare the answer:
    Double_t Answer = 0.0;
    
    // Extract both scores:
    Double_t PrimScore = GetHistScore(Value,kTRUE);
    Double_t FullScore = GetHistScore(Value,kFALSE);
    
    // Combine them:
    if ((PrimScore>0.0)&&(FullScore>0.0))
    {
        Answer = PrimScore/FullScore;
    }
    
    // return answer:
    return Answer;
}
    
// Control-functions:
void ScoringClass::GenerateSpectrum(Int_t const Nbins, Double_t const Xmin, Double_t const Xmax)
{
    // Generate empty histograms:
    TString FullName = Key + "_" + ScorerName + "_AllHits";
    if (IsCluster==kTRUE) {FullName = FullName + "_Clusters";} else {FullName = FullName + "_Signals";}
    FullSpectrum = new TH1D(FullName,FullName,Nbins,Xmin,Xmax);
    
    TString PrimaryName = Key + "_" + ScorerName + "_PrimaryHits"; 
    if (IsCluster==kTRUE) {PrimaryName = PrimaryName + "_Clusters";} else {PrimaryName = PrimaryName + "_Signals";}
    PrimarySpectrum = new TH1D(PrimaryName,PrimaryName,Nbins,Xmin,Xmax);
    
    for (Int_t k = 0; k<(Nbins+2); ++k)
    {
        FullSpectrum->SetBinContent(k,0.0);
        FullSpectrum->SetBinError(k,0.0);
        PrimarySpectrum->SetBinContent(k,0.0);
        PrimarySpectrum->SetBinError(k,0.0);
    }
}

void ScoringClass::WriteScorer()
{
    // Assign The OutputFile as the curent file:
    OutputFile->cd();
    
    // Next, simply write the entire object, as
    // we made this class inherit from TObject:
    TString WriteName = Key + "_" + ScorerName + "_Scorer";
    if (IsCluster==kTRUE) {WriteName = WriteName + "_Clusters";} else {WriteName = WriteName + "_Signals";}
    this->Write(WriteName.Data(),2);
    
    // However, we should also write the histograms separately:
    WriteName = FullSpectrum->GetName();
    FullSpectrum->Write(WriteName.Data(),2);
    
    WriteName = PrimarySpectrum->GetName();
    PrimarySpectrum->Write(WriteName.Data(),2);
}

void ScoringClass::ReadScorer()
{
    // Assign The OutputFile as the curent file:
    OutputFile->cd();
    
    // Next, we must duplicate the full class content.
    // Access the object:
    TString WriteName = Key + "_" + ScorerName + "_Scorer";
    if (IsCluster==kTRUE) {WriteName = WriteName + "_Clusters";} else {WriteName = WriteName + "_Signals";}
    ScoringClass* ReadClass = (ScoringClass*) OutputFile->Get(WriteName.Data());
    
    // Duplicate the errors:
    Int_t ErrorSize = ReadClass->Errors.size();
    Errors.resize(ErrorSize);
    PrintErrors = ReadClass->PrintErrors;
    
    for (Int_t k = 0; k<ErrorSize; ++k)
    {
        Errors.at(k) = ReadClass->Errors.at(k);
        cout << Errors.at(k) << "\n";
    }
    
    // Duplicate the rest:
    // NOTE: Name & OutputFile do not need to be dupliacted. they are OK already.
    
    // Retrieve the histograms:
    
    // Then, collect them:
    TString FullName = Key + "_" + ScorerName + "_AllHits";
    if (IsCluster==kTRUE) {FullName = FullName + "_Clusters";} else {FullName = FullName + "_Signals";}
    FullSpectrum = (TH1D*) OutputFile->Get(FullName.Data());
    
    TString PrimaryName = Key + "_" + ScorerName + "_PrimaryHits"; 
    if (IsCluster==kTRUE) {PrimaryName = PrimaryName + "_Clusters";} else {PrimaryName = PrimaryName + "_Signals";}
    PrimarySpectrum = (TH1D*) OutputFile->Get(PrimaryName.Data());
    
    // Done.
}

// Error messages:
void ScoringClass::ErrorMessage(TString const Message)
{
    TString Output = "### SCORINGCLASS ERROR ###: " + Message + "\n";
    if (PrintErrors==kTRUE) {cout << Output.Data();}
    Errors.push_back(Output);
}

void ScoringClass::PrintAllErrors()
{
    for (Int_t k = 0; k<Errors.size(); ++k)
    {
        cout << Errors.at(k).Data();
    }
}

void ScoringClass::DisableErrorPrinting()
{
    PrintErrors = kFALSE;
}

void ScoringClass::EnableErrorPrinting()
{
    PrintErrors = kTRUE;
}
    
Bool_t ScoringClass::ContainsNoErrors()
{
    if (Errors.size()==0) {return kTRUE;}
    else {return kFALSE;}
}

void ScoringClass::SetScaleFactor(Double_t const scale)
{
    if (scale>0.0) {ScaleFactor = scale;}
}

// Generation of a ROOT-dictionary:
ClassImp(ScoringClass)


