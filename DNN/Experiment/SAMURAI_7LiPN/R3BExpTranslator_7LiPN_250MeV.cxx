// Every CXX-file includes its own header file:
#include "R3BExpTranslator_7LiPN_250MeV.h"

// Default constructor definition:
R3BExpTranslator_7LiPN_250MeV::R3BExpTranslator_7LiPN_250MeV() : FairTask("R3BExpTranslator_7LiPN_250MeV")
{
    // Set default verbosity level:
    fVerbose = 1;
    
    OutputMessages = kFALSE;

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
    BackGroundMode = kFALSE;
    TestMode = kFALSE;
    OutputPath = "./";
    EventCounter = 0;               
    nEvents = 1;
    nPaddlesPerPlane = 50;
    nDoublePlanes = 4;
    NeuLAND_Center_X = 0.0;
    NeuLAND_Center_Y = 0.0;
    NeuLAND_Front_Z = 1093.0;
    NeuLAND_Rot_X = 0.0;
    NeuLAND_Rot_Y = 0.0;
    NeuLAND_Rot_Z = 0.0;
    NeuLAND_TotalBarThicknessXY = 5.0;
    NeuLAND_TotalBarThicknessZ = 5.0;
    VETO_nBars = 8;
    VETO_Center_X = 0.0;
    VETO_Center_Y = 0.0; 
    VETO_Front_Z = 1073.0; 
    VETO_Rot_X = 0.0; 
    VETO_Rot_Y = 0.0; 
    VETO_Rot_Z = 0.0;
    VETO_TotalBarThicknessXY = 32.0;
    VETO_TotalBarThicknessZ = 1.0;
    NEBULA_NBars_OnePlane = 30;
    NEBULA_NModules = 2;
    NEBULA_DistModules = 80.0;
    NEBULA_Center_X = 0.0;
    NEBULA_Center_Y = 0.0; 
    NEBULA_Front_Z = 1500.0; 
    NEBULA_Rot_X = 0.0; 
    NEBULA_Rot_Y = 0.0; 
    NEBULA_Rot_Z = 0.0;
    NEBULA_TotalBarThickness = 12.0;
    NEBULA_VETO_nBars = 12;
    NEBULA_VETO_Center_X = 0.0;
    NEBULA_VETO_Center_Y = 0.0; 
    NEBULA_VETO_Front_Z = 1470.0; 
    NEBULA_VETO_Rot_X = 0.0; 
    NEBULA_VETO_Rot_Y = 0.0; 
    NEBULA_VETO_Rot_Z = 0.0;
    NEBULA_VETO_TotalBarThicknessXY = 32.0;
    NEBULA_VETO_TotalBarThicknessZ = 1.0;
    TargetT = 0.0;
    TargetX = 0.0;
    TargetY = 0.0;
    TargetZ = 0.0;
}

// Destructor definition:
R3BExpTranslator_7LiPN_250MeV::~R3BExpTranslator_7LiPN_250MeV()
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
    delete NLNeutron_fTime;
    delete NLNeutron_fBar;
    delete NLNeutron_fMeVee;
    delete NLNeutron_fPosVec_fX;
    delete NLNeutron_fPosVec_fY;
    delete NLNeutron_fPosVec_fZ;
    delete nlv_id;
    delete nlv_tof;
    delete nlv_eloss;
    delete NEBULAPla_fQAveCal;
    delete NEBULAPla_fTAveCal;
    delete NEBULAPla_fLayer;
    delete NEBULAPla_fSubLayer;
    delete NEBULAPla_fPosCal;
    delete NEBULAPla_id;
}

// Function to connect to the experimental datafiles:
void R3BExpTranslator_7LiPN_250MeV::ConnectToInputFiles(TString const Folder)
{
    // Define the path to the files:
    TString ExpFilePath = "/home/christiaan/Desktop/RIKEN_Data/250MeV/";
    if (Folder=="110MeV") {ExpFilePath = "/home/christiaan/Desktop/RIKEN_Data/110MeV/";}
    
    // Define the tree-name to search for:
    TString TreeName = "sox";
    
    // Clean the relevant vectors:
    InputFileNames.clear();
    InputFiles.clear();
    InputTrees.clear();
    
    // Define the fileNames (ATTENTION add what you need):
    if (Folder=="110MeV")
    {
        if (BackGroundMode==kFALSE)
        {
            InputFileNames.push_back(ExpFilePath + "MS_Oct18_NeuLAND_p110MeV0496.root");
            InputFileNames.push_back(ExpFilePath + "MS_Oct18_NeuLAND_p110MeV0497.root");
            InputFileNames.push_back(ExpFilePath + "MS_Oct18_NeuLAND_p110MeV0498.root");
            InputFileNames.push_back(ExpFilePath + "MS_Oct18_NeuLAND_p110MeV0500.root");
            InputFileNames.push_back(ExpFilePath + "MS_Oct18_NeuLAND_p110MeV0501.root");
            InputFileNames.push_back(ExpFilePath + "MS_Oct18_NeuLAND_p110MeV0502.root");
            InputFileNames.push_back(ExpFilePath + "MS_Oct18_NeuLAND_p110MeV0503.root");
            InputFileNames.push_back(ExpFilePath + "MS_Oct18_NeuLAND_p110MeV0504.root");
            InputFileNames.push_back(ExpFilePath + "MS_Oct18_NeuLAND_p110MeV0505.root");
            InputFileNames.push_back(ExpFilePath + "MS_Oct18_NeuLAND_p110MeV0506.root");
            InputFileNames.push_back(ExpFilePath + "MS_Oct18_NeuLAND_p110MeV0507.root");
        }
        else
        {
            InputFileNames.push_back(ExpFilePath + "MS_Oct18_NeuLAND_p110MeV0508.root");
        }
    }
    else
    {
        if (BackGroundMode==kFALSE)
        {
            InputFileNames.push_back(ExpFilePath + "MS_Oct18_NeuLAND_p250MeV0004.root");
            InputFileNames.push_back(ExpFilePath + "MS_Oct18_NeuLAND_p250MeV0005.root");    
            InputFileNames.push_back(ExpFilePath + "MS_Oct18_NeuLAND_p250MeV0006.root");
            InputFileNames.push_back(ExpFilePath + "MS_Oct18_NeuLAND_p250MeV0007.root");
            InputFileNames.push_back(ExpFilePath + "MS_Oct18_NeuLAND_p250MeV0010.root");
            InputFileNames.push_back(ExpFilePath + "MS_Oct18_NeuLAND_p250MeV0011.root");
            InputFileNames.push_back(ExpFilePath + "MS_Oct18_NeuLAND_p250MeV0013.root");
            InputFileNames.push_back(ExpFilePath + "MS_Oct18_NeuLAND_p250MeV0014.root");
        }
        else
        {
            InputFileNames.push_back(ExpFilePath + "MS_Oct18_NeuLAND_p250MeV0015.root");
            InputFileNames.push_back(ExpFilePath + "MS_Oct18_NeuLAND_p250MeV0016.root");    
            InputFileNames.push_back(ExpFilePath + "MS_Oct18_NeuLAND_p250MeV0017.root");    
            InputFileNames.push_back(ExpFilePath + "MS_Oct18_NeuLAND_p250MeV0018.root");
        }
    }
    
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
        cout << " ### ==> InputFileNames.at(" << k << "/" << nFiles << ") = " << InputFileNames.at(k) << "\n";
        
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
Int_t R3BExpTranslator_7LiPN_250MeV::GetNumberOfEvents()
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
    
    if (TestMode==kTRUE) {nEvents = 1000000;}
    
    return nEvents;
}

