// FairTask that translates the NeuLAND digis into general signals.
// Each detector should have its own translator, so that the DNN 
// software can be applied to general signals.
// Written by C. A. Douma.
#ifndef R3BNEULANDTRANSLATOR_H
#define R3BNEULANDTRANSLATOR_H

// Inclusion of C++ headers:
#include <iostream>
#include <fstream>

// Inclusion of ROOT headers:
#include "TLorentzVector.h"
#include "TClonesArray.h"
#include "TMath.h"
#include "TString.h"
#include "TFile.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TH1I.h"
#include "TH1D.h"
#include "TH3D.h"
#include "TVector3.h"
#include "TPolyLine3D.h"

// Inclusion of R3BRoot headers:
#include "FairRootManager.h"
#include "FairTask.h"
#include "FairMCPoint.h"
#include "../../r3bdata/R3BMCTrack.h"
#include "../../r3bdata/neulandData/R3BNeulandPoint.h"
#include "R3BCADNeulandPoint.h"
#include "R3BCADNEBULA.h"
#include "R3BNeulandDigi.h"
#include "R3BNeulandCADHit.h"
#include "R3BNeulandCADCluster.h"
#include "R3BSignal.h"
#include "R3BInputClass.h"
#include "ObjInteger.h"
#include "Nuclei.h"

using namespace std;

class R3BNeuLANDTranslator : public FairTask 
{
    public:
        // Default constructor:
        R3BNeuLANDTranslator();

        // Destructor:
        virtual ~R3BNeuLANDTranslator();

        // Task initialization:
        virtual InitStatus Init();

        // Process a single event:
        virtual void Exec(Option_t *option);
        
        // Tasks to be done AFTER writing the event:
        virtual void FinishEvent();

        // Finishing the task:
        virtual void Finish();
        
        // Linking functions for inputs:
        void LinkInputClass(R3BInputClass* inp) {Inputs = inp;}
        void SetNevents(Int_t const nn) {nEvents = nn;}
        
        // NOTE: The Digitizer-file should be regarded
        // as the FairTask input-file. is UseSimulation==kFALSE,
        // this is all that needs to be done. If 
        // UseSimulation==kTRUE is called, the FairTask
        // needs the MonteCarlo sim-file as inputs too.
        // this should be done by an ->AddFried(TString) call.
        
        // Control Member functions:
        void RetrieveInputs();                  // Use in Init()-function.
        void CreateControlHistograms();         // Use in Init()-function.
        void InitializeNeutronMatrix();         // Use in Init()-function.
        
        void TranslateSignals(TClonesArray* fDigiArray, TClonesArray* fSignalArray); // Use inExec()-function. Can be used for multiple translations.
        void BuildPrimaryTracks();              // Use in Exec()-function.
        void GetPrimaryInteractionPointsR3BMCTrack(); // Use in Exec()-function.
        void GetPrimaryInteractionLandPoints(); // Use in Exec()-function.
        void GetPrimaryInteractionPoints_Jan(); // Use in Exec()-function.
        void MarkSignals_BackTracing();         // Use in Exec()-function.
        void MarkSignals_NewTracing();          // Use in Exec()-function.
        void MarkSignals_MinimizeArray();       // Use in Exec()-function.
        void MarkSignals_UseJansCode();         // Use in Exec()-function.
        void MarkSignals_DummyJansCode();       // Use in Exec()-function.
        void FillControlHistograms();           // Use in Exec()-function.
        void FillDistanceHistograms();          // Use in Exec()-function.
        void UpdateSingleCounters();            // Use in Exec()-function.
        void UpdateMultMatrix();                // Use in Exec()-function.
        
        void WriteMultMatrix();                 // Use in Finish()-function.
        
        // Other member functions:
        Int_t InWhichNeuLANDBar(Double_t const xpoint_c, Double_t const ypoint_c, Double_t const zpoint_c);
        TString RoundOff(Double_t const our_input, Int_t const significance);
        void SetMT(Int_t const nT, Int_t const cT);
        Bool_t NeuClusters_AreIdentical(R3BNeulandCADCluster* First, R3BNeulandCADCluster* Second);
        
        // Multiple digitizer runs:
        void SetTag(TString const Tag) {OutputNameTag = Tag;}

    private:
        // Class content:
        TClonesArray* fArrayMCNeutronTracks;// Array of the Monte Carlo neutron tracks fired by the particle gun.
        TClonesArray* fArrayMCRecoilTracks; // Array of the Monte Carlo non-neutron tracks fired by the particle gun.
        TClonesArray* fArraySignals;        // Array to store the translated signals in for NeuLAND.
        TClonesArray* fArrayVETOSignals;    // Array to store the translated signals in for the NeuLAND VETO.
        TClonesArray* fArrayNEBULASignals;  // Array to store the translated signals in for NEBULA.
        TClonesArray* fArrayNEBVETOSignals; // Array to store the translated signals in for the NEBULA VETO.
        
