// Every CXX-file includes its own header file:
#include "R3BPhysListMerger.h"

// Default constructor definition:
R3BPhysListMerger::R3BPhysListMerger(Int_t const nBran, TString* const Name, TString* const Title) : FairTask("R3BPhysListMerger")
{
    // Set default values:
    fVerbose = 1;
    
    fArray_MCTrack = 0;
    fArray_NeulandPoints = 0;
    fArray_PrimNeutronPoints = 0;
    fArray_NEBULAPoints = 0;
    fArray_VETOPoints = 0;
    fArray_NEBVETOPoints = 0;
    
    EventCounter = 0;
    nEvents = 1;
    nPhysicsLists = 1;
    
    UseVETO = kFALSE;
    UseNEBULA = kFALSE;
    UseNEBVETO = kFALSE;
    NeuLAND_IsOriginal = kTRUE;
    
    TheNuclei = new Nuclei();
    TheNuclei->BuilDataBase();
}

// Destructor definition:
R3BPhysListMerger::~R3BPhysListMerger()
{
    // Delete all arrays:
    delete[] fArray_MCTrack;
    delete[] fArray_NeulandPoints;
    delete[] fArray_PrimNeutronPoints;
    delete[] fArray_NEBULAPoints;
    delete[] fArray_VETOPoints;
    delete[] fArray_NEBVETOPoints;
    
    // Delete the rest:
    delete TheNuclei;
}

// FairTask initialization function:
InitStatus R3BPhysListMerger::Init()
{
    // Check input class first:
    if (Inputs->IsLinked()==kFALSE)
    {
        cout << "I/O-manager FATAL: The R3BInputClass is not linked to a file!\n\n";
        return kFATAL;
    }
    
    // Retrieve the required inputs:
    UseVETO = Inputs->GetInputBoolian("VETO_Include_in_SETUP");
    UseNEBULA = Inputs->GetInputBoolian("NEBULA_Include_in_SETUP");
    UseNEBVETO = Inputs->GetInputBoolian("NEBULA_VET_Include_in_SETUP");
    NeuLAND_IsOriginal = Inputs->GetInputBoolian("NeuLAND_Use_Original");
    
    // Create the TClonesArrays*
    fArray_MCTrack = new TClonesArray*[nPhysicsLists+1];
    fArray_NeulandPoints = new TClonesArray*[nPhysicsLists+1];
    fArray_PrimNeutronPoints = new TClonesArray*[nPhysicsLists+1];
    if (UseNEBULA==kTRUE) {fArray_NEBULAPoints = new TClonesArray*[nPhysicsLists+1];}
    if (UseVETO==kTRUE) {fArray_VETOPoints = new TClonesArray*[nPhysicsLists+1];}
    if (UseNEBVETO==kTRUE) {fArray_NEBVETOPoints = new TClonesArray*[nPhysicsLists+1];}
    
    for (Int_t kphys = 0; kphys<(nPhysicsLists+1); ++kphys)
    {
        fArray_MCTrack = new TClonesArray("R3BMCTrack");
        
        if (NeuLAND_IsOriginal==kTRUE)
        {
            fArray_NeulandPoints = new TClonesArray("R3BNeulandPoint");
        }
        else
        {
            fArray_NEBULAPoints = new TClonesArray("R3BCADNeulandPoint");
        }
        
        fArray_PrimNeutronPoints = new TClonesArray("FairMCPoint");
        
        if (UseNEBULA==kTRUE) {fArray_NEBULAPoints = new TClonesArray("R3BCADNEBULAPoint");}
        if (UseVETO==kTRUE) {fArray_VETOPoints = new TClonesArray("R3BCADVETOPoint");}
        if (UseNEBVETO==kTRUE) {fArray_NEBVETOPoints = new TClonesArray("R3BCADNEBULAVETOPoint");}
    }
    
    // Obtain the I/O manager:
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman)
    {
        cout << "I/O-manager FATAL: R3BPhysListMerger::Init: No FairRootManager\n\n";
        return kFATAL;
    }
    TheOutputFile = ioman->GetOutFile();
    
    // Obtain all arrays:
    for (Int_t k = 0; k<nBranches; ++k)
    {
        if ((TClonesArray*)ioman->GetObject((BranchTitle[k]+OutputNameTag).Data()) == nullptr)
        {
            cout << "I/O-manager FATAL: R3BPhysListMerger::Init No " << BranchTitle[k].Data() << " found!\n\n";
            return kFATAL;
        }
        
        fArrays[k] = (TClonesArray*) ioman->GetObject((BranchTitle[k]+OutputNameTag).Data());
    }
    
    // Now register those same arrays:
    for (Int_t k = 0; k<nBranches; ++k)
    {
        ioman->Register((BranchTitle[k]+OutputNameTag).Data(),BranchName[k].Data(),fArrays[k],kTRUE);
    }
    
    // That's it. This should do nothing more then copying the data.
    // return succes statement:
    EventCounter = 0;
    return kSUCCESS;
}

// FairTask Execute function:
void R3BPhysListMerger::Exec(Option_t *option)
{
    // NOTE: Since FairTask will automatically load the inputs into the TClonesArrays,
    // and since those same arrays are also registered, this function needs no content at all.
    
    // We just log the progress:
    EventCounter = EventCounter + 1;
    if (EventCounter%1000==0) {cout << "Merger Log: We processed " << EventCounter << " events.\n";}
}
        
// Definition of Finish-function:
void R3BPhysListMerger::Finish()
{
    // Select TheOutputFile:
    TheOutputFile->cd();
}

// Generate ROOT dictionary:
ClassImp(R3BPhysListMerger);
