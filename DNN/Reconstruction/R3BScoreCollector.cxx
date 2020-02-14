// Every CXX-file includes its own header file:
#include "R3BScoreCollector.h"

// Default constructor definition:
R3BScoreCollector::R3BScoreCollector() : FairTask("R3BScoreCollector")
{
    // Set default verbosity level:
    fVerbose = 1;

    // Create the TClonesArrays:
    fArraySignals = new TClonesArray("R3BSignal");
    fArrayClusters = new TClonesArray("R3BSignalCluster");
    fArrayNeuMult = new TClonesArray("ÖbjInteger");
    fArrayGunMult = new TClonesArray("ÖbjInteger");
    fArrayDetMult = new TClonesArray("ÖbjInteger");
    
    // Set parameters:
    nEvents = 1;
    EventCounter = 0;
    Calibration = 0;
    Calibration_Total = 0;
    MaxMultiplicity = 1;
    SimulationData_IsAvailable = kTRUE;
    
    // Create the auxillary classes:
    Inputs = 0;
    TheNuclei = new Nuclei();
    TheNuclei->BuildDataBase();
    TheScorers = new AllScorers();
}

// Destructor definition:
R3BScoreCollector::~R3BScoreCollector()
{
    // Delete all arrays:
    if(fArraySignals) {fArraySignals->Clear(); delete fArraySignals;}
    if(fArrayClusters) {fArrayClusters->Clear(); delete fArrayClusters;}
    if(fArrayNeuMult) {fArrayNeuMult->Clear(); delete fArrayNeuMult;}
    if(fArrayGunMult) {fArrayGunMult->Clear(); delete fArrayGunMult;}
    if(fArrayDetMult) {fArrayDetMult->Clear(); delete fArrayDetMult;}
    
    // Delete other classes:
    delete TheNuclei;
    delete TheScorers;
}

// FairTask initialization function:
InitStatus R3BScoreCollector::Init()
{    
    // Obtain the I/O manager:
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman)
    {
        cout << "I/O-manager FATAL: R3BScoreCollector::Init: No FairRootManager\n\n";
        return kFATAL;
    }
    TheOutputFile = ioman->GetOutFile();
    
    // Check the inputs class:
    if (Inputs->IsLinked()==kFALSE)
    {
        cout << "I/O-manager FATAL: The R3BInputClass is not linked to a file!\n\n";
        return kFATAL;
    }
    
    // Obtain the required inputs:
    MaxMultiplicity = Inputs->GetInputInteger("ParticleGun_Multiplicity");
    SimulationData_IsAvailable = Inputs->GetInputBoolian("NeuLAND_TranslateToSignals_Mark_TruePrimarySignals");
    
    // Obtain the R3BSignals:
    if ((TClonesArray*)ioman->GetObject("Signals") == nullptr)
    {
        cout << "I/O-manager FATAL: R3BScoreCollector::Init No Signals!\n\n";
        return kFATAL;
    }
    fArraySignals = (TClonesArray*)ioman->GetObject("Signals");
    
    // Obtain the R3BSignalClusters:
    if ((TClonesArray*)ioman->GetObject("Clusters") == nullptr)
    {
        cout << "I/O-manager FATAL: R3BScoreCollector::Init No Clusters!\n\n";
        return kFATAL;
    }
    fArrayClusters = (TClonesArray*)ioman->GetObject("Clusters");
    
    // Obtain the multiplicities:
    if (SimulationData_IsAvailable==kTRUE)
    {
        if ((TClonesArray*)ioman->GetObject("NeuLAND_Multiplicity") == nullptr)
        {
            cout << "I/O-manager FATAL: R3BMultMatrix::Init No NeuLAND_Multiplicity!\n\n";
            return kFATAL;
        }
        fArrayNeuMult = (TClonesArray*)ioman->GetObject("NeuLAND_Multiplicity");
    
        if ((TClonesArray*)ioman->GetObject("Gun_Multiplicity") == nullptr)
        {
            cout << "I/O-manager FATAL: R3BMultMatrix::Init No Gun_Multiplicity!\n\n";
            return kFATAL;
        }
        fArrayGunMult = (TClonesArray*)ioman->GetObject("Gun_Multiplicity");
  
        if ((TClonesArray*)ioman->GetObject("Detected_Multiplicity") == nullptr)
        {
            cout << "I/O-manager FATAL: R3BMultMatrix::Init No Detected_Multiplicity!\n\n";
            return kFATAL;
        }
        fArrayDetMult = (TClonesArray*)ioman->GetObject("Detected_Multiplicity");
    }
  
    // Next, initialize the scoring class:
    TheScorers->LinkInputsClass(Inputs);
    Bool_t ScoreTest = TheScorers->Initialize();
    
    // Also initialize the calibration histograms:
    Double_t EBeamPerN = Inputs->GetAvgBeamEnergyPerNucleon();
    Double_t Eboundary = EBeamPerN*((Int_t) MaxMultiplicity);
    Int_t Ebins = (Int_t) (Eboundary/10.0);
    Int_t ClusBins = MaxMultiplicity*12;
    TString st = "";
    TString kstr = "";
    Calibration = new TH2D*[MaxMultiplicity+1];
    Calibration_Total = new TH2D("TDR_Multiplicity_Calibration_Total","TDR_Multiplicity_Calibration_Total",Ebins*10,0.0,Eboundary,ClusBins+1,0.0,((Int_t) (ClusBins+1)));
    
    for (Int_t k = 0; k<(MaxMultiplicity+1); ++k)
    {
        kstr = st.Itoa(k,10);
        Calibration[k] = new TH2D("TDR_Multiplicity_Calibration_"+kstr,"TDR_Multiplicity_Calibration_"+kstr,Ebins*10,0.0,Eboundary,ClusBins+1,0.0,((Int_t) (ClusBins+1)));
    }
    
    // Check for errors:
    if (Inputs->ContainsNoErrors()==kFALSE) {Inputs->PrintAllErrors(); return kFATAL;}
    if (TheScorers->ContainsNoErrors()==kFALSE) {TheScorers->PrintAllErrors(); return kFATAL;}
    if (ScoreTest==kFALSE) {std::cout << "\n\nAllScorers Initialization Failed!\n\n"; return kFATAL;}
    
    // Return the succes statement:
    EventCounter = 0;
    return kSUCCESS;
}

