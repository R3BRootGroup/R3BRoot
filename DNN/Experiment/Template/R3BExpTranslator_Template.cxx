// Every CXX-file includes its own header file:
#include "R3BExpTranslator_Template.h"

// Default constructor definition:
R3BExpTranslator_Template::R3BExpTranslator_Template() : FairTask("R3BExpTranslator_Template")
{
    // Set default verbosity level:
    fVerbose = 1;

    // Create output array:
    fArraySignals = new TClonesArray("R3BSignal");
    fArrayNEBULASignals = new TClonesArray("R3BSignal");    
    fArrayVETOSignals = new TClonesArray("R3BSignal");    
    fArrayNEBVETOSignals = new TClonesArray("R3BSignal");     
    TheOutputFile = 0;
        
    // Set input data variables:
    ConnectionTest = kFALSE;
    InputFileNames.clear();
    InputFiles.clear();
    InputTrees.clear();
    
    // Initialize Nuclear dataBase:
    TheNuclei = new Nuclei();
    TheNuclei->BuildDataBase();
    
    // Set input parameters:
    TestMode = kFALSE;
    OutputPath = "./";
    EventCounter = 0;               
    nEvents = 1;
}

// Destructor definition:
R3BExpTranslator_Template::~R3BExpTranslator_Template()
{
    // Delete the TClonesArrays:
    if (fArraySignals) {fArraySignals->Clear(); delete fArraySignals;}
    if (fArrayVETOSignals) {fArrayVETOSignals->Clear(); delete fArrayVETOSignals;}
    if (fArrayNEBULASignals) {fArrayNEBULASignals->Clear(); delete fArrayNEBULASignals;}
    if (fArrayNEBVETOSignals) {fArrayNEBVETOSignals->Clear(); delete fArrayNEBVETOSignals;}
    
    // Clean vectors:
    ConnectionTest = kFALSE;
    InputFileNames.clear();
    InputFiles.clear();
    InputTrees.clear();

    // Delete other objects:
    delete TheNuclei;
    delete TheOutputFile;
    
    // delete all leafs (ATTENTION add all your leafs):
    delete NL_Hits_nl_bar;
}

// Function to connect to the experimental datafiles:
void R3BExpTranslator_Template::ConnectToInputFiles()
{
    // Define the path to the files:
    TString ExpFilePath = "/home/christiaan/Desktop/RIKEN_Data/250MeV/";
    
    // Define the tree-name to search for (ATTENTION adapt to your needs):
    TString TreeName = "sox";
    
    // Clean the relevant vectors:
    InputFileNames.clear();
    InputFiles.clear();
    InputTrees.clear();
    
    // Define the fileNames (ATTENTION add what you need):
    InputFileNames.push_back(ExpFilePath + "MS_Oct18_NeuLAND_p250MeV0004.root");
    
    // Next, connect to all input files:
    Int_t nFiles = InputFileNames.size();
    InputFiles.resize(nFiles);
    InputTrees.resize(nFiles);
    FileStat_t stom;
    Int_t FileTest;
    
    // Update the connection test:
    ConnectionTest = kTRUE;
    
    for (Int_t k = 0; k<nFiles; ++k)
    {
        FileTest = gSystem->GetPathInfo(InputFileNames.at(k).Data(),stom);
        
        if (FileTest==0)
        {
            InputFiles.at(k) = new TFile(InputFileNames.at(k).Data(),"read");
            InputTrees.at(k) = (TTree*) InputFiles.at(k)->Get(TreeName);
            if ((InputTrees.at(k)==0)||(InputTrees.at(k)==nullptr)) {ConnectionTest = kFALSE;}
        }
        else
        {
            ConnectionTest = kFALSE;
        }
    }
}

// Function to retrieve the total number of events from the source:
Int_t R3BExpTranslator_Template::GetNumberOfEvents()
{
    nEvents = 0;
    
    if (ConnectionTest==kTRUE)
    {
        Int_t nFiles = InputTrees.size();
    
        for (Int_t k = 0; k<nFiles; ++k)
        {
            nEvents = nEvents + InputTrees.at(k)->GetEntries();
        }
    }
    
    if (TestMode==kTRUE) {nEvents = 10000;}
    
    return nEvents;
}

// Function to update the inputs-class:
void R3BExpTranslator_Template::UpdateInputs()
{
    // Update all required inputs (ATTENTION add what you need. Keep in mind that this matters for future analysis steps!)
    if (ConnectionTest==kTRUE)
    {
        nEvents = GetNumberOfEvents();
        Inputs->ModifyInteger("R3BRoot_nEvents",nEvents);
    
        // Process modifications:
        Inputs->WriteFile();
        Inputs->ReadFile();
    }
}

Bool_t R3BExpTranslator_Template::ConnectLeafs()
{
    // Connect all the required leafs.
    Bool_t LeafTest = kFALSE;
    
    if (ConnectionTest==kTRUE)
    {
        Int_t nFiles = InputTrees.size();
        LeafTest = kTRUE;
    
        // ATTENTION Add what you need here:
        NL_Hits_nl_bar = new TLeaf*[nFiles];
    
        for (Int_t k = 0; k<nFiles; ++k)
        {   
            // ATTENTION Add what you need here:
            NL_Hits_nl_bar[k] = InputTrees.at(k)->FindLeaf("NL_Hits.nl_bar");
            
            // ATTENTION Add what you need here:
            if ((NL_Hits_nl_bar[k]==0)||(NL_Hits_nl_bar[k]==nullptr)) {LeafTest = kFALSE; cout << "Failed to locate leaf NL_Hits.nl_bar in file " << InputFileNames.at(k) << "\n";}
        }
    }
    
    // retunr the answer:
    return LeafTest;
}
    
