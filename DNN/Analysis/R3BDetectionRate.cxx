// Every CXX-file includes its own header file:
#include "R3BDetectionRate.h"

// Default constructor definition:
R3BDetectionRate::R3BDetectionRate() : FairTask("R3BDetectionRate")
{
    // Set default verbosity level:
    fVerbose = 1;

    // Create arrays:
    fArraySignals = new TClonesArray("R3BSignal");
  
    // Set other class values:
    nEvents = 1;
    EventCounter = 0;
    nMeasurementPoints = 1;
    Estep = 1.0;
    NbinsE = 50;
    Range = 150.0;
    NeuLAND_Total_BarLength = 250.0;
    NeuLAND_Total_BarWidth = 5.0;
    Randomize_BarPositions = kFALSE;
    FileName = "./TheGraph.root";
    nPaddlesPerPlane = 1;
    TheOutputFile = 0;
    
    // Build nuclear database:
    TheNuclei = new Nuclei();
    TheNuclei->BuildDataBase();
}

// Destructor definition:
R3BDetectionRate::~R3BDetectionRate()
{
    // Delete the arrays:
    if(fArraySignals) {fArraySignals->Clear(); delete fArraySignals;}
    
    // delete other stuff:
    delete TheNuclei;
    delete EnergySpread;
}

// FairTask initialization function:
InitStatus R3BDetectionRate::Init()
{
    // Obtain the I/O manager:
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman)
    {
        cout << "I/O-manager FATAL: R3BDetectionRate::Init: No FairRootManager\n\n";
        return kFATAL;
    }
    // Do not collect the outputfile, because this clas is not specified with
    // an outputfile. // TheOutputFile = ioman->GetOutFile();
    
    // Obtain inputs:
    if (Inputs->IsLinked()==kFALSE)
    {
        cout << "I/O-manager FATAL: R3BInputClass: not linked to a file!\n\n";
        return kFATAL;
    }
    
    nMeasurementPoints = Inputs->GetInputInteger("NeuLAND_JanGraphs_PhysList_nPoints");
    Estep = Inputs->GetInputDouble("NeuLAND_JanGraphs_PhysList_Estep","MeV");
    FileName = Inputs->GetInputString("TheOutputPath") + Inputs->GetInputString("NeuLAND_Distributions");
    nPaddlesPerPlane = Inputs->GetInputInteger("NeuLAND_Number_of_Bars_per_plane");
    
    // Obtain R3BSignals:
    if ((TClonesArray*)ioman->GetObject("Signals") == nullptr)
    {
        cout << "I/O-manager FATAL: R3BDetectionRate::Init No R3BSignals!\n\n";
        return kFATAL;
    }
    fArraySignals = (TClonesArray*)ioman->GetObject("Signals");
    
    // Define measurement array:
    RateCounters = new Int_t[nMeasurementPoints];
    
    for (Int_t k = 0; k<nMeasurementPoints; ++k)
    {
        RateCounters[k] = 0;
    }
    
    // Define control histograms:
    Randomize_BarPositions = Inputs->GetInputBoolian("NeuLAND_EnergySpreadPlot_RandomizeBarPositions");
    NbinsE = Inputs->GetInputInteger("NeuLAND_EnergySpreadPlot_NBins");
    NeuLAND_Total_BarLength = Inputs->GetInputDouble("NeuLAND_Total_BarLength","cm");
    NeuLAND_Total_BarWidth = Inputs->GetInputDouble("NeuLAND_AluminiumFrame_Width","cm");
    NeuLAND_Distance = Inputs->GetInputDouble("NeuLAND_front_z_position","cm");
    Range = 0.6*NeuLAND_Total_BarLength;
    EnergySpread = new TH2D("EnergySpread","EnergySpread",NbinsE,-1.0*Range,Range,NbinsE,-1.0*Range,Range);
    PolarSpread = new TH1D("PolarSpread","PolarSpread",NbinsE,0.0,Range);
    
    for (Int_t kx = 0; kx<(NbinsE+2); ++kx)
    {
        for (Int_t ky = 0; ky<(NbinsE+2); ++ky)
        {
            EnergySpread->SetBinContent(kx,ky,0.0);
            EnergySpread->SetBinError(kx,ky,0.0);
        }
        
        PolarSpread->SetBinContent(kx,0.0);
        PolarSpread->SetBinError(kx,0.0);
    }
    
    // Then, return the succes statement:
    if (Inputs->ContainsNoErrors()==kFALSE) {Inputs->PrintAllErrors(); return kFATAL;}
    EventCounter = 0;
    Generator = new TRandom3(0);
    return kSUCCESS;
}

