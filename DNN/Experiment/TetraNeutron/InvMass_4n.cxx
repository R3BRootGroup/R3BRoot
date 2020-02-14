// Every CXX-file includes its own header file:
#include "InvMass_4n.h"

// Default constructor definition:
InvMass_4n::InvMass_4n() : FairTask("InvMass_4n")
{
    // Set default verbosity level:
    fVerbose = 1;

    // Create arrays:
    fArrayMCNeutronTracks = new TClonesArray("TLorentzVector");
    fArrayNeuLANDSignals = new TClonesArray("R3BSignal");
    fArrayNeuLANDClusters = new TClonesArray("R3BSignalCluster");
    fArrayNeutronTracks = new TClonesArray("TLorentzVector"); 
    fArrayMass = new TClonesArray("MassValue");
        
    // Other variables:
    BranchTitle = "TheNeutronTracks";
    OutputPath = "./";
    EventCounter = 0;         
    nEvents = 1;
    Inputs = 0;
    SimulationData_IsAvailable = kFALSE;
    LimitToMC = kFALSE;
    
    // Initialize Nuclear dataBase:
    TheNuclei = new Nuclei();
    TheNuclei->BuildDataBase();
    TheOutputFile = 0;
    
    // Histograms:
    MassHistogram = 0;
}

// Destructor definition:
InvMass_4n::~InvMass_4n()
{
    // Delete the arrays:
    if (fArrayMCNeutronTracks) {fArrayMCNeutronTracks->Clear(); delete fArrayMCNeutronTracks;}
    if (fArrayNeuLANDSignals) {fArrayNeuLANDSignals->Clear(); delete fArrayNeuLANDSignals;}
    if (fArrayNeuLANDClusters) {fArrayNeuLANDClusters->Clear(); delete fArrayNeuLANDClusters;}
    if (fArrayNeutronTracks) {fArrayNeutronTracks->Clear(); delete fArrayNeutronTracks;}
    if (fArrayMass) {fArrayMass->Clear(); delete fArrayMass;}
    if (fArrayMCNeutronTracks) {fArrayMCNeutronTracks->Clear(); delete fArrayMCNeutronTracks;}

    // Delete other objects:
    delete TheNuclei;
    delete MassHistogram;
}

// FairTask initialization function:
InitStatus InvMass_4n::Init()
{
    // Obtain the I/O manager:
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman)
    {
        cout << "I/O-manager FATAL: InvMass_4n::Init: No FairRootManager\n\n";
        return kFATAL;
    }
    TheOutputFile = ioman->GetOutFile();
    
    // Check the inputs class:
    if (Inputs->IsLinked()==kFALSE)
    {
        cout << "I/O-manager FATAL: InvMass_4n: The R3BInputClass is not linked to a file!\n\n";
        return kFATAL;
    }
    
    // Obtain all required inputs:
    OutputPath = Inputs->GetInputString("TheOutputPath");
    SimulationData_IsAvailable = Inputs->GetInputBoolian("NeuLAND_TranslateToSignals_Mark_TruePrimarySignals");
    
    // Obtain the reconstructed Neutron Tracks:
    if ((TClonesArray*)ioman->GetObject(BranchTitle.Data()) == nullptr)
    {
        cout << "I/O-manager FATAL: InvMass_4n::Init() ==> Could not locate neutron tracks <" + BranchTitle + ">!\n\n";
        return kFATAL;
    }
    fArrayNeutronTracks = (TClonesArray*) ioman->GetObject(BranchTitle.Data());
    
    // Obtain NeuLAND signals:
    if ((TClonesArray*)ioman->GetObject("Signals") == nullptr)
    {
        cout << "I/O-manager FATAL: InvMass_4n::Init No Signals!\n\n";
        return kFATAL;
    }
    fArrayNeuLANDSignals = (TClonesArray*)ioman->GetObject("Signals");
    
    // Also retrieve the clusters:
    if ((TClonesArray*)ioman->GetObject("Clusters") == nullptr)
    {
        cout << "I/O-manager FATAL: R3BMultReader::Init No R3BSignalClusters!\n\n";
        return kFATAL;
    }
    fArrayNeuLANDClusters = (TClonesArray*)ioman->GetObject("Clusters");
    
    // Obtain the MC neutron tracks if they exist:
    if (SimulationData_IsAvailable==kTRUE)
    {
        // Retrieve the MC neutron tracks:
        if ((TClonesArray*)ioman->GetObject("MCNeutronTracks") == nullptr)
        {
            cout << "I/O-manager FATAL: InvMass_4n::Init No MCNeutronTracks!\n\n";
            return kFATAL;
        }
        fArrayMCNeutronTracks = (TClonesArray*)ioman->GetObject("MCNeutronTracks");
    }
    
    // Register the output:
    ioman->Register(BranchTitle+"_InvMass","MassValue",fArrayMass,kTRUE);
    
    // Create histogram:
    MassHistogram = new TH1D(BranchTitle+"_MassHistogram",BranchTitle+"_MassHistogram",400,-2.0,5.0);
    
    // Then, return the succes statement:
    if (Inputs->ContainsNoErrors()==kFALSE) {Inputs->PrintAllErrors(); return kFATAL;}
    EventCounter = 0;
    
    return kSUCCESS;
}

