// Every CXX-file includes its own header file:
#include "R3BNeuLANDTranslator.h"

// Default constructor definition:
R3BNeuLANDTranslator::R3BNeuLANDTranslator() : FairTask("R3BNeuLANDTranslator")
{
    // Set default verbosity level:
    fVerbose = 1;
    DistanceCheck = kFALSE;

    // Create arrays:
    fArrayDigis = new TClonesArray("R3BNeulandDigi");
    fArrayVETODigis = new TClonesArray("R3BNeulandDigi");
    fArrayNEBULADigis = new TClonesArray("R3BNeulandDigi");
    fArrayNEBVETODigis = new TClonesArray("R3BNeulandDigi");
    fArrayPrims = new TClonesArray("FairMCPoint");
    fArrayMCPoints = new TClonesArray("R3BMCTrack");
    fArrayPoints = new TClonesArray("R3BNeulandPoint");
    fArrayNEBPoints = new TClonesArray("R3BCADNEBULAPoint");
    fArrayJanClusters = new TClonesArray("R3BNeulandCADCluster");
    
    fArraySignals = new TClonesArray("R3BSignal");
    fArrayVETOSignals = new TClonesArray("R3BSignal");
    fArrayNEBULASignals = new TClonesArray("R3BSignal");
    fArrayNEBVETOSignals = new TClonesArray("R3BSignal");
    fArrayMCNeutronTracks = new TClonesArray("TLorentzVector");
    fArrayMCRecoilTracks = new TClonesArray("TLorentzVector");
    fArrayGunMult = new TClonesArray("ObjInteger");
    fArrayNeuMult = new TClonesArray("ObjInteger");
    fArrayNEBMult = new TClonesArray("ObjInteger");
    fArrayDetMult = new TClonesArray("ObjInteger");
    fArrayNEBDetMult = new TClonesArray("ObjInteger");
  
    // Set default input parameters:
    UseVETO = kFALSE;
    UseNEBULA = kFALSE;
    UseNEBVETO = kFALSE;
    TheVMCFolder = "./";
    MaxClusterMult = 1000;
    UseSimulation = kFALSE;
    ceff = 14.0;
    MaxMultiplicity = 10;
    EventCounter = 0;
    nEvents = 1;
    nPaddlesPerPlane = 1;
    nDoublePlanes = 1;
    ParticleType_ToBeDetected = "neutron";
    ParticleMass = -1.0;
    ParticlePDG = 2112;
    MarkSignals_Method = "BackTracing";
    MarkPrimPoints_Method = "R3BMCTrack";
    Target_PrimVolume_Thickness = 1.0;
    TargetX = 0.0;
    TargetY = 0.0;
    TargetZ = 0.0;
    TargetT = 0.0;
    NeuLAND_Center_X = 0.0;
    NeuLAND_Center_Y = 0.0;
    NeuLAND_Front_Z = 1400.0;
    NeuLAND_Rot_X = 0.0;
    NeuLAND_Rot_Y = 0.0;
    NeuLAND_Rot_Z = 0.0;
    NeuLAND_Active_Bar_Thickness = 4.8;
    NeuLAND_TotalBarThicknessXY = 5.0;
    NeuLAND_TotalBarThicknessZ = 5.0;
    NeuLAND_Total_Bar_Length = 250.0;
    MinimizationMarking_MaxDistance = 1e99;
    MinimizationMarking_IncludeTime = kTRUE;
    MinimizationMarking_AllowMaxDist = kFALSE;
    NumberOfDoublePlanes_PerModule = 30;
    DoModularSplit = kFALSE;
    ModularDistance = 0.0;
    BeamBeta = 0.5;
    ParticleGun_x_position = 0.0;
    ParticleGun_y_position = 0.0;
    ParticleGun_z_position = 0.0;
    
    // Set counters & verification parameters:
    GunMult_Counter = 0;
    NeuLANDMult_Counter = 0;
    DetectedMult_Counter = 0;
    MultMatrix = 0;
    InteractionMatrix = 0;
    DetectionMatrix = 0;
    MultNEBULAMatrix = 0;
    InteractionNEBULAMatrix = 0;
    DetectionNEBULAMatrix = 0;
    MultDoubleMatrix = 0;
    TotalNumberOfThreads = 1;
    CurrentThread = 0;
    ThisGunMultiplicity = 0;
    ThisNeuLANDMultiplicity = 0;
    ThisNEBULAMultiplicity = 0;
    ThisNEBDetectedMultiplicity = 0;
    IsPrimTrack = 0;
    MinTOF = 0;
    BarIndices = 0;
    PrimIntPoints.clear();
    PrimIntPoints_TrackID.clear();
    PrimLines.clear();
    AllTimes = 0;
    PrimTimes = 0;
    
    // Initialize Nuclear dataBase:
    TheNuclei = new Nuclei();
    TheNuclei->BuildDataBase();
    TheOutputFile = 0;
    OutputNameTag = "";
}

