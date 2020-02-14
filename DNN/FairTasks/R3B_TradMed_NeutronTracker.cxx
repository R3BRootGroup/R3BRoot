// Traditional method neutron reconstruction.

// Beginning stuff:
#include "R3B_TradMed_NeutronTracker.h"
using namespace std;
ClassImp(R3B_TradMed_NeutronTracker)

// Declaration of global variables:
Double_t fTarget_Xpos_global = 0.0;
Double_t fTarget_Ypos_global = 0.0;
Double_t fTarget_Zpos_global = 0.0;
Double_t fBeam_Xpos_global = 0.0;
Double_t fBeam_Ypos_global = 0.0;
Double_t fBeam_Zpos_global = 0.0;
Double_t c = 29.9792458;
Double_t gBeamBeta;
bool AuxSortClustersBeta(R3BSignalCluster*, R3BSignalCluster*);

// inclusion of other stuff used in the reconstruction:
#include "TradMedReconstruction.h"
#include "TradMedMultiplicity.h"
#include "PerfectMultiplicity.h"
#include "GetDNNMultiplicity.h"

// Default constructor:
R3B_TradMed_NeutronTracker::R3B_TradMed_NeutronTracker() : FairTask("R3B NeuLAND Neutron Tracker")
{   
    // Initialisation of the TClonesArrays:
    fArrayCluster = new TClonesArray("R3BSignalCluster");
    fNeutHits = new TClonesArray("R3BSignal");
    fArrayMult = new TClonesArray("ObjInteger");
    TheOutputFile = 0;
    
    // Parameters:
    MultiplicityDetermination = "Cuts";
    fTarget_Xpos = 0.0;
    fTarget_Ypos = 0.0;
    fTarget_Zpos = 0.0;
    fBeam_Xpos = 0.0;
    fBeam_Ypos = 0.0;
    fBeam_Zpos = 0.0;
    EventCounter = 0;
    MaxMultiplicity = 0;
    IntendedMultiplicity = 0;
    FoundMultiplicity = 0;
    UseCalibrationCuts = kFALSE;
    ParticleType = "neutron";
    ParticleMass = 932.0; // MeV.
    mNeutron = 1.0; // amu.
    cMedia = 14.0;
    dio = 10.6; 
    nBarsTotal = 3000;
    
    // constants of physics:
    amu = 931.494028; // atomic mass unit in MeV/c**2
    
    // Intermediate values:
    fVectorCluster.clear();
    fNofClusters = 0;
    fNofClusters1 = 0;
    fNofTracks = 0;
    
    // Nuclear Database:
    TheNuclei = new Nuclei();
    TheNuclei->BuildDataBase();
    nEvents = 1;
}

// Destructor:
R3B_TradMed_NeutronTracker::~R3B_TradMed_NeutronTracker() 
{
    if(fArrayCluster) {fArrayCluster->Clear(); delete fArrayCluster;}
    if(fNeutHits)     {fNeutHits->Clear(); delete fNeutHits;}
    if(fArrayMult)    {fArrayMult->Clear(); delete fArrayMult;}
}