// Function to update the inputs-class:
void R3BExpTranslator_7LiPN_250MeV::UpdateInputs()
{
    // Update all required inputs (ATTENTION add what you need. also matters for future analysis steps!)
    if (ConnectionTest==kTRUE)
    {
        nEvents = GetNumberOfEvents();
        Inputs->ModifyInteger("R3BRoot_nEvents",nEvents);
    
        // Process modifications:
        Inputs->WriteFile();
        Inputs->ReadFile();
    }
}

Bool_t R3BExpTranslator_7LiPN_250MeV::ConnectLeafs()
{
    // Connect all the required leafs.
    Bool_t LeafTest = kFALSE;
    
    if (ConnectionTest==kTRUE)
    {
        Int_t nFiles = InputTrees.size();
        LeafTest = kTRUE;
    
        // ATTENTION Add what you need here:
        NLNeutron_fTime = new TLeaf*[nFiles];
        NLNeutron_fBar = new TLeaf*[nFiles];
        NLNeutron_fMeVee = new TLeaf*[nFiles];
        NLNeutron_fPosVec_fX = new TLeaf*[nFiles];
        NLNeutron_fPosVec_fY = new TLeaf*[nFiles];
        NLNeutron_fPosVec_fZ = new TLeaf*[nFiles];
        nlv_id = new TLeaf*[nFiles];
        nlv_tof = new TLeaf*[nFiles];
        nlv_eloss = new TLeaf*[nFiles];
        NEBULAPla_fQAveCal = new TLeaf*[nFiles];
        NEBULAPla_fTAveCal = new TLeaf*[nFiles];
        NEBULAPla_fLayer = new TLeaf*[nFiles];
        NEBULAPla_fSubLayer = new TLeaf*[nFiles];
        NEBULAPla_fPosCal = new TLeaf*[nFiles];
        NEBULAPla_id = new TLeaf*[nFiles];
    
        for (Int_t k = 0; k<nFiles; ++k)
        {   
            // ATTENTION Add what you need here:
            NLNeutron_fTime[k] = InputTrees.at(k)->FindLeaf("NLNeutron.fTime");
            NLNeutron_fBar[k] = InputTrees.at(k)->FindLeaf("NLNeutron.fBar");
            NLNeutron_fMeVee[k] = InputTrees.at(k)->FindLeaf("NLNeutron.fMeVee");
            NLNeutron_fPosVec_fX[k] = InputTrees.at(k)->FindLeaf("NLNeutron.fPosVec.fX");
            NLNeutron_fPosVec_fY[k] = InputTrees.at(k)->FindLeaf("NLNeutron.fPosVec.fY");
            NLNeutron_fPosVec_fZ[k] = InputTrees.at(k)->FindLeaf("NLNeutron.fPosVec.fZ");
            nlv_id[k] = InputTrees.at(k)->FindLeaf("nlv_id");
            nlv_tof[k] = InputTrees.at(k)->FindLeaf("nlv_tof");
            nlv_eloss[k] = InputTrees.at(k)->FindLeaf("nlv_eloss");
            NEBULAPla_fQAveCal[k] = InputTrees.at(k)->FindLeaf("NEBULAPla.fQAveCal");
            NEBULAPla_fTAveCal[k] = InputTrees.at(k)->FindLeaf("NEBULAPla.fTAveCal");
            NEBULAPla_fLayer[k] = InputTrees.at(k)->FindLeaf("NEBULAPla.fLayer");
            NEBULAPla_fSubLayer[k] = InputTrees.at(k)->FindLeaf("NEBULAPla.fSubLayer");
            NEBULAPla_fPosCal[k] = InputTrees.at(k)->FindLeaf("NEBULAPla.fPosCal");
            NEBULAPla_id[k] = InputTrees.at(k)->FindLeaf("NEBULAPla.id");
            
            
            // ATTENTION Add what you need here:
            if ((NLNeutron_fTime[k]==0)||(NLNeutron_fTime[k]==nullptr)) {LeafTest = kFALSE; cout << "Failed to locate leaf <NLNeutron.fTime> in file " << InputFileNames.at(k) << "\n";}
            if ((NLNeutron_fBar[k]==0)||(NLNeutron_fBar[k]==nullptr)) {LeafTest = kFALSE; cout << "Failed to locate leaf <NLNeutron.fBar> in file " << InputFileNames.at(k) << "\n";}
            if ((NLNeutron_fMeVee[k]==0)||(NLNeutron_fMeVee[k]==nullptr)) {LeafTest = kFALSE; cout << "Failed to locate leaf <NLNeutron.fMeVee> in file " << InputFileNames.at(k) << "\n";}
            if ((NLNeutron_fPosVec_fX[k]==0)||(NLNeutron_fPosVec_fX[k]==nullptr)) {LeafTest = kFALSE; cout << "Failed to locate leaf <NLNeutron.fPosVec.fX> in file " << InputFileNames.at(k) << "\n";}
            if ((NLNeutron_fPosVec_fY[k]==0)||(NLNeutron_fPosVec_fY[k]==nullptr)) {LeafTest = kFALSE; cout << "Failed to locate leaf <NLNeutron.fPosVec.fY> in file " << InputFileNames.at(k) << "\n";}
            if ((NLNeutron_fPosVec_fZ[k]==0)||(NLNeutron_fPosVec_fZ[k]==nullptr)) {LeafTest = kFALSE; cout << "Failed to locate leaf <NLNeutron.fPosVec.fZ> in file " << InputFileNames.at(k) << "\n";}
            if ((nlv_id[k]==0)||(nlv_id[k]==nullptr)) {LeafTest = kFALSE; cout << "Failed to locate leaf <nlv_id> in file " << InputFileNames.at(k) << "\n";}
            if ((nlv_tof[k]==0)||(nlv_tof[k]==nullptr)) {LeafTest = kFALSE; cout << "Failed to locate leaf <nlv_tof> in file " << InputFileNames.at(k) << "\n";}
            if ((nlv_eloss[k]==0)||(nlv_eloss[k]==nullptr)) {LeafTest = kFALSE; cout << "Failed to locate leaf <nlv_eloss> in file " << InputFileNames.at(k) << "\n";}
            if ((NEBULAPla_fQAveCal[k]==0)||(NEBULAPla_fQAveCal[k]==nullptr)) {LeafTest = kFALSE; cout << "Failed to locate leaf <NEBULAPla.fQAveCal> in file " << InputFileNames.at(k) << "\n";}
            if ((NEBULAPla_fTAveCal[k]==0)||(NEBULAPla_fTAveCal[k]==nullptr)) {LeafTest = kFALSE; cout << "Failed to locate leaf <NEBULAPla.fTAveCal> in file " << InputFileNames.at(k) << "\n";}
            if ((NEBULAPla_fLayer[k]==0)||(NEBULAPla_fLayer[k]==nullptr)) {LeafTest = kFALSE; cout << "Failed to locate leaf <NEBULAPla.fLayer> in file " << InputFileNames.at(k) << "\n";}
            if ((NEBULAPla_fSubLayer[k]==0)||(NEBULAPla_fSubLayer[k]==nullptr)) {LeafTest = kFALSE; cout << "Failed to locate leaf <NEBULAPla.fSubLayer> in file " << InputFileNames.at(k) << "\n";}
            if ((NEBULAPla_fPosCal[k]==0)||(NEBULAPla_fPosCal[k]==nullptr)) {LeafTest = kFALSE; cout << "Failed to locate leaf <NEBULAPla.fPosCal> in file " << InputFileNames.at(k) << "\n";}
            if ((NEBULAPla_id[k]==0)||(NEBULAPla_id[k]==nullptr)) {LeafTest = kFALSE; cout << "Failed to locate leaf <NEBULAPla.id> in file " << InputFileNames.at(k) << "\n";}
        }
    }
    
    // retunr the answer:
    return LeafTest;
}
    