// FairTask initialization function:
InitStatus R3BExpTranslator_Template::Init()
{
    // Check that this task is connected to the input-files:
    if (ConnectionTest==kFALSE)
    {
        cout << "I/O-manager FATAL: R3BExpTranslator_Template::Init: Could not connect to experimental data!\n\n";
        return kFATAL;
    }
    
    // Obtain the I/O manager:
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman)
    {
        cout << "I/O-manager FATAL: R3BExpTranslator_Template::Init: No FairRootManager\n\n";
        return kFATAL;
    }
    TheOutputFile = ioman->GetOutFile();
    
    // Check the inputs class:
    if (Inputs->IsLinked()==kFALSE)
    {
        cout << "I/O-manager FATAL: R3BExpTranslator_Template: The R3BInputClass is not linked to a file!\n\n";
        return kFATAL;
    }
    
    // Obtain all required inputs (ATTENTION add what you need):
    OutputPath = Inputs->GetInputString("TheOutputPath");
    
    // Connect all relevant leafs to the input data:
    Bool_t LeafTest = ConnectLeafs();
    
    if (ConnectionTest==kFALSE)
    {
        cout << "I/O-manager FATAL: R3BExpTranslator_Template::Init: Could not connect to some of the leafs!\n\n";
        return kFATAL;
    }

    // Register the output-array:
    ioman->Register("Signals","R3BSignal",fArraySignals,kTRUE);
    ioman->Register("VETOSignals","R3BSignal",fArrayVETOSignals,kTRUE);
    ioman->Register("NEBULASignals","R3BSignal",fArrayNEBULASignals,kTRUE);
    ioman->Register("NEBULAVETOSignals","R3BSignal",fArrayNEBVETOSignals,kTRUE);
    
    // Then, return the succes statement:
    if (Inputs->ContainsNoErrors()==kFALSE) {Inputs->PrintAllErrors(); return kFATAL;}
    EventCounter = 0;
    return kSUCCESS;
}

// Reading of the proper event:
Int_t R3BExpTranslator_Template::ReadCurrentEvent()
{
    Int_t VectorIndex = -1;
    
    // Reads the current event from the list of files.
    if ((EventCounter>=0)&&(EventCounter<nEvents))
    {
        Int_t FileIndex = -1;
        Int_t EventIndex = EventCounter;
        Int_t nEvents_InTree;
    
        while (EventIndex>=0)
        {
            FileIndex = FileIndex + 1;
            nEvents_InTree = InputTrees.at(FileIndex)->GetEntries();
            EventIndex = EventIndex - nEvents_InTree;
        }
        
        // Then, restore the last step:
        nEvents_InTree = InputTrees.at(FileIndex)->GetEntries();
        EventIndex = EventIndex + nEvents_InTree;
        
        // And now we have the correct indices to read:
        Long64_t nBytes = InputTrees.at(FileIndex)->GetEntry(EventIndex);
        
        // store vector index:
        VectorIndex = FileIndex;
    }
    
    // Update EventCounter:
    EventCounter = EventCounter + 1;
    
    // return the index, so we know how to read out the leafs:
    return VectorIndex;
}

// FairTask Execute function:
void R3BExpTranslator_Template::Exec(Option_t *option)
{
    // Reset all output arrays:
    fArraySignals->Clear("C");
    fArrayVETOSignals->Clear("C");
    fArrayNEBULASignals->Clear("C");
    fArrayNEBVETOSignals->Clear("C");
    
    // Read the new event:
    Int_t LeafIndex = ReadCurrentEvent();
    
    // check that it worked:
    if (LeafIndex>=0)
    {
        // Do the translation. ATTENTION Adapt it to your needs.
        Int_t nSignals = NL_Hits_nl_bar[LeafIndex]->GetNdata();
        R3BSignal* ThisSignal;
        
        for (Int_t ksig = 0; ksig<nSignals; ++ksig)
        {
            // create a new signal:
            new ((*fArraySignals)[ksig]) R3BSignal();
            
            // Retrieve it:
            ThisSignal = (R3BSignal*) fArraySignals->At(ksig);
            
            // Put in the proper values:
            ThisSignal->SetCrystalIndex(NL_Hits_nl_bar[LeafIndex]->GetValue(ksig));
            // NOTE: check out starting at 0 or 1!
        }
    }
    
    // Log progress:
    if (EventCounter%1000==0) {cout << "R3BExpTranslator_Template Log: We processed " << EventCounter << " events.\n";}
}
        
// Definition of Finish-function:
void R3BExpTranslator_Template::Finish()
{
    // Select output-file:
    TheOutputFile->cd();
    
    // Finish up whatever you want.
}

// Generate ROOT dictionary:
ClassImp(R3BExpTranslator_Template);
