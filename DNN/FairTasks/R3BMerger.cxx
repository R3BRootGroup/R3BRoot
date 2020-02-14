// Every CXX-file includes its own header file:
#include "R3BMerger.h"

// Default constructor definition:
R3BMerger::R3BMerger(Int_t const nBran, TString* const Name, TString* const Title) : FairTask("R3BMerger")
{
    // Set default verbosity level:
    fVerbose = 1;

    // Create the single TConesArray:
    TheOutputFile = 0;
    nBranches = nBran;
    BranchName = Name;
    BranchTitle = Title;
    fArrays = new TClonesArray*[nBranches];
    OutputNameTag = "";
    
    if (nBranches>0)
    {
        for (Int_t k = 0; k<nBranches; ++k)
        {
            fArrays[k] = new TClonesArray(BranchName[k].Data());
        }
    }
}

// Destructor definition:
R3BMerger::~R3BMerger()
{
    // Delete all arrays:
    delete[] fArrays;
    
    // Do not delete the Name & Title arrays, since they were provided externally.
}

// FairTask initialization function:
InitStatus R3BMerger::Init()
{
    // Check that we supplied a sensible nr. of branches:
    if (nBranches<=0) {cout << "R3BMerger::Init: We need a positive number of branches!\n\n"; return kFATAL;}
    
    // Obtain the I/O manager:
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman)
    {
        cout << "I/O-manager FATAL: R3BMerger::Init: No FairRootManager\n\n";
        return kFATAL;
    }
    TheOutputFile = ioman->GetOutFile();
    
    // Obtain all arrays:
    for (Int_t k = 0; k<nBranches; ++k)
    {
        if ((TClonesArray*)ioman->GetObject((BranchTitle[k]+OutputNameTag).Data()) == nullptr)
        {
            cout << "I/O-manager FATAL: R3BMerger::Init No " << BranchTitle[k].Data() << " found!\n\n";
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
void R3BMerger::Exec(Option_t *option)
{
    // NOTE: Since FairTask will automatically load the inputs into the TClonesArrays,
    // and since those same arrays are also registered, this function needs no content at all.
    
    // We just log the progress:
    EventCounter = EventCounter + 1;
    if (EventCounter%1000==0) {cout << "Merger Log: We processed " << EventCounter << " events.\n";}
}
        
// Definition of Finish-function:
void R3BMerger::Finish()
{
    // Select TheOutputFile:
    TheOutputFile->cd();
}

// Generate ROOT dictionary:
ClassImp(R3BMerger);