// FairTask initialization function:
InitStatus R3BExpTranslator_7LiPN_250MeV::Init()
{
    // Check that this task is connected to the input-files:
    if (ConnectionTest==kFALSE)
    {
        cout << "I/O-manager FATAL: R3BExpTranslator_7LiPN_250MeV::Init: Could not connect to experimental data!\n\n";
        return kFATAL;
    }
    
    // Obtain the I/O manager:
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman)
    {
        cout << "I/O-manager FATAL: R3BExpTranslator_7LiPN_250MeV::Init: No FairRootManager\n\n";
        return kFATAL;
    }
    TheOutputFile = ioman->GetOutFile();
    
    // Check the inputs class:
    if (Inputs->IsLinked()==kFALSE)
    {
        cout << "I/O-manager FATAL: R3BExpTranslator_7LiPN_250MeV: The R3BInputClass is not linked to a file!\n\n";
        return kFATAL;
    }
    
    // Obtain all required inputs:
    OutputPath = Inputs->GetInputString("TheOutputPath");
    nPaddlesPerPlane = Inputs->GetInputInteger("NeuLAND_Number_of_Bars_per_plane");
    nDoublePlanes = Inputs->GetInputInteger("NeuLAND_Number_of_DoublePlanes");
    NeuLAND_Center_X = Inputs->GetInputDouble("NeuLAND_center_x_position","cm");
    NeuLAND_Center_Y = Inputs->GetInputDouble("NeuLAND_center_y_position","cm");
    NeuLAND_Front_Z = Inputs->GetInputDouble("NeuLAND_front_z_position","cm");
    NeuLAND_Rot_X = Inputs->GetInputDouble("NeuLAND_x_rotation_angle","degree");
    NeuLAND_Rot_Y = Inputs->GetInputDouble("NeuLAND_y_rotation_angle","degree");
    NeuLAND_Rot_Z = Inputs->GetInputDouble("NeuLAND_z_rotation_angle","degree");
    NeuLAND_TotalBarThicknessXY = Inputs->GetTotalPaddleThicknessXY();
    NeuLAND_TotalBarThicknessZ = Inputs->GetTotalPaddleThicknessZ();
    VETO_nBars = Inputs->GetInputInteger("VETO_Number_of_Bars_in_plane");
    VETO_Center_X = Inputs->GetInputDouble("VETO_center_x_position","cm");
    VETO_Center_Y = Inputs->GetInputDouble("VETO_center_y_position","cm");
    VETO_Front_Z = Inputs->GetInputDouble("VETO_front_z_position","cm");
    VETO_Rot_X = Inputs->GetInputDouble("VETO_x_rotation_angle","degree");
    VETO_Rot_Y = Inputs->GetInputDouble("VETO_y_rotation_angle","degree");
    VETO_Rot_Z = Inputs->GetInputDouble("VETO_z_rotation_angle","degree");
    VETO_TotalBarThicknessXY = Inputs->GetVETOTotalPaddleThicknessXY();
    VETO_TotalBarThicknessZ = Inputs->GetVETOTotalPaddleThicknessZ();
    NEBULA_NBars_OnePlane = Inputs->GetInputInteger("NEBULA_Number_of_Bars_in_OnePlane");
    NEBULA_NModules = Inputs->GetInputInteger("NEBULA_Number_of_DoublePlaneModules");
    NEBULA_DistModules = Inputs->GetInputDouble("NEBULA_Distance_BetweenModules","cm");
    NEBULA_Center_X = Inputs->GetInputDouble("NEBULA_center_x_position","cm");
    NEBULA_Center_Y = Inputs->GetInputDouble("NEBULA_center_y_position","cm");
    NEBULA_Front_Z = Inputs->GetInputDouble("NEBULA_front_z_position","cm");
    NEBULA_Rot_X = Inputs->GetInputDouble("NEBULA_x_rotation_angle","degree");
    NEBULA_Rot_Y = Inputs->GetInputDouble("NEBULA_y_rotation_angle","degree");
    NEBULA_Rot_Z = Inputs->GetInputDouble("NEBULA_z_rotation_angle","degree");
    NEBULA_TotalBarThickness = Inputs->GetNEBULATotalPaddleThickness();
    NEBULA_VETO_Center_X = Inputs->GetInputDouble("NEBULA_VET_center_x_position","cm");
    NEBULA_VETO_Center_Y = Inputs->GetInputDouble("NEBULA_VET_center_y_position","cm");
    NEBULA_VETO_Front_Z = Inputs->GetInputDouble("NEBULA_VET_front_z_position","cm");
    NEBULA_VETO_Rot_X = Inputs->GetInputDouble("NEBULA_VET_x_rotation_angle","degree");
    NEBULA_VETO_Rot_Y = Inputs->GetInputDouble("NEBULA_VET_y_rotation_angle","degree");
    NEBULA_VETO_Rot_Z = Inputs->GetInputDouble("NEBULA_VET_z_rotation_angle","degree");
    NEBULA_VETO_TotalBarThicknessXY = Inputs->GetNEBULAVETOTotalPaddleThicknessXY();
    NEBULA_VETO_TotalBarThicknessZ = Inputs->GetNEBULAVETOTotalPaddleThicknessXY();
    TargetX = Inputs->GetInputDouble("TARGET_center_x_position","cm");
    TargetY = Inputs->GetInputDouble("TARGET_center_y_position","cm");
    TargetZ = Inputs->GetInputDouble("TARGET_center_z_position","cm");
    TargetT = Inputs->GetTargetTime();
    
    // Connect all relevant leafs to the input data:
    Bool_t LeafTest = ConnectLeafs();
    
    if (ConnectionTest==kFALSE)
    {
        cout << "I/O-manager FATAL: R3BExpTranslator_7LiPN_250MeV::Init: Could not connect to some of the leafs!\n\n";
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
    
    if (BackGroundMode==kTRUE)  {cout << "\n\n ==> ATTENTION: We run in Background-mode! <==\n\n";}
    if (BackGroundMode==kFALSE) {cout << "\n\n ==> ATTENTION: We run in Signal-mode! <==\n\n";}
    
    return kSUCCESS;
}