        TClonesArray* fArrayDigis;          // Array to store the NeuLAND digis in.
        TClonesArray* fArrayVETODigis;      // Array to store the NeuLAND VETO digis in.
        TClonesArray* fArrayNEBULADigis;    // Array to store the NEBULA digis in.
        TClonesArray* fArrayNEBVETODigis;   // Array to store the NEBULA VETO digis in.
        TClonesArray* fArrayPrims;          // Array to store the simulated primary interaction points in.
        TClonesArray* fArrayMCPoints;       // Array to store the MC tracks of the simulation in.
        TClonesArray* fArrayPoints;         // Array to store the NeuLAND interaction points in.
        TClonesArray* fArrayNEBPoints;      // Array to store the NEBULA interaction points in.
        TClonesArray* fArrayJanClusters;    // Array to store the NeuLAND primary clusters in, as determined by Jan Mayer.
        
        TClonesArray* fArrayGunMult;        // Multiplicity as fired by the particle gun.
        TClonesArray* fArrayNeuMult;        // Multiplicity as determined from the number of MC first interaction points in NeuLAND.
        TClonesArray* fArrayNEBMult;        // Multiplicity as determined from the number of MC first interaction points in NEBULA.
        TClonesArray* fArrayDetMult;        // Multiplicity from the number of primary-marked signals in NeuLAND.
        TClonesArray* fArrayNEBDetMult;     // Multiplicity from the number of primary-marked signals in NEBULA.
        
        // Control histograms:
        TH1I* Multiplicity_Prim;            // obtains the multiplicity histogram directly from the NeuLAND primaries.
        TH1I* Multiplicity_Signal;          // obtains the multiplicity histogram from the translated signals.
        TH1I* Multiplicity_Gun;             // obtains the multiplicity histogram as fired by the particle gun.
        TH1D* PrimToSignal_Distance;        // Distance between the neutron primary interaction point (MC) and the corresponding marked signal.
        TH1D* PrimToSignal_Distance_Conditional; // same, but now with a cut.
        TH1D* AllTimes;                     // Signal times of all signals.
        TH1D* PrimTimes;                    // Signal times of the primary-marked signals by the simulation.
        TH3D* MCPrint;                      // Single event prints for MC points.
        TH3D** MCPrint_Indu;                // Single event prints for MC points (per track).
        TH3D** SignalPrint_Indu;            // Single event prints for signals (per track).
        TH3D* SignalPrint;                  // Signle event prints for signals.
        std::vector<TPolyLine3D*> PrimLines;// contain primary tracks corresponding to the event of MCPrint and SignalPrint.
        Bool_t DistanceCheck;               // Decides whether or not, we will do a lot more work for monitoring the distance.
        Bool_t FillTheDistanceHistograms;   // Skips filling the distance histograms.
        
        // Input parameters:
        Bool_t UseVETO;                     // Whether or not the VETO is in the setup.
        Bool_t UseNEBULA;                   // Whether or not NEBULA is in the setup.
        Bool_t UseNEBVETO;                  // Whether or not the NEBULA VETO is in the setup.
        TString TheVMCFolder;               // Folder of R3BRoot source code.
        Int_t MaxClusterMult;               // When we use Jan Mayers clusters, this is the max. allowed multiplicity per cluster.
        TString TheOutputPath;              // Output path where result files are written to.
        Bool_t UseSimulation;               // This tag is kFALSE if no simulation data is available and kTRUE if it is.
        Double_t ceff;                      // Effective speed of light in NeuLAND medium; used to link signals to primaries.
        Int_t MaxMultiplicity;              // Upper boundary of multiplicity histogram.
        Int_t nPaddlesPerPlane;             // Number of paddles per single NeuLAND plane.
        Int_t nDoublePlanes;                // Number of NeuLAND Double Planes.
        TString ParticleType_ToBeDetected;  // Particle that should be detected. for NeuLAND, these are neutrons.
        Double_t ParticleMass;              // Mass in MeV of ParticleType_ToBeDetected.
        Int_t ParticlePDG;                  // PDG of ParticleType_ToBeDetected;
        TString MarkSignals_Method;         // Holds the method used to mark the primary signals.
        TString MarkPrimPoints_Method;      // Holds the method for identifying the primary interaction points.
        Double_t MinimizationMarking_MaxDistance; // Max. Distance between the signal and prim. int. point that is still allowed to link them.
        Bool_t MinimizationMarking_IncludeTime; // Decides whether or not the ceff*Delta(Time) is added to the minimization distance or not.
        Bool_t MinimizationMarking_AllowMaxDist; // If this is kFALSE, The max. distance for minimization is put to infinity, so we simply search for the smallest possible one.
        Double_t Target_PrimVolume_Thickness; // if a track starts within +/- this of the target location, it is a primary track.
        Double_t TargetX;                   // Location of the target.
        Double_t TargetY;                   // Location of the target.
        Double_t TargetZ;                   // Location of the target.
        Double_t TargetT;                   // Location of the target.
        Double_t NeuLAND_Center_X;          // NeuLAND geometry parameter.
        Double_t NeuLAND_Center_Y;          // NeuLAND geometry parameter.
        Double_t NeuLAND_Front_Z;           // NeuLAND geometry parameter.
        Double_t NeuLAND_Rot_X;             // NeuLAND geometry parameter.
        Double_t NeuLAND_Rot_Y;             // NeuLAND geometry parameter.
        Double_t NeuLAND_Rot_Z;             // NeuLAND geometry parameter.
        Double_t NeuLAND_TotalBarThicknessXY; // NeuLAND geometry parameter.
        Double_t NeuLAND_TotalBarThicknessZ; // NeuLAND geometry parameter.
        Double_t NeuLAND_Active_Bar_Thickness; // NeuLAND geometry parameter.
        Double_t NeuLAND_Total_Bar_Length; // NeuLAND geometry parameter.
        Int_t NumberOfDoublePlanes_PerModule; // NeuLAND geometry parameter.
        Bool_t DoModularSplit;              // NeuLAND geometry parameter.
        Double_t ModularDistance;           // NeuLAND geometry parameter. 
        Double_t BeamBeta;                  // Avgerage relativistic beta of the beam.
        Double_t ParticleGun_x_position;    // x-position of the particle gun.
        Double_t ParticleGun_y_position;    // y-position of the particle gun.
        Double_t ParticleGun_z_position;    // z-position of the particle gun.
        