// Destructor definition:
R3BNeuLANDTranslator::~R3BNeuLANDTranslator()
{
    // Delete the arrays:
    if(fArraySignals) {fArraySignals->Clear(); delete fArraySignals;}
    if(fArrayVETOSignals) {fArrayVETOSignals->Clear(); delete fArrayVETOSignals;}
    if(fArrayNEBULASignals) {fArrayNEBULASignals->Clear(); delete fArrayNEBULASignals;}
    if(fArrayNEBVETOSignals) {fArrayNEBVETOSignals->Clear(); delete fArrayNEBVETOSignals;}
    if(fArrayMCNeutronTracks) {fArrayMCNeutronTracks->Clear(); delete fArrayMCNeutronTracks;}
    if(fArrayMCRecoilTracks) {fArrayMCRecoilTracks->Clear(); delete fArrayMCRecoilTracks;}
    
    if(fArrayDigis) {fArrayDigis->Clear(); delete fArrayDigis;}
    if(fArrayVETODigis) {fArrayVETODigis->Clear(); delete fArrayVETODigis;}
    if(fArrayNEBULADigis) {fArrayNEBULADigis->Clear(); delete fArrayNEBULADigis;}
    if(fArrayNEBVETODigis) {fArrayNEBVETODigis->Clear(); delete fArrayNEBVETODigis;}
    if(fArrayPrims) {fArrayPrims->Clear(); delete fArrayPrims;}
    if(fArrayMCPoints) {fArrayMCPoints->Clear(); delete fArrayMCPoints;}
    if(fArrayJanClusters) {fArrayJanClusters->Clear(); delete fArrayJanClusters;}
    
    if(fArrayGunMult) {fArrayGunMult->Clear(); delete fArrayGunMult;}
    if(fArrayNeuMult) {fArrayNeuMult->Clear(); delete fArrayNeuMult;}
    if(fArrayNEBMult) {fArrayNEBMult->Clear(); delete fArrayNEBMult;}
    if(fArrayDetMult) {fArrayDetMult->Clear(); delete fArrayDetMult;}
    if(fArrayNEBDetMult) {fArrayNEBDetMult->Clear(); delete fArrayNEBDetMult;}
    
    // Delete histograms:
    delete Multiplicity_Prim;
    delete Multiplicity_Signal;
    delete Multiplicity_Gun;
    delete PrimToSignal_Distance;

    // clear vectors:
    PrimIntPoints.clear();
    PrimIntPoints_TrackID.clear();
    PrimLines.clear();
    
    // Delete other objects:
    delete TheNuclei;
}