// FairTask Initialization function:
void R3BDetectionRate::Exec(Option_t *option)
{
    // NOTE: FairTask will automatically load the inputs into the TClonesArrays.
    
    // Declare what we need:
    Int_t nSignals = fArraySignals->GetEntries();
    Double_t Edep_Total = 0.0;
    Double_t ThisThreshold = 0.0;
    Double_t Edep = 0.0;
    Double_t xx = 0.0;
    Double_t yy = 0.0;
    Double_t zz = 0.0;
    Double_t rr = 0.0;
    Double_t BinContent = 0.0;
    Double_t AngleWeight = 0.0;
    Int_t xbin = 0;
    Int_t ybin = 0;
    Int_t rbin = 0;
    Bool_t Horizontal;
    R3BSignal* ThisSignal;
    
    // Loop over all signals:
    for (Int_t k = 0; k<nSignals; ++k)
    {
        // Get the current signal:
        ThisSignal = (R3BSignal*) fArraySignals->At(k);
        
        // Get data:
        Edep = ThisSignal->GetEnergy();
        xx = ThisSignal->GetPositionX();
        yy = ThisSignal->GetPositionY();
        zz = ThisSignal->GetPositionZ();
        Horizontal = ThisSignal->IsHorPaddle("NeuLAND",nPaddlesPerPlane);
        
        // Add its energy:
        Edep_Total = Edep_Total + Edep;
        
        // Randomize:
        if (Randomize_BarPositions==kTRUE)
        {
            if (Horizontal==kTRUE) {yy = yy + Generator->Uniform(-0.5*NeuLAND_Total_BarWidth,0.5*NeuLAND_Total_BarWidth);}
            else {xx = xx + Generator->Uniform(-0.5*NeuLAND_Total_BarWidth,0.5*NeuLAND_Total_BarWidth);}
        }
        
        // Compute radius:
        rr = TMath::Sqrt(xx*xx + yy*yy);
        AngleWeight = 1.0/TMath::ATan(rr/NeuLAND_Distance);
        if (!((AngleWeight>0.0)&&(AngleWeight<1e99))) {AngleWeight = 0.0;}
        
        // Fill its energy to the histogram:
        xx = ((xx+Range)/(2.0*Range))*((Int_t) NbinsE) + 0.5;
        yy = ((yy+Range)/(2.0*Range))*((Int_t) NbinsE) + 0.5;
        rr = (rr/Range)*((Int_t) NbinsE) + 0.5;
        xbin = (Int_t) xx;
        ybin = (Int_t) yy;
        rbin = (Int_t) rr;
        
        BinContent = EnergySpread->GetBinContent(xbin,ybin);
        BinContent = BinContent + Edep;
        EnergySpread->SetBinContent(xbin,ybin,BinContent);
        
        BinContent = PolarSpread->GetBinContent(rbin);
        BinContent = BinContent + Edep*AngleWeight;
        PolarSpread->SetBinContent(rbin,BinContent);
    }
    
    // Next, if this energy passes the thresholds, count the event:
    for (Int_t ke = 0; ke<nMeasurementPoints; ++ke)
    {
        ThisThreshold = ((Int_t) ke)*Estep;
        
        if (Edep_Total>ThisThreshold)
        {
            RateCounters[ke] = RateCounters[ke] + 1;
        }
    }
    
    // Log progress:
    EventCounter = EventCounter + 1;
    if (EventCounter%1000==0) {cout << "NeuLANDTranslator Log: We processed " << EventCounter << " events.\n";}
}
        
// Definition of Finish-function:
void R3BDetectionRate::Finish()
{
    // Set to final file:
    
    // Design the final graph:
    Double_t* Xvalues = new Double_t[nMeasurementPoints];
    Double_t* Xerrors = new Double_t[nMeasurementPoints];
    Double_t* Yvalues = new Double_t[nMeasurementPoints];
    Double_t* Yerrors = new Double_t[nMeasurementPoints];
    
    for (Int_t k = 0; k<nMeasurementPoints; ++k)
    {
        Xvalues[k] = ((Int_t) k)*Estep;
        Xerrors[k] = 0.0;
        
        Yvalues[k] = 1.0/((Int_t) EventCounter);
        Yvalues[k] = Yvalues[k]*((Int_t) RateCounters[k]);
        
        Yerrors[k] = Yvalues[k]*(1.0/TMath::Sqrt((Int_t) EventCounter));
    }
    
    TGraphErrors* TheGraph = new TGraphErrors(nMeasurementPoints,&Xvalues[0],&Yvalues[0],&Xerrors[0],&Yerrors[0]);
    
    // Write this graph to the file:
    FileStat_t stom;
    Int_t test = gSystem->GetPathInfo(FileName.Data(),stom);
    TFile* f;
    
    if (test==0)
    {
        // Then the file already exists:
        f = new TFile(FileName.Data(),"update");
    }
    else
    {
        // recreate the file:
        f = new TFile(FileName.Data(),"recreate");
    }
    
    // write the histogram to the file:
    f->cd();
    
    TheGraph->Write("JanDetectionGraph",2);
    EnergySpread->Write("EnergySpread",2);
    PolarSpread->Write("PolarSpread",2);
    
    // Generate polar plot (simple one)
    
    // Close the file & delete everything:
    f->Close();
    
    delete f;
    delete TheGraph;
    delete Xvalues;
    delete Xerrors;
    delete Yvalues;
    delete Yerrors;
    delete RateCounters;
    // Done.
}

// Generate ROOT dictionary:
ClassImp(R3BDetectionRate);