        // Verification parameters & Counters:
        Int_t EventCounter;                 // Counts events for logging progress.
        Int_t nEvents;                      // Total number of events that is processed.
        Int_t GunMult_Counter;              // Counts the number of events where the particle gun fired 1 particle,
        Int_t NeuLANDMult_Counter;          // Counts the number of events where 1 particle interacted with NeuLAND.
        Int_t DetectedMult_Counter;         // Counts the number of events where a 1 particle multiplicity was detected.
        Int_t ThisGunMultiplicity;          // The number of primary neutron tracks fired from the target in the current event.
        Int_t ThisNeuLANDMultiplicity;      // The number of primary neutron tracks that interacted with NeuLAND in the current event.
        Int_t ThisDetectedMultiplicity;     // The number of primary neutron tracks that were detected by NeuLAND in the current event.
        Int_t ThisNEBULAMultiplicity;       // The number of primary neutron tracks that interacted with NEBULA in the current event.
        Int_t ThisNEBDetectedMultiplicity;  // The number of primary neutron tracks that were detected by NEBULA in the current event.
        Double_t** MultMatrix;              // Neutron separation matrix: gun/detected.
        Double_t** InteractionMatrix;       // Neutron separation matrix: gun/NeuLAND.
        Double_t** DetectionMatrix;         // Neutron separation matrix: NeuLAND/detected.
        Double_t** MultNEBULAMatrix;        // Neutron separation matrix: gun/detected.
        Double_t** InteractionNEBULAMatrix; // Neutron separation matrix: gun/NeuLAND.
        Double_t** DetectionNEBULAMatrix;   // Neutron separation matrix: NeuLAND/detected.
        Double_t** MultDoubleMatrix;        // Neutron separation matrix: gun/detected.
        Int_t TotalNumberOfThreads;         // Keeps track of the MT simulation. Only used to store the neutron separation matrix.
        Int_t CurrentThread;                // Keeps track of the MT simulation. Only used to store the neutron separation matrix.
        Bool_t* IsPrimTrack;                // Identifies the primary tracks in the R3BMCTrack for the current event. Generated in BuildPrimaryTracks().
        Double_t* MinTOF;                   // Identifies the min. TOF interaction point of the primary track with NeuLAND. Generated in BuildPrimaryTracks(). 
        Int_t* BarIndices;                  // Contains the bar in which the min TOF interaction point of the primary track with NeuLAND is located. Generated in BuildPrimaryTracks(). 
        std::vector<TLorentzVector> PrimIntPoints; // Contains time and place of the primary interaction points.
        std::vector<Int_t> PrimIntPoints_TrackID; // Contaisn the trackID of the corresponding primary R3BMCTrack to PrimIntPoints.
             
        // Auxillary classes:
        R3BInputClass* Inputs;
        Nuclei* TheNuclei;
        TFile* TheOutputFile;
        TString OutputNameTag;
    
    public:
        // Generation of ROOT dictionary:
        ClassDef(R3BNeuLANDTranslator, 1);
};

#endif