// FairTask initialization function:
InitStatus R3BNeuLANDTranslator::Init()
{
    // Obtain the I/O manager:
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman)
    {
        cout << "I/O-manager FATAL: R3BNeuLANDTranslator::Init: No FairRootManager\n\n";
        return kFATAL;
    }
    TheOutputFile = ioman->GetOutFile();
    
    // Check the inputs class:
    if (Inputs->IsLinked()==kFALSE)
    {
        cout << "I/O-manager FATAL: The R3BInputClass is not linked to a file!\n\n";
        return kFATAL;
    }
    
    // Retrieve Input parameters:
    RetrieveInputs();    
    
    cout << NeuLAND_TotalBarThicknessXY << " <======================================================= \n";
    
    // Obtain R3BNeulandDigis:
    if ((TClonesArray*)ioman->GetObject("NeulandDigis"+OutputNameTag) == nullptr)
    {
        cout << "I/O-manager FATAL: R3BNeuLANDTranslator::Init No NeulandDigis!\n\n";
        return kFATAL;
    }
    fArrayDigis = (TClonesArray*)ioman->GetObject("NeulandDigis"+OutputNameTag);
    
    // Obtain VETO digis:
    if (UseVETO==kTRUE)
    {
        if ((TClonesArray*)ioman->GetObject("VETODigis"+OutputNameTag) == nullptr)
        {
            cout << "I/O-manager FATAL: R3BNeuLANDTranslator::Init No VETODigis!\n\n";
            return kFATAL;
        }
        fArrayVETODigis = (TClonesArray*)ioman->GetObject("VETODigis"+OutputNameTag);
    }
    
    // Obtain NEBULA digis:
    if (UseNEBULA==kTRUE)
    {
        if ((TClonesArray*)ioman->GetObject("NEBULADigis"+OutputNameTag) == nullptr)
        {
            cout << "I/O-manager FATAL: R3BNeuLANDTranslator::Init No NEBULADigis!\n\n";
            return kFATAL;
        }
        fArrayNEBULADigis = (TClonesArray*)ioman->GetObject("NEBULADigis"+OutputNameTag);
    }
    
    // Obtain NEBULA VETO digis:
    if (UseNEBVETO==kTRUE)
    {
        if ((TClonesArray*)ioman->GetObject("NEBULAVETODigis"+OutputNameTag) == nullptr)
        {
            cout << "I/O-manager FATAL: R3BNeuLANDTranslator::Init No NEBULAVETODigis!\n\n";
            return kFATAL;
        }
        fArrayNEBVETODigis = (TClonesArray*)ioman->GetObject("NEBULAVETODigis"+OutputNameTag);
    }
    
    // Obtain simulation data (for NeuLAND):
    if (UseSimulation==kTRUE)
    {
        // Jan Mayers primary interaction points:
        if (MarkPrimPoints_Method=="JanPrimPoints")
        {
            if ((TClonesArray*)ioman->GetObject("NeulandPrimaryNeutronInteractionPoints") == nullptr)
            {
                cout << "I/O-manager FATAL: R3BNeuLANDTranslator::Init No NeulandPrimaryNeutronInteractionPoints!\n\n";
                return kFATAL;
            }
            fArrayPrims = (TClonesArray*)ioman->GetObject("NeulandPrimaryNeutronInteractionPoints");
        }
        else
        {
            fArrayPrims = 0;
        }
        
        // MC tracks:
        if ((TClonesArray*)ioman->GetObject("MCTrack") == nullptr)
        {
            cout << "I/O-manager FATAL: R3BNeuLANDTranslator::Init No MCTrack!\n\n";
            return kFATAL;
        }
        fArrayMCPoints = (TClonesArray*)ioman->GetObject("MCTrack");
        
        // MC points in NeuLAND:
        if ((TClonesArray*)ioman->GetObject("NeulandPoints") == nullptr)
        {
            cout << "I/O-manager FATAL: R3BNeuLANDTranslator::Init No NeulandPoints!\n\n";
            return kFATAL;
        }
        fArrayPoints = (TClonesArray*)ioman->GetObject("NeulandPoints");
        
        if (UseNEBULA==kTRUE)
        {
            if ((TClonesArray*)ioman->GetObject("NEBULAPoints") == nullptr)
            {
                cout << "I/O-manager FATAL: R3BNeuLANDTranslator::Init No NEBULAPoints!\n\n";
                return kFATAL;
            }
            fArrayNEBPoints = (TClonesArray*)ioman->GetObject("NEBULAPoints");
        }
        
        // Jan Mayers marked clusters:
        if (MarkSignals_Method=="JanMayer")
        {
            if ((TClonesArray*)ioman->GetObject("NeulandPrimaryClustersMultiple"+OutputNameTag) == nullptr)
            {
                cout << "I/O-manager FATAL: R3BNeuLANDTranslator::Init No NeulandPrimaryClustersMultiple!\n\n";
                return kFATAL;
            }
            fArrayJanClusters = (TClonesArray*)ioman->GetObject("NeulandPrimaryClustersMultiple"+OutputNameTag);
        }
    }
    else
    {
        fArrayPrims->Clear();
        fArrayMCPoints->Clear();
        fArrayPoints->Clear();
        fArrayJanClusters->Clear();
    }
    
    // Register output signals:
    ioman->Register("Signals"+OutputNameTag,"R3BSignal",fArraySignals,kTRUE);
    if (UseVETO==kTRUE) {ioman->Register("VETOSignals"+OutputNameTag,"R3BSignal",fArrayVETOSignals,kTRUE);}
    if (UseNEBULA==kTRUE) {ioman->Register("NEBULASignals"+OutputNameTag,"R3BSignal",fArrayNEBULASignals,kTRUE);}
    if (UseNEBVETO==kTRUE) {ioman->Register("NEBULAVETOSignals"+OutputNameTag,"R3BSignal",fArrayNEBVETOSignals,kTRUE);}
    
    // Register Monte Carlo Neutron Tracks:
    ioman->Register("MCNeutronTracks"+OutputNameTag,"TLorentzVector",fArrayMCNeutronTracks,kTRUE);
    ioman->Register("MCRecoilTracks"+OutputNameTag,"TLorentzVector",fArrayMCRecoilTracks,kTRUE);
    // NOTE: They will always be registered, so no conflict is obtained later on
    // in the software. But without using simulation data, they will remain empty (size==0).
    
    // Register multiplicities:
    ioman->Register("Gun_Multiplicity"+OutputNameTag,"ObjInteger",fArrayGunMult,kTRUE);
    ioman->Register("NeuLAND_Multiplicity"+OutputNameTag,"ObjInteger",fArrayNeuMult,kTRUE);
    ioman->Register("NEBULA_Multiplicity"+OutputNameTag,"ObjInteger",fArrayNEBMult,kTRUE);
    ioman->Register("Detected_Multiplicity"+OutputNameTag,"ObjInteger",fArrayDetMult,kTRUE);
    ioman->Register("NEBULA_Detected_Multiplicity"+OutputNameTag,"ObjInteger",fArrayNEBDetMult,kTRUE);
    
    // Initialize the neutron separation matrix:
    InitializeNeutronMatrix();
    
    // Initialize the control histograms:
    CreateControlHistograms();
    
    // Then, return the succes statement & reset counters:
    if (Inputs->ContainsNoErrors()==kFALSE) {Inputs->PrintAllErrors(); return kFATAL;}
    if (MinimizationMarking_AllowMaxDist==kFALSE) {MinimizationMarking_MaxDistance = 1e99;}
    EventCounter = 0;
    GunMult_Counter = 0;
    NeuLANDMult_Counter = 0;
    DetectedMult_Counter = 0;
    return kSUCCESS;
}

