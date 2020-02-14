// Every CXX-file includes its own header file:
#include "R3BMultReader.h"

// Default constructor definition:
R3BMultReader::R3BMultReader() : FairTask("R3BMultReader")
{
    // Set default verbosity level:
    fVerbose = 1;

    // Create arrays:
    fArrayMults = new TClonesArray("ObjInteger");    
    fArraySignals = new TClonesArray("R3BSignal");
    fArrayClusters = new TClonesArray("R3BSignalCluster");
        
    // Other variables:
    OutputPath = "./";
    EventCounter = 0;         
    TextFileCounter = 0;
    nEvents = 1;
    Inputs = 0;
    TextFile_IsThere = kFALSE;
    
    // Initialize Nuclear dataBase:
    TheNuclei = new Nuclei();
    TheNuclei->BuildDataBase();
    TheOutputFile = 0;
}

// Destructor definition:
R3BMultReader::~R3BMultReader()
{
    // Delete the arrays:
    if(fArrayMults) {fArrayMults->Clear(); delete fArrayMults;}
    if(fArraySignals) {fArraySignals->Clear(); delete fArraySignals;}
    if(fArrayClusters) {fArrayClusters->Clear(); delete fArrayClusters;}

    // Delete other objects:
    delete TheNuclei;
}

// FairTask initialization function:
InitStatus R3BMultReader::Init()
{
    // Obtain the I/O manager:
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman)
    {
        cout << "I/O-manager FATAL: R3BMultReader::Init: No FairRootManager\n\n";
        return kFATAL;
    }
    TheOutputFile = ioman->GetOutFile();
    
    // Check the inputs class:
    if (Inputs->IsLinked()==kFALSE)
    {
        cout << "I/O-manager FATAL: R3BMultReader: The R3BInputClass is not linked to a file!\n\n";
        return kFATAL;
    }
    
    // Obtain all required inputs:
    OutputPath = Inputs->GetInputString("TheOutputPath");
    
    // Retrieve the NeuLAND signals:
    if ((TClonesArray*)ioman->GetObject("Signals") == nullptr)
    {
        cout << "I/O-manager FATAL: R3BMultReader::Init No Signals!\n\n";
        return kFATAL;
    }
    fArraySignals = (TClonesArray*)ioman->GetObject("Signals");
    
    // Also retrieve the clusters:
    if ((TClonesArray*)ioman->GetObject("Clusters") == nullptr)
    {
        cout << "I/O-manager FATAL: R3BMultReader::Init No R3BSignalClusters!\n\n";
        return kFATAL;
    }
    fArrayClusters = (TClonesArray*)ioman->GetObject("Clusters");
    
    // However, we do need to read from the DNN output TextFile.
    TString Name = OutputPath + "/PredictedMultiplicities.txt";
    DNN_Output_TextFile.open(Name.Data());
    
    if (!DNN_Output_TextFile.is_open())
    {
        cout << "\n\n ### FATAL ERROR: Ÿou do need to generate the .txt-file with multiplicities before you can run this code!\n\n";
        TextFile_IsThere = kFALSE;
    }
    else
    {
        TextFile_IsThere = kTRUE;
    }
    
    // Register the multiplicity into a ROOT-file:
    ioman->Register("DNN_Multiplicity","ObjInteger",fArrayMults,kTRUE);
    
    // Then, return the succes statement:
    if (Inputs->ContainsNoErrors()==kFALSE) {Inputs->PrintAllErrors(); return kFATAL;}
    EventCounter = 0;
    TextFileCounter = 0;
    return kSUCCESS;
}

// FairTask Execute function:
void R3BMultReader::Exec(Option_t *option)
{
    // Reset the current multiplicity array:
    fArrayMults->Clear();
    
    // Read the current multiplicity:
    std::string ThisLine;
    Int_t CurrentMultiplicity = 1;
    if (TextFile_IsThere==kTRUE)
    {
        DNN_Output_TextFile >> CurrentMultiplicity;
    }

    // Move on to the next line:
    if (TextFile_IsThere==kTRUE)
    {
        if (std::getline(DNN_Output_TextFile,ThisLine)) {TextFileCounter = TextFileCounter + 1;}
    }
    
    // Adjust multiplicity in the case of no signals:
    if (fArrayClusters->GetEntries()==0) {CurrentMultiplicity = 0;}
    
    // And also in the case of 1 signal:
    if (fArrayClusters->GetEntries()==1) {CurrentMultiplicity = 1;}
    
    // Store the multiplicity:
    new ((*fArrayMults)[0]) ObjInteger("DNN_Multiplicity",CurrentMultiplicity);

    // Log progress:
    EventCounter = EventCounter + 1;
    if (EventCounter%1000==0) {cout << "R3BMultReader Log: We processed " << EventCounter << " events.\n";}
    
    // when the two counters are not in parallel, give an error message:
    if (EventCounter!=TextFileCounter)
    {
        cout << "At Event = " << EventCounter << ", the two counters are no longer parallel!\n";
    }
}
        
// Definition of Finish-function:
void R3BMultReader::Finish()
{
    // close the TextFile:
    if (TextFile_IsThere==kTRUE)
    {
        DNN_Output_TextFile.close();
    }
    
    // Check whether everything worked:
    if (EventCounter!=TextFileCounter)
    {
        cout << "\n\n";
        cout << " ==> ### FATAL ERROR: ### <== \n";
        cout << "Somewhere during execution, the two counters were no longer parallel!\n";
        cout << "That means we cannot be sure that the right multiplicity was assigned to each event!\n";
        cout << "Hence, the output is WORTHLESS and the code should be re-checked!\n\n\n";
    }
}

// Generate ROOT dictionary:
ClassImp(R3BMultReader);
