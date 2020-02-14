// Every CXX-file includes its own header file:
#include "R3BNEBULAScoreCollector.h"

// Default constructor definition:
R3BNEBULAScoreCollector::R3BNEBULAScoreCollector() : FairTask("R3BNEBULAScoreCollector")
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
    MaxMultiplicity = 1;
    UseNEBULA = kFALSE;
    SimulationData_IsAvailable = kTRUE;
    
    // Create the auxillary classes:
    Inputs = 0;
    TheNuclei = new Nuclei();
    TheNuclei->BuildDataBase();
    TheScorers = new AllScorers();
}

// Destructor definition:
R3BNEBULAScoreCollector::~R3BNEBULAScoreCollector()
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
InitStatus R3BNEBULAScoreCollector::Init()
{    
    // Obtain the I/O manager:
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman)
    {
        cout << "I/O-manager FATAL: R3BNEBULAScoreCollector::Init: No FairRootManager\n\n";
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
    UseNEBULA = Inputs->GetInputBoolian("NEBULA_Include_in_SETUP");
    SimulationData_IsAvailable = Inputs->GetInputBoolian("NeuLAND_TranslateToSignals_Mark_TruePrimarySignals");
    if (UseNEBULA==kFALSE) {cout << " ==> NEBULA must be in the setup!!!\n\n"; return kFATAL;}
    
    // Obtain the R3BSignals:
    if ((TClonesArray*)ioman->GetObject("NEBULASignals") == nullptr)
    {
        cout << "I/O-manager FATAL: R3BNEBULAScoreCollector::Init No NEBULASignals!\n\n";
        return kFATAL;
    }
    fArraySignals = (TClonesArray*)ioman->GetObject("NEBULASignals");
    
    // Obtain the R3BSignalClusters:
    if ((TClonesArray*)ioman->GetObject("NEBULAClusters") == nullptr)
    {
        cout << "I/O-manager FATAL: R3BNEBULAScoreCollector::Init No NEBULAClusters!\n\n";
        return kFATAL;
    }
    fArrayClusters = (TClonesArray*)ioman->GetObject("NEBULAClusters");
    
    // Obtain the multiplicities:
    if (SimulationData_IsAvailable==kTRUE)
    {
        if ((TClonesArray*)ioman->GetObject("NEBULA_Multiplicity") == nullptr)
        {
            cout << "I/O-manager FATAL: R3BNEBULAScoreCollector::Init No NEBULA_Multiplicity!\n\n";
            return kFATAL;
        }
        fArrayNeuMult = (TClonesArray*)ioman->GetObject("NEBULA_Multiplicity");
    
        if ((TClonesArray*)ioman->GetObject("Gun_Multiplicity") == nullptr)
        {
            cout << "I/O-manager FATAL: R3BNEBULAScoreCollector::Init No Gun_Multiplicity!\n\n";
            return kFATAL;
        }
        fArrayGunMult = (TClonesArray*)ioman->GetObject("Gun_Multiplicity");
  
        if ((TClonesArray*)ioman->GetObject("NEBULA_Detected_Multiplicity") == nullptr)
        {
            cout << "I/O-manager FATAL: R3BNEBULAScoreCollector::Init No NEBULA_Detected_Multiplicity!\n\n";
            return kFATAL;
        }
        fArrayDetMult = (TClonesArray*)ioman->GetObject("NEBULA_Detected_Multiplicity");
    }
  
    // Next, initialize the scoring class:
    TheScorers->SetDetector("NEBULA");
    TheScorers->LinkInputsClass(Inputs);
    Bool_t ScoreTest = TheScorers->Initialize();
    
    // Check for errors:
    if (Inputs->ContainsNoErrors()==kFALSE) {Inputs->PrintAllErrors(); return kFATAL;}
    if (TheScorers->ContainsNoErrors()==kFALSE) {TheScorers->PrintAllErrors(); return kFATAL;}
    if (ScoreTest==kFALSE) {std::cout << "\n\nAllScorers Initialization Failed!\n\n"; return kFATAL;}
    
    // Return the succes statement:
    EventCounter = 0;
    return kSUCCESS;
}

// FairTask Execute function:
void R3BNEBULAScoreCollector::Exec(Option_t *option)
{
    // NOTE: Since FairTask will automatically load the inputs into the TClonesArrays,
    // and since those same arrays are also registered, this function needs no content at all.
    
    // Obtain the NEBULA multiplicity:
    Int_t NEBULAMultiplicity = 0;
    Int_t GunMultiplicity = 0;
    Int_t DetMultiplicity = 0;
    
    if (SimulationData_IsAvailable==kTRUE)
    {
        ObjInteger* ThisObjInt;
        ThisObjInt = (ObjInteger*) fArrayNeuMult->At(0);
        NEBULAMultiplicity = ThisObjInt->GetNumber();
    
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
    
    // Then, log the progress:
    EventCounter = EventCounter + 1;
    if (EventCounter%1000==0) {cout << "R3BNEBULAScoreCollector Log: We processed " << EventCounter << " events.\n";}
}
        
// Definition of Finish-function:
void R3BNEBULAScoreCollector::Finish()
{   
    // Select OutputFile:
    TheOutputFile->cd();
    
    // Write the obtained scorers to their respective output file:
    TheScorers->WriteScorers();
    
    // Next, make the plots:
    TheScorers->MakePlots();
    
    // Done.
}

// Generate ROOT dictionary:
ClassImp(R3BNEBULAScoreCollector);