// FairTask Execute function:
void R3BNeuLANDTranslator::Exec(Option_t *option)
{
    // NOTE: FairTask will automatically load the inputs into the TClonesArrays.
    
    // Clean the output vectors:
    fArraySignals->Clear("C");
    fArrayVETOSignals->Clear("C");
    fArrayNEBULASignals->Clear("C");
    fArrayNEBVETOSignals->Clear("C");
    fArrayMCNeutronTracks->Clear("C");
    fArrayMCRecoilTracks->Clear("C");
    fArrayGunMult->Clear("C");
    fArrayNeuMult->Clear("C");
    fArrayNEBMult->Clear("C");
    fArrayDetMult->Clear("C");
    fArrayNEBDetMult->Clear("C");
    
    // Clean the current multiplicities:
    ThisGunMultiplicity = 0;
    ThisNeuLANDMultiplicity = 0;
    ThisNEBULAMultiplicity = 0;
    ThisDetectedMultiplicity = 0;
    ThisNEBDetectedMultiplicity = 0;
    
    // Clean other vectors:
    PrimIntPoints.clear();
    PrimIntPoints_TrackID.clear();
    
    // Fill the output Array with signals:
    TranslateSignals(fArrayDigis,fArraySignals);
    if (UseVETO==kTRUE) {TranslateSignals(fArrayVETODigis,fArrayVETOSignals);}
    if (UseNEBULA==kTRUE) {TranslateSignals(fArrayNEBULADigis,fArrayNEBULASignals);}
    if (UseNEBVETO==kTRUE) {TranslateSignals(fArrayNEBVETODigis,fArrayNEBVETOSignals);}
    
    // Next, we have to decide if we will use the simulation
    // data to mark some signals as primary ones:
    if (UseSimulation==kTRUE)
    {
        // In that case, begin with retrieving the primary neutron tracks:
        BuildPrimaryTracks();
        
        // The next step is then to retrieve the primary neutron interaction
        // points in NeuLAND:
        if (MarkPrimPoints_Method=="R3BMCTrack")
        {
            // Then, the primary neutron interaction points will be 
            // obtained directly from the R3BMCtrack Monte Carlo information:
            GetPrimaryInteractionPointsR3BMCTrack();
            cout << "R3BNeuLANDTranslator ERROR: The option R3BMCtrack does not obey #n->#n = p^# binominal distribution! This option is WRONG!\n";
            if (UseNEBULA==kTRUE) {cout << "R3BNeuLANDTranslator ERROR: The option R3BMCtrack does NOT support tracking in NEBULA!\n";}
        }
        else if (MarkPrimPoints_Method=="JanPrimPoints")
        {
            // Then, the primary neutron interaction points will be 
            // obtained from Jan Mayers NeulandPrimaryNeutronInteractionPoints:
            GetPrimaryInteractionPoints_Jan();
            if (UseNEBULA==kTRUE) {cout << "R3BNeuLANDTranslator ERROR: The option JanPrimPoints does NOT support tracking in NEBULA!\n";}
        }
        else if (MarkPrimPoints_Method=="LandPoint")
        {
            // Then, we obtain the primary neutron interaction points from 
            // the MC NeulandPoints, by tracing them back:
            GetPrimaryInteractionLandPoints();
        }
        else
        {
            // This is not programmed:
            cout << "R3BNeuLANDTranslator ERROR: The option <" << MarkPrimPoints_Method << "> was not programmed for obtaining the neutron prim. int. points!\n";
        }
        
        // Then, we have to mark the signals as primary/secondary based on the simulation data:
        if (MarkSignals_Method=="BackTracing")
        {
            // Then, we mark the signals by actually tracing to which MC track they belong:
            MarkSignals_BackTracing();
        }
        else if (MarkSignals_Method=="NewTracing")
        {
            // Then, we mark the signals by doing a signal backtracing followed by a minimization procedure.
            MarkSignals_NewTracing();
            if (UseNEBULA==kTRUE) {cout << "R3BNeuLANDTranslator ERROR: The option NewTracing does NOT support tracking in NEBULA!\n";}
        }
        else if (MarkSignals_Method=="Minimize")
        {
            // Then, we use our own old-fashioned minimization procedure
            // to link the most probable signals to the primary neutron interaction points:
            MarkSignals_MinimizeArray();
            if (UseNEBULA==kTRUE) {cout << "R3BNeuLANDTranslator ERROR: The option Minimize does NOT support tracking in NEBULA!\n";}
        }
        else if (MarkSignals_Method=="JanMayer")
        {
            // Then, we use the outcome of Jan Mayers R3BNeulandNeutronClusters class to
            // decide which signals to mark as primary/secondary signals:
            MarkSignals_UseJansCode();
            if (UseNEBULA==kTRUE) {cout << "R3BNeuLANDTranslator ERROR: The option JanMayer does NOT support tracking in NEBULA!\n";}
        }
        else
        {
            // This is not programmed:
            cout << "R3BNeuLANDTranslator ERROR: The option <" << MarkSignals_Method << "> was not programmed for marking the signals!\n";
        }
        
        // Then, we need to do some evaluation of our results:
        FillControlHistograms();
        UpdateSingleCounters();
        UpdateMultMatrix();
        
        // Done.
    }
    
    // Store multiplicities:
    // ATTENTION: These lines generate a memory leak. Size of the leak = 650 MB --> 1 GB in 275000 events with 10 digitizers.
    // So the leak is extremely subtile and will not cause many problems.
    new ((*fArrayGunMult)[0]) ObjInteger("Gun_Multiplicity",ThisGunMultiplicity);
    new ((*fArrayNeuMult)[0]) ObjInteger("NeuLAND_Multiplicity",ThisNeuLANDMultiplicity);
    new ((*fArrayNEBMult)[0]) ObjInteger("NEBULA_Multiplicity",ThisNEBULAMultiplicity);
    new ((*fArrayDetMult)[0]) ObjInteger("Detected_Multiplicity",ThisDetectedMultiplicity);
    new ((*fArrayNEBDetMult)[0]) ObjInteger("NEBULA_Detected_Multiplicity",ThisNEBDetectedMultiplicity);
    
    // Log progress:
    EventCounter = EventCounter + 1;
    if ((EventCounter%1000==0)&&(OutputNameTag=="")) {cout << "NeuLANDTranslator Log: We processed " << EventCounter << " events.\n";}
            
    // Delete all arrays that were generated:
    if ((IsPrimTrack!=0)&&(IsPrimTrack!=nullptr)) {delete[] IsPrimTrack;}
    if ((MinTOF!=0)&&(MinTOF!=nullptr)) {delete[] MinTOF;}
    if ((BarIndices!=0)&&(BarIndices!=nullptr)) {delete[] BarIndices;}
}