// FairTask Execute function:
void R3BScoreCollector::Exec(Option_t *option)
{
    // NOTE: Since FairTask will automatically load the inputs into the TClonesArrays,
    // and since those same arrays are also registered, this function needs no content at all.
    
    // Obtain the NeuLAND multiplicity:
    Int_t NeuLANDMultiplicity = 0;
    Int_t GunMultiplicity = 0;
    Int_t DetMultiplicity = 0;
    
    if (SimulationData_IsAvailable==kTRUE)
    {
        ObjInteger* ThisObjInt;
        ThisObjInt = (ObjInteger*) fArrayNeuMult->At(0);
        NeuLANDMultiplicity = ThisObjInt->GetNumber();
    
        ThisObjInt = (ObjInteger*) fArrayGunMult->At(0);
        GunMultiplicity = ThisObjInt->GetNumber();
    
        ThisObjInt = (ObjInteger*) fArrayDetMult->At(0);
        DetMultiplicity = ThisObjInt->GetNumber();
    }
    
    // Loop over the signals and update the scorers:
    Int_t nSignals = fArraySignals->GetEntries();
    R3BSignal* ThisSignal;
    
    for (Int_t ksignal = 0; ksignal<nSignals; ++ksignal)
    {
        ThisSignal = (R3BSignal*) fArraySignals->At(ksignal);
        TheScorers->UpdateSignalScorers(ThisSignal);
    }
    
    // Same for the clusters:
    Int_t nClusters = fArrayClusters->GetEntries();
    R3BSignalCluster* ThisCluster;
    Int_t DetectedMultiplicity = 0;
    Double_t Total_Energy = 0.0;
    
    for (Int_t kcluster = 0; kcluster<nClusters; ++kcluster)
    {
        ThisCluster = (R3BSignalCluster*) fArrayClusters->At(kcluster);
        DetectedMultiplicity = DetectedMultiplicity + ThisCluster->GetPrimaryMultSim();
        Total_Energy = Total_Energy + ThisCluster->GetEnergy();
        TheScorers->UpdateClusterScorers(ThisCluster);
    }
    
    // Fill calibration histograms:
    if (DetectedMultiplicity<0) {DetectedMultiplicity = 0;}
    if (DetectedMultiplicity>MaxMultiplicity) {DetectedMultiplicity = MaxMultiplicity;}
    Calibration[DetectedMultiplicity]->Fill(Total_Energy,(Int_t) nClusters);
    Calibration_Total->Fill(Total_Energy,(Int_t) nClusters);
    
    // Then, log the progress:
    EventCounter = EventCounter + 1;
    if (EventCounter%1000==0) {cout << "R3BScoreCollector Log: We processed " << EventCounter << " events.\n";}
}
        
// Definition of Finish-function:
void R3BScoreCollector::Finish()
{   
    // Write histograms:
    TheOutputFile->cd();
    TString st = "";
    TString kstr = "";
    
    for (Int_t k = 0; k<(MaxMultiplicity+1); ++k) 
    {
        kstr = st.Itoa(k,10); 
        Calibration[k]->Write("TDR_Calibration_"+kstr,2);
    }
    
    Calibration_Total->Write("TDR_Calibration_Total",2);
    
    // Write the obtained scorers to their respective output file:
    TheScorers->WriteScorers();
    
    // Next, make the plots:
    TheScorers->MakePlots();
    
    // Done.
}

// Generate ROOT dictionary:
ClassImp(R3BScoreCollector);