// FairTask Execute function:
void InvMass_4n::Exec(Option_t *option)
{
    // Reset the output array:
    fArrayMass->Clear("C");
    
    // Begin by testing if the multiplicity is 4.
    // Otherwise, the data is unsuited for a tetraneutron experiment.
    Int_t Multiplicity = fArrayNeutronTracks->GetEntries();
    Int_t CreationIndex = 0;
    
    Int_t TrueMultiplicity = -1;
    if (SimulationData_IsAvailable==kTRUE) {TrueMultiplicity = fArrayMCNeutronTracks->GetEntries();}
    
    // Do the multiplicity test:
    Bool_t MultiplicityTest = kTRUE;
    
    if ((LimitToMC==kFALSE)||((LimitToMC==kTRUE)&&(SimulationData_IsAvailable==kFALSE)))
    {
        if (Multiplicity==4) {MultiplicityTest = kTRUE;}
        else {MultiplicityTest = kFALSE;}
    }
    
    if ((SimulationData_IsAvailable==kTRUE)&&(LimitToMC==kTRUE))
    {
        if ((Multiplicity==4)&&(TrueMultiplicity==4)) {MultiplicityTest = kTRUE;}
        else {MultiplicityTest = kFALSE;}
    }
        
    // Now, decide whether we can use the event:
    if (MultiplicityTest==kTRUE)
    {
        // Then, we can move on. Extract all neutron tracks and add them up:
        TLorentzVector* Sumvector = new TLorentzVector();
        Sumvector->SetT(0.0);
        Sumvector->SetX(0.0);
        Sumvector->SetY(0.0);
        Sumvector->SetZ(0.0);
        
        TLorentzVector* NeutronTrack;
        
        for (Int_t k = 0; k<4; ++k)
        {
            NeutronTrack = (TLorentzVector*) fArrayNeutronTracks->At(k);
            Sumvector->SetT(Sumvector->T() + NeutronTrack->T());
            Sumvector->SetX(Sumvector->X() + NeutronTrack->X());
            Sumvector->SetY(Sumvector->Y() + NeutronTrack->Y());
            Sumvector->SetZ(Sumvector->Z() + NeutronTrack->Z());
        }
        
        // Next, compute invariant mass:
        Double_t TheInvMass = (Sumvector->T()*Sumvector->T()) - 
                              (Sumvector->X()*Sumvector->X() +
                               Sumvector->Y()*Sumvector->Y() +
                               Sumvector->Z()*Sumvector->Z());
        TheInvMass = TMath::Sqrt(TheInvMass);
        
        // Then, subtract the neutron masses:
        TheInvMass = TheInvMass - 4.0*TheNuclei->GetMass("neutron","MeV");
        
        // Done. write the output:
        new ((*fArrayMass)[CreationIndex]) MassValue(TheInvMass);
        CreationIndex = CreationIndex + 1;
        
        // And also fill the histograms:
        MassHistogram->Fill(TheInvMass);
        
        // Done. Now delete everything:
        delete Sumvector;
    }
    
    // Log progress:
    EventCounter = EventCounter + 1;
    if (EventCounter%1000==0) {cout << "InvMass_4n Log: We processed " << EventCounter << " events.\n";}
}

// Definition of Finish-function:
void InvMass_4n::Finish()
{
    // Select OutputFile:
    TheOutputFile->cd();
    
    // write histograms:
    MassHistogram->Write(BranchTitle+"_MassHistogram",2);
}

// Generate ROOT dictionary:
ClassImp(InvMass_4n);