// Init-function:
InitStatus R3B_TradMed_NeutronTracker::Init()
{
    // Obtain the I/O manager:
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman)
    {
        cout << "I/O-manager FATAL: R3B_TradMed_NeutronTracker::Init: No FairRootManager\n\n";
        return kFATAL;
    }
    TheOutputFile = ioman->GetOutFile();
    
    // Check the inputs class:
    if (Inputs->IsLinked()==kFALSE)
    {
        cout << "I/O-manager FATAL: The R3BInputClass is not linked to a file!\n\n";
        return kFATAL;
    }
    
    // Retrieve the required inputs:   
    TString CalFileName = Inputs->GetInputString("TheOutputPath");
    CalFileName = CalFileName + "/CutsFile.txt";
    fTarget_Xpos = Inputs->GetInputDouble("TARGET_center_x_position","cm");
    fTarget_Ypos = Inputs->GetInputDouble("TARGET_center_y_position","cm");
    fTarget_Zpos = Inputs->GetInputDouble("TARGET_center_z_position","cm");    
    fBeam_Xpos = Inputs->GetInputDouble("ParticleGun_x_position","cm");
    fBeam_Ypos = Inputs->GetInputDouble("ParticleGun_y_position","cm");
    fBeam_Zpos = Inputs->GetInputDouble("ParticleGun_z_position","cm");
    TString BeamType = Inputs->GetInputString("ParticleGun_ParticleType");
    MaxMultiplicity = Inputs->GetInputInteger("ParticleGun_Multiplicity");
    beamEnergy = Inputs->GetAvgBeamEnergy();
    beamBeta = Inputs->GetAvgBeamBeta();
    ParticleType = Inputs->GetInputString("NeuLAND_ParticleType_ToBeDetected");
    ParticleMass = TheNuclei->GetMass(ParticleType,"MeV");
    mNeutron = TheNuclei->GetMass(ParticleType,"u"); // Particle mass in amu.
    cMedia = Inputs->GetInputDouble("NeuLAND_Digitizer_Effective_LightSpeed","cm/ns");
    dio = Inputs->GetTotalPaddleThickness()*TMath::Sqrt(2.0)*1.5; // ATTENTION: Has to be 3 times half the diagonal of a paddle thickness.
    nBarsTotal = 2*Inputs->GetInputInteger("NeuLAND_Number_of_Bars_per_plane")*Inputs->GetInputInteger("NeuLAND_Number_of_DoublePlanes");
    
    // Initialize in case we wanted to use a calibration file.
    ReadCalibrationFile();
    
    // Obtain the clusters:
    if ((TClonesArray*)ioman->GetObject("Clusters") == nullptr)
    {
        cout << "I/O-manager FATAL: R3B_TradMed_NeutronTracker::Init No R3BSignalClusters!\n\n";
        return kFATAL;
    }
    fArrayCluster = (TClonesArray*)ioman->GetObject("Clusters");  
    
    // Obtain the DNN multiplicity:
    if ((TClonesArray*)ioman->GetObject("DNN_Multiplicity") == nullptr)
    {
        cout << "I/O-manager FATAL: R3B_TradMed_NeutronTracker::Init No DNN Multiplicity!\n\n";
        return kFATAL;
    }
    fArrayMult = (TClonesArray*)ioman->GetObject("DNN_Multiplicity");

    // Write the primary neutrons as outputs:
    TString RegistrationName = "PrimaryHits_TradMed_Clusters_";
    if      (MultiplicityDetermination=="Cuts")    {RegistrationName = RegistrationName + "CutsMult";}
    else if (MultiplicityDetermination=="Perfect") {RegistrationName = RegistrationName + "PerfectMult";}
    else if (MultiplicityDetermination=="DNN")     {RegistrationName = RegistrationName + "DNNMult";}
    else                                           {RegistrationName = RegistrationName + "CutsMult";}
    ioman->Register(RegistrationName,"R3BSignal",fNeutHits,kTRUE);
  
    // Create control histograms:   
    IntendedMultiplicity = new TH1D(RegistrationName+"Intended_Multiplicity",RegistrationName+"Intended_Multiplicity",MaxMultiplicity+1,0.0,((Int_t) (MaxMultiplicity+1)));
    FoundMultiplicity = new TH1D(RegistrationName+"Found_Multiplicity",RegistrationName+"Found_Multiplicity",MaxMultiplicity+1,0.0,((Int_t) (MaxMultiplicity+1)));
    
    // Pass values to global variables:
    fTarget_Xpos_global = fTarget_Xpos;
    fTarget_Ypos_global = fTarget_Ypos;
    fTarget_Zpos_global = fTarget_Zpos;
    fBeam_Xpos_global = fBeam_Xpos;
    fBeam_Ypos_global = fBeam_Ypos;
    fBeam_Zpos_global = fBeam_Zpos;
    gBeamBeta = beamBeta;
    
    // Finish up:
    if (Inputs->ContainsNoErrors()==kFALSE) {Inputs->PrintAllErrors(); return kFATAL;}
    EventCounter = 0;
    return kSUCCESS;
}

// Execute-function:
void R3B_TradMed_NeutronTracker::Exec(Option_t* opt)
{
    // Reset everything:
    Reset();

    // NOTE: Obtain the multiplicity:
    if (MultiplicityDetermination=="Cuts")
    {
        nNeut = ApplyCalibrationCuts();
    }
    else if (MultiplicityDetermination=="Perfect")
    {
        nNeut = PerfectMultiplicity();
    }
    else if (MultiplicityDetermination=="DNN")
    {
        nNeut = GetDNNMultiplicity();
    }
    else
    {
        nNeut = ApplyCalibrationCuts();
    }
  
    // Fill histograms:
    IntendedMultiplicity->Fill((Int_t) nNeut);

    // Perform the reconstruction:
    if (nNeut>0) 
    {
        AdvancedMethod(); // NOTE: This is where the actual magic happens!
    }
    
    // Give output:
    EventCounter = EventCounter + 1;
    
    if ((EventCounter%1000)==0) 
    {
        cout << "-I- R3B_TradMed_NeutronTracker: We processed " << EventCounter << "/" << nEvents << " events.\n";
    }
}

// Reset-function:
void R3B_TradMed_NeutronTracker::Reset()
{
    // Clean the array with primaries:
    fNeutHits->Clear();
    
    // Other cleaning:
    if(fVectorCluster.size() > 0) {fVectorCluster.clear();}
    fNofClusters = 0;
    fNofClusters1 = 0;
    fNofTracks = 0;
}   

// Finish-function:
void R3B_TradMed_NeutronTracker::Finish()
{
    // Write control histograms:
    TheOutputFile->cd();
    
    TString WriteName = IntendedMultiplicity->GetName();
    IntendedMultiplicity->Write(WriteName.Data(),2);
    
    WriteName = FoundMultiplicity->GetName();
    FoundMultiplicity->Write(WriteName.Data(),2);
}