// Reading of the proper event:
Int_t R3BExpTranslator_7LiPN_250MeV::ReadCurrentEvent()
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
void R3BExpTranslator_7LiPN_250MeV::Exec(Option_t *option)
{
    // Reset all output arrays:
    fArraySignals->Clear("C");
    fArrayVETOSignals->Clear("C");
    fArrayNEBULASignals->Clear("C");
    fArrayNEBVETOSignals->Clear("C");
    
    // Read the new event:
    Int_t LeafIndex = ReadCurrentEvent();
    
    // Check that it worked:
    if (LeafIndex>=0)
    {
        // Declare what we need:
        Int_t nSignals = NLNeutron_fBar[LeafIndex]->GetNdata();
        Int_t CreationIndex = 0;
        Int_t CurrentPlane = -1;
        Int_t LocalPaddle = -1;
        Double_t xpos = 0.0;
        Double_t ypos = 0.0;
        Double_t zpos = 0.0;
        Double_t ThisBeta = 0.0;
        Bool_t TranslationTest = kFALSE;
        Bool_t NANTest = kFALSE;
        TVector3 ThePos;
        R3BSignal* ThisSignal = new R3BSignal();
        
        // =============================================================================
        
        // Do the NeuLAND translation:
        for (Int_t ksig = 0; ksig<nSignals; ++ksig)
        {
            // Put in the proper values in the separate ThisSignal:
            ThisSignal->SetNonPrimaryExp();
            ThisSignal->SetNonPrimarySim();
            
            // Bar number. NL_Hits_nl_bar starts at 0, we need to start at 1!
            ThisSignal->SetCrystalIndex(1+NLNeutron_fBar[LeafIndex]->GetValue(ksig));
            
            // QDC (Edep) value [MeV]:
            ThisSignal->SetEnergy(NLNeutron_fMeVee[LeafIndex]->GetValue(ksig));
            
            // TDC (TOF) value:
            ThisSignal->SetTime(NLNeutron_fTime[LeafIndex]->GetValue(ksig));
            
            // Position coordinates (retrieve them from the bar index):
            CurrentPlane = ThisSignal->GetCurrentPlane("NeuLAND",nPaddlesPerPlane);
            zpos = -1.0*NeuLAND_TotalBarThicknessZ*((Int_t) nDoublePlanes) + (((Int_t) (CurrentPlane-1))+0.5)*NeuLAND_TotalBarThicknessZ;
            LocalPaddle = ThisSignal->GetCrystalIndex() - (CurrentPlane-1)*nPaddlesPerPlane;
                
            if (ThisSignal->IsHorPaddle("NeuLAND",nPaddlesPerPlane)==kTRUE)
            {
                xpos = NLNeutron_fPosVec_fX[LeafIndex]->GetValue(ksig)/10.0; // leaf-value is in [mm].
                ypos = -0.5*NeuLAND_TotalBarThicknessXY*((Int_t) nPaddlesPerPlane) + (((Int_t) (LocalPaddle-1))+0.5)*NeuLAND_TotalBarThicknessXY;
            }
            else
            {
                // NOTE: The charged-particle background is at negative x, so negative x is where the magnet bends to. This corresponds to the simulation!
                xpos = -0.5*NeuLAND_TotalBarThicknessXY*((Int_t) nPaddlesPerPlane) + (((Int_t) (LocalPaddle-1))+0.5)*NeuLAND_TotalBarThicknessXY;
                ypos = NLNeutron_fPosVec_fY[LeafIndex]->GetValue(ksig)/10.0; // leaf-value is in [mm].
            }
                
            // Do proper transformation:
            ThePos.SetX(xpos);
            ThePos.SetY(ypos);
            ThePos.SetZ(zpos);
               
            ThePos.RotateX(NeuLAND_Rot_X*TMath::Pi()/180.0);
            ThePos.RotateY(NeuLAND_Rot_Y*TMath::Pi()/180.0);
            ThePos.RotateZ(NeuLAND_Rot_Z*TMath::Pi()/180.0);
                
            xpos = ThePos.X() + NeuLAND_Center_X;
            ypos = ThePos.Y() + NeuLAND_Center_Y;
            zpos = ThePos.Z() + NeuLAND_Front_Z + NeuLAND_TotalBarThicknessZ*((Int_t) nDoublePlanes);
                
            // Put the values in the signal:
            ThisSignal->SetPositionX(xpos);
            ThisSignal->SetPositionY(ypos);
            ThisSignal->SetPositionZ(zpos);
            
            // Compare beta as extra safety-check:
            ThisBeta = ThisSignal->GetBeta(TargetT,TargetX,TargetY,TargetZ);
            
            // Do a comparison to test if the data is meaningful:
            TranslationTest = kTRUE;
            
            // Remove (-)nan data:
            NANTest = kFALSE;
            if (((ThisSignal->GetEnergy()>-1.0*1e99)&&(ThisSignal->GetEnergy()<1.0*1e99))&&
                ((ThisSignal->GetTime()>-1.0*1e99)&&(ThisSignal->GetTime()<1.0*1e99))&&
                ((ThisSignal->GetPositionX()>-1.0*1e99)&&(ThisSignal->GetPositionX()<1.0*1e99))&&
                ((ThisSignal->GetPositionY()>-1.0*1e99)&&(ThisSignal->GetPositionY()<1.0*1e99))&&
                ((ThisSignal->GetPositionZ()>-1.0*1e99)&&(ThisSignal->GetPositionZ()<1.0*1e99))&&
                ((ThisSignal->GetCrystalIndex()>-1000000)&&(ThisSignal->GetPositionX()<1000000)))
            {
                NANTest = kTRUE;
            }
            if (NANTest==kFALSE) {TranslationTest = kFALSE;}
            
            if ((ThisSignal->GetEnergy()<0.0)&&(TranslationTest==kTRUE))
            {
                TranslationTest = kFALSE;
                if (OutputMessages==kTRUE) {cout << "Event " << EventCounter << " & index " << ksig << "/" << nSignals << " in file " << InputFileNames.at(LeafIndex) << ": NeuLAND Signal Energy was smaller then zero.\n";}
            }
            
            if ((ThisSignal->GetTime()<TargetT)&&(TranslationTest==kTRUE))
            {
               TranslationTest = kFALSE;
               if (OutputMessages==kTRUE) {cout << "Event " << EventCounter << " & index " << ksig << "/" << nSignals << " in file " << InputFileNames.at(LeafIndex) << ": NeuLAND Signal Time was smaller then the targte time.\n";}
            }
            
            if ((ThisBeta<0.0)&&(TranslationTest==kTRUE))
            {
                TranslationTest = kFALSE;
                if (OutputMessages==kTRUE) {cout << "Event " << EventCounter << " & index " << ksig << "/" << nSignals << " in file " << InputFileNames.at(LeafIndex) << ": NeuLAND Signal-computed beta was smaller then zero.\n";}
            }
            
            if ((ThisBeta>1.0)&&(TranslationTest==kTRUE))
            {
                TranslationTest = kFALSE;
                if (OutputMessages==kTRUE) {cout << "Event " << EventCounter << " & index " << ksig << "/" << nSignals << " in file " << InputFileNames.at(LeafIndex) << ": NeuLAND Signal-computed beta was bigger then one.\n";}
            }
    
            // Only keep signals that passed the tests:
            if (TranslationTest==kTRUE)
            {
                new ((*fArraySignals)[CreationIndex]) R3BSignal(ThisSignal);
                CreationIndex = CreationIndex + 1;
            }
                      
            // Done.
        }
        
        // =============================================================================
        
        // Do the NeuLAND VETO translation:
        nSignals = nlv_id[LeafIndex]->GetNdata();
        CreationIndex = 0;
        
        for (Int_t ksig = 0; ksig<nSignals; ++ksig)
        {
            // Put in the proper values in the separate ThisSignal:
            ThisSignal->SetNonPrimaryExp();
            ThisSignal->SetNonPrimarySim();
            
            // Bar number. NL_Hits_nl_bar starts at 0, we need to start at 1!
            ThisSignal->SetCrystalIndex(nlv_id[LeafIndex]->GetValue(ksig));
            
            // QDC (Edep) value [MeV]:
            ThisSignal->SetEnergy(nlv_eloss[LeafIndex]->GetValue(ksig));
            
            // TDC (TOF) value:
            ThisSignal->SetTime(nlv_tof[LeafIndex]->GetValue(ksig));
            
            // Position coordinates (retrieve them from the bar index):
            zpos = 0.0;
            xpos = -0.5*VETO_TotalBarThicknessXY*((Int_t) VETO_nBars) + (((Int_t) (ThisSignal->GetCrystalIndex()-1))+0.5)*NeuLAND_TotalBarThicknessXY;
            xpos = -1.0*xpos; // ATTENTION: negative xpos should correspond to the charged-particle background!
            ypos = 0.0; // ATTENTION: Not available in the tree, so this is the best we can do!
                
            // Do proper transformation:
            ThePos.SetX(xpos);
            ThePos.SetY(ypos);
            ThePos.SetZ(zpos);
               
            ThePos.RotateX(VETO_Rot_X*TMath::Pi()/180.0);
            ThePos.RotateY(VETO_Rot_Y*TMath::Pi()/180.0);
            ThePos.RotateZ(VETO_Rot_Z*TMath::Pi()/180.0);
                
            xpos = ThePos.X() + VETO_Center_X;
            ypos = ThePos.Y() + VETO_Center_Y;
            zpos = ThePos.Z() + VETO_Front_Z + VETO_TotalBarThicknessZ*0.5;
                
            // Put the values in the signal:
            ThisSignal->SetPositionX(xpos);
            ThisSignal->SetPositionY(ypos);
            ThisSignal->SetPositionZ(zpos);
               
            // Compute Beta:
            ThisBeta = ThisSignal->GetBeta(TargetT,TargetX,TargetY,TargetZ);
            
            // Do a comparison:
            TranslationTest = kTRUE;
            
            // Remove (-)nan data:
            NANTest = kFALSE;
            if (((ThisSignal->GetEnergy()>-1.0*1e99)&&(ThisSignal->GetEnergy()<1.0*1e99))&&
                ((ThisSignal->GetTime()>-1.0*1e99)&&(ThisSignal->GetTime()<1.0*1e99))&&
                ((ThisSignal->GetPositionX()>-1.0*1e99)&&(ThisSignal->GetPositionX()<1.0*1e99))&&
                ((ThisSignal->GetPositionY()>-1.0*1e99)&&(ThisSignal->GetPositionY()<1.0*1e99))&&
                ((ThisSignal->GetPositionZ()>-1.0*1e99)&&(ThisSignal->GetPositionZ()<1.0*1e99))&&
                ((ThisSignal->GetCrystalIndex()>-1000000)&&(ThisSignal->GetPositionX()<1000000)))
            {
                NANTest = kTRUE;
            }
            if (NANTest==kFALSE) {TranslationTest = kFALSE;}
            
            if ((ThisSignal->GetEnergy()<0.0)&&(TranslationTest==kTRUE))
            {
                TranslationTest = kFALSE;
                if (OutputMessages==kTRUE) {cout << "Event " << EventCounter << " & index " << ksig << "/" << nSignals << " in file " << InputFileNames.at(LeafIndex) << ": VETO Signal Energy was smaller then zero.\n";}
            }
            
            if ((ThisSignal->GetTime()<TargetT)&&(TranslationTest==kTRUE))
            {
               TranslationTest = kFALSE;
               if (OutputMessages==kTRUE) {cout << "Event " << EventCounter << " & index " << ksig << "/" << nSignals << " in file " << InputFileNames.at(LeafIndex) << ": VETO Signal Time was smaller then the targte time.\n";}
            }
            
            if ((ThisBeta<0.0)&&(TranslationTest==kTRUE))
            {
                TranslationTest = kFALSE;
                if (OutputMessages==kTRUE) {cout << "Event " << EventCounter << " & index " << ksig << "/" << nSignals << " in file " << InputFileNames.at(LeafIndex) << ": VETO Signal-computed beta was smaller then zero.\n";}
            }
            
            if ((ThisBeta>1.0)&&(TranslationTest==kTRUE))
            {
                TranslationTest = kFALSE;
                if (OutputMessages==kTRUE) {cout << "Event " << EventCounter << " & index " << ksig << "/" << nSignals << " in file " << InputFileNames.at(LeafIndex) << ": VETO Signal-computed beta was bigger then one.\n";}
            }
         
            // Only keep signals that passed the tests:
            if (TranslationTest==kTRUE)
            {
                new ((*fArrayVETOSignals)[CreationIndex]) R3BSignal(ThisSignal);
                CreationIndex = CreationIndex + 1;
            }
                      
            // Done.
        }
        
        // =============================================================================
        
        // Do the NEBULA (& its VETO) translation:
        nSignals = NEBULAPla_id[LeafIndex]->GetNdata();
        Int_t NEBULA_CreationIndex = 0;
        Int_t NEBVETO_CreationIndex = 0;
        Int_t BarIndex = 0;
        Int_t LocalBarIndex = 0;
        Int_t OurLocalBarIndex = 0;
        Int_t LayerIndex = 0;
        Bool_t HitIsVETO = kFALSE;
        
        for (Int_t ksig = 0; ksig<nSignals; ++ksig)
        {
            // Put in the proper values in the separate ThisSignal:
            ThisSignal->SetNonPrimaryExp();
            ThisSignal->SetNonPrimarySim();
            
            // Decide between NEBULA and VETO:
            BarIndex = NEBULAPla_id[LeafIndex]->GetValue(ksig);
            
            if ((BarIndex>=121)&&(BarIndex<=144))
            {
                // Then, we have a VETO hit:
                HitIsVETO = kTRUE;
                
                LayerIndex = 1;
                LocalBarIndex = BarIndex - 120;
                if (BarIndex>=133)
                {
                    LocalBarIndex = BarIndex - 12;
                    LayerIndex = 2;
                }
                
                // Hence, define the crystal index:
                ThisSignal->SetCrystalIndex(LocalBarIndex + (LayerIndex-1)*NEBULA_VETO_nBars);
                
                // QDC (Edep) value [MeV]:
                ThisSignal->SetEnergy(NEBULAPla_fQAveCal[LeafIndex]->GetValue(ksig));
            
                // TDC (TOF) value:
                ThisSignal->SetTime(NEBULAPla_fTAveCal[LeafIndex]->GetValue(ksig)/10.0); // NOTE: conversion to [ns]!
            
                // Position coordinates (retrieve them from the bar index):
                zpos = 0.0;
                if (LayerIndex==2) {zpos = zpos + NEBULA_DistModules;}
                
                // Hard to see whether we start + or -. Just make sure it is consistent for NEBULA & its VETO.
                xpos = -0.5*((Int_t) NEBULA_VETO_nBars)*NEBULA_VETO_TotalBarThicknessXY + (((Int_t) (LocalBarIndex-1))+0.5)*NEBULA_VETO_TotalBarThicknessXY;
                ypos = NEBULAPla_fPosCal[LeafIndex]->GetValue(ksig)/10.0; // NOTE: Conversion to cm!
                
                // Do proper transformation:
                ThePos.SetX(xpos);
                ThePos.SetY(ypos);
                ThePos.SetZ(zpos);
               
                ThePos.RotateX(NEBULA_VETO_Rot_X*TMath::Pi()/180.0);
                ThePos.RotateY(NEBULA_VETO_Rot_Y*TMath::Pi()/180.0);
                ThePos.RotateZ(NEBULA_VETO_Rot_Z*TMath::Pi()/180.0);
                
                xpos = ThePos.X() + NEBULA_VETO_Center_X;
                ypos = ThePos.Y() + NEBULA_VETO_Center_Y;
                zpos = ThePos.Z() + NEBULA_VETO_Front_Z + NEBULA_VETO_TotalBarThicknessZ*0.5;
                
                // Put the values in the signal:
                ThisSignal->SetPositionX(xpos);
                ThisSignal->SetPositionY(ypos);
                ThisSignal->SetPositionZ(zpos);
            }
            else if ((BarIndex>=1)&&(BarIndex<=120))
            {
                // Then, we have a NEBULA-hit:
                HitIsVETO = kFALSE;
                
                LayerIndex = 1;
                LocalBarIndex = BarIndex;
                if (BarIndex>60)
                {
                    LocalBarIndex = BarIndex - 60;
                    LayerIndex = 2;
                }
                
                // Transform bar index to be consistent with our labelling:
                if (BarIndex<=30) {OurLocalBarIndex = (LocalBarIndex-1)*2+1;}
                else              {OurLocalBarIndex = LocalBarIndex - 30; OurLocalBarIndex = OurLocalBarIndex*2;}
                
                // Hence, define the crystal index:
                ThisSignal->SetCrystalIndex(OurLocalBarIndex + (LayerIndex-1)*NEBULA_NBars_OnePlane*2);
                
                // QDC (Edep) value [MeV]:
                ThisSignal->SetEnergy(NEBULAPla_fQAveCal[LeafIndex]->GetValue(ksig));
            
                // TDC (TOF) value:
                ThisSignal->SetTime(NEBULAPla_fTAveCal[LeafIndex]->GetValue(ksig)/10.0); // NOTE: conversion to [ns]!
            
                // Position coordinates (retrieve them from the bar index (NOT our labelling!)):
                Int_t SubLayer = 1;
                Int_t SubIndex = LocalBarIndex;
                
                while (SubIndex>0)
                {
                    SubIndex = SubIndex - NEBULA_NBars_OnePlane;
                    SubLayer = SubLayer + 1;
                }
                
                SubLayer = SubLayer - 1;
                SubIndex = SubIndex + NEBULA_NBars_OnePlane;
                
                zpos = ((Int_t) SubLayer)-0.5*NEBULA_TotalBarThickness;
                if (LayerIndex==2) {zpos = zpos + NEBULA_DistModules;}
                
                // Hard to see whether we start + or -. Just make sure it is consistent for NEBULA & its VETO.
                xpos = -0.5*((Int_t) NEBULA_NBars_OnePlane)*NEBULA_TotalBarThickness + (((Int_t) (SubIndex-1))+0.5)*NEBULA_TotalBarThickness;
                ypos = NEBULAPla_fPosCal[LeafIndex]->GetValue(ksig)/10.0; // NOTE: Conversion to cm!
                
                // Do proper transformation:
                ThePos.SetX(xpos);
                ThePos.SetY(ypos);
                ThePos.SetZ(zpos);
               
                ThePos.RotateX(NEBULA_Rot_X*TMath::Pi()/180.0);
                ThePos.RotateY(NEBULA_Rot_Y*TMath::Pi()/180.0);
                ThePos.RotateZ(NEBULA_Rot_Z*TMath::Pi()/180.0);
                
                xpos = ThePos.X() + NEBULA_Center_X;
                ypos = ThePos.Y() + NEBULA_Center_Y;
                zpos = ThePos.Z() + NEBULA_Front_Z;
                
                // Put the values in the signal:
                ThisSignal->SetPositionX(xpos);
                ThisSignal->SetPositionY(ypos);
                ThisSignal->SetPositionZ(zpos);
            }
            
            // Do a comparison:
            TranslationTest = kTRUE;
            ThisBeta = ThisSignal->GetBeta(TargetT,TargetX,TargetY,TargetZ);
            
            // Remove (-)nan data:
            NANTest = kFALSE;
            if (((ThisSignal->GetEnergy()>-1.0*1e99)&&(ThisSignal->GetEnergy()<1.0*1e99))&&
                ((ThisSignal->GetTime()>-1.0*1e99)&&(ThisSignal->GetTime()<1.0*1e99))&&
                ((ThisSignal->GetPositionX()>-1.0*1e99)&&(ThisSignal->GetPositionX()<1.0*1e99))&&
                ((ThisSignal->GetPositionY()>-1.0*1e99)&&(ThisSignal->GetPositionY()<1.0*1e99))&&
                ((ThisSignal->GetPositionZ()>-1.0*1e99)&&(ThisSignal->GetPositionZ()<1.0*1e99))&&
                ((ThisSignal->GetCrystalIndex()>-1000000)&&(ThisSignal->GetPositionX()<1000000)))
            {
                NANTest = kTRUE;
            }
            if (NANTest==kFALSE) {TranslationTest = kFALSE;}
            
            if ((ThisSignal->GetEnergy()<0.0)&&(TranslationTest==kTRUE))
            {
                TranslationTest = kFALSE;
                if (OutputMessages==kTRUE) {cout << "Event " << EventCounter << " & index " << ksig << "/" << nSignals << " in file " << InputFileNames.at(LeafIndex) << ": NEBULA (VETO) Signal Energy was smaller then zero.\n";}
            }
            
            if ((ThisSignal->GetTime()<TargetT)&&(TranslationTest==kTRUE))
            {
               TranslationTest = kFALSE;
               if (OutputMessages==kTRUE) {cout << "Event " << EventCounter << " & index " << ksig << "/" << nSignals << " in file " << InputFileNames.at(LeafIndex) << ": NEBULA (VETO) Signal Time was smaller then the targte time.\n";}
            }
            
            if ((ThisBeta<0.0)&&(TranslationTest==kTRUE))
            {
                TranslationTest = kFALSE;
                if (OutputMessages==kTRUE) {cout << "Event " << EventCounter << " & index " << ksig << "/" << nSignals << " in file " << InputFileNames.at(LeafIndex) << ": NEBULA (VETO) Signal-computed beta was smaller then zero.\n";}
            }
            
            if ((ThisBeta>1.0)&&(TranslationTest==kTRUE))
            {
                TranslationTest = kFALSE;
                if (OutputMessages==kTRUE) {cout << "Event " << EventCounter << " & index " << ksig << "/" << nSignals << " in file " << InputFileNames.at(LeafIndex) << ": NEBULA (VETO) Signal-computed beta was bigger then one.\n";}
            }
          
            // Only keep signals that passed the tests:
            if (TranslationTest==kTRUE)
            {
                if (HitIsVETO==kTRUE)
                {
                    new ((*fArrayNEBVETOSignals)[NEBVETO_CreationIndex]) R3BSignal(ThisSignal);
                    NEBVETO_CreationIndex = NEBVETO_CreationIndex + 1;
                }
                else
                {
                    new ((*fArrayNEBULASignals)[NEBULA_CreationIndex]) R3BSignal(ThisSignal);
                    NEBULA_CreationIndex = NEBULA_CreationIndex + 1;
                }
            }
                      
            // Done.
        }
        
        // delete objects:
        delete ThisSignal;
    }
    
    // Log progress:
    if (EventCounter%1000==0) {cout << "R3BExpTranslator_7LiPN_250MeV Log: We processed " << EventCounter << " events.\n";}
}
        
// Definition of Finish-function:
void R3BExpTranslator_7LiPN_250MeV::Finish()
{
    // Select output-file:
    TheOutputFile->cd();
    
    // Finish up whatever you want.
}

// Generate ROOT dictionary:
ClassImp(R3BExpTranslator_7LiPN_250MeV);