// After filling:
void R3BNeuLANDTranslator::FinishEvent()
{
    fArrayGunMult->Clear();
    fArrayNeuMult->Clear();
    fArrayDetMult->Clear();
    fArrayNEBMult->Clear();
    fArrayNEBDetMult->Clear();
}
        
// Definition of Finish-function:
void R3BNeuLANDTranslator::Finish()
{
    // Write histograms:
    TheOutputFile->cd();
    TString st = "";
    TString kstr = "";
    
    Multiplicity_Prim->Write("Multiplicity_FromNeuLANDPrimaries"+OutputNameTag,2);
    Multiplicity_Signal->Write("Multiplicity_FromR3BSignals"+OutputNameTag,2);
    Multiplicity_Gun->Write("Multiplicity_FromParticleGun"+OutputNameTag,2);
    PrimToSignal_Distance->Write("PrimToSignal_Distance"+OutputNameTag,2);
    PrimToSignal_Distance_Conditional->Write("PrimToSignal_Distance_Conditional"+OutputNameTag,2);
    AllTimes->Write("AllTimes"+OutputNameTag,2);
    PrimTimes->Write("PrimTimes"+OutputNameTag,2);
    
    if (DistanceCheck==kTRUE)
    {
        MCPrint->Write("MCPrint"+OutputNameTag);
        SignalPrint->Write("SignalPrint"+OutputNameTag);
    
        for (Int_t k = 0; k<PrimLines.size(); ++k)
        {
            kstr = st.Itoa(k,10);
            PrimLines.at(k)->Write("PrimTrack_"+kstr+OutputNameTag);
        }
    
        for (Int_t k = 0; k<MaxMultiplicity; ++k)
        {
            kstr = st.Itoa(k,10);
            MCPrint_Indu[k]->Write("MCPrint_Track"+kstr+OutputNameTag,2);
            SignalPrint_Indu[k]->Write("SignalPrint_Track"+kstr+OutputNameTag,2);
        }
    }
    
    // Write the neutron matrix to a separate text file:
    WriteMultMatrix();
    
    // Save histograms also in inputs-file:
    TString FileName = TheVMCFolder + "/DNN/InputFiles/Multiplicities.root";
    
    // First, see if our output-file already exists or not:
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
        // Then, recreate the file:
        f = new TFile(FileName.Data(),"recreate");
    }
    
    // Next, write the histograms to this file too:
    f->cd();
    Multiplicity_Prim->Write("Multiplicity_FromNeuLANDPrimaries"+OutputNameTag,2);
    Multiplicity_Signal->Write("Multiplicity_FromR3BSignals"+OutputNameTag,2);
    Multiplicity_Gun->Write("Multiplicity_FromParticleGun"+OutputNameTag,2);
    PrimToSignal_Distance->Write("PrimToSignal_Distance"+OutputNameTag,2);
    PrimToSignal_Distance_Conditional->Write("PrimToSignal_Distance_Conditional"+OutputNameTag,2);
    AllTimes->Write("AllTimes"+OutputNameTag,2);
    PrimTimes->Write("PrimTimes"+OutputNameTag,2);
    
    // Then, finish up:
    f->Close();
    delete f;
}

// Inclusion of other member functions:
#include "RoundOff.h"
#include "NeuClusters_AreIdentical.h"
#include "SetMT.h"
#include "InWhichNeuLANDBar.h"
#include "RetrieveInputs.h"
#include "TranslateSignals.h"
#include "BuildPrimaryTracks.h"
#include "GetPrimaryInteractionPointsR3BMCTrack.h"
#include "GetPrimaryInteractionLandPoints.h"
#include "GetPrimaryInteractionPoints_Jan.h"
#include "MarkSignals_BackTracing.h"
#include "MarkSignals_NewTracing.h"
#include "MarkSignals_MinimizeArray.h"
#include "MarkSignals_UseJansCode.h"
#include "MarkSignals_DummyJansCode.h"
#include "FillDistanceHistograms.h"
#include "Evaluation.h"
#include "WriteMultMatrix.h"

// Generate ROOT dictionary:
ClassImp(R3BNeuLANDTranslator);
