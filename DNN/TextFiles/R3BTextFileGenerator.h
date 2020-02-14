// Class that generates the TextFiles that contain the DNN 
// input and output files.

// General directives:
#ifndef R3BTEXTFILEGENERATOR_H
#define R3BTEXTFILEGENERATOR_H

// Include C++ headers:
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sys/stat.h>
#include <stdio.h>

// Include ROOT headers:
#include "TSystem.h"
#include "TROOT.h"
#include "TString.h"
#include "TFile.h"
#include "TMath.h"
#include "TClonesArray.h"
#include "TClass.h"
#include "TVector3.h"
#include "TH1D.h"
#include "TLorentzVector.h"
#include "TRandom3.h"

// Include R3B headers:
#include "FairRootManager.h"
#include "R3BInputClass.h"
#include "R3BSignal.h"
#include "R3BNeulandDigi.h"
#include "R3BSignalCluster.h"
#include "R3BSimpleCluster.h"
#include "Nuclei.h"
#include "NormalizationClass.h"
#include "AllScorers.h"
#include "ObjInteger.h"

// use standard namespace:
using namespace std;

// Define the class:
class R3BTextFileGenerator
{
public:
    // Constructor (Default):
    R3BTextFileGenerator();
    
    // Destructor (Default):
    virtual ~R3BTextFileGenerator();
    
    // initialization function for easy handling:
    Bool_t Initialize(Int_t const Local_nEvents);// Defines everything properly. It only requires FairRootManager* ioman and R3BInputClass* Inputs to be properly linked.
                                                // Then, in Exec()-function, all you have to do is call Fill_Network_IO() & WriteEvent() and possibly SetTargetPoint()
                                                // In case you wanted the target to be present as inputs. In the Finish()-function, only call FinishTextFile().
    
    // Functions to switch to the second reconstruction step:
    void SetSubFolder(TString const folder) {SubFolder = folder;} // Allows one to specify a different subfolder to write the .txt-files to. Is NOT called inside Initialize()!
    void SelectStep2(Int_t const DetMult) {Select_Determined_Multiplicity = DetMult; Step2_IsSelected = kTRUE;} // Switches to the second reconstruction step. Is NOT called inside Initialize()!
    void SelectStep1() {Step2_IsSelected = kFALSE;} // Switches back to the first reconstruction step (multiplicity). Is NOT called inside Initialize()!
    
    // Control functions:
    void LinkInputClass(R3BInputClass* inp);    // Links this class to the inputs class.                                                  // Call only once in beginning.
    void CreateFile();                          // Creates and opens the textfile. Format defines the type (FANN, TensorFlow, etc.)       // Call only once in beginning.
    void GenerateOneFile(TString const EventTag); // Called inside CreateFile(). actually generates the file, but without the tests of CreateFile().
    void LinkIOManager(FairRootManager* io);    // Provides the initialization function a FairRootManager to obtain links.                // Call only once in beginning.
    Bool_t LinkSignals();                       // Retrieves the Signals from the Fair IO-manager.                                        // Call only once in beginning.
    Bool_t LinkDigis();                         // Retrieves the Digis from the Fair IO-manager.                                          // Call only once in beginning.
    Bool_t LinkClusters();                      // Retrieves the Clusters from the Fair IO-manager.                                       // Call only once in beginning.
    Bool_t LinkFilteredClusters();              // Retrieves the Filtered Clusters from the Fair IO-manager.                              // Call only once in beginning.
    Bool_t LinkNeutronTracks();                 // Retrieves the MC Neutron Tracks from the Fair IO-manager.                              // Call only once in beginning.
    Bool_t LinkMultiplicities();                // Retrieves the 3 multiplicities (Gun, Neu, Det).                                        // Call only once in beginning.
    Bool_t LinkDNNMultiplicity();               // Retrieves the DNN predicted multiplicities.                                            // Call only once in beginning.
    void NormClassDefinition();                 // Defines the normalization classes required to transform the input parameters.          // Call only once in beginning.
    
    void SetCurrentDigiRun(Int_t const ThisRun);// Switches to a new DigiRun. Designed to switch over all DigiRuns per MC per event.      // NOTE: Call each event!
    void Fill_Network_IO();                     // Fills input/output of the DNN. Call this function before writing an event to the file. // NOTE: Call each event!
    void WriteEvent();                          // Writes a single event to the textFile.                                                 // NOTE: Call each event!
    void WriteEventNTimes(Int_t const nTimes);  // Writes a single event multiple times to the textFile.                                  // NOTE: Call each event!
    
    void NormClassFinish();                     // Finbishes up the normalization classes. It is called inside FinishTextFile().          // Call only once in the end.
    void FinishTextFile();                      // Close up the file after you are done.                                                  // Call only once in the end.
    
    // Processing of specific parameters:
    void SetBoolianOutputs(TString const Option);
    void SetBoolianInputs(TString const Option);
    void NormClassSelection(TString const Option);
    
    // IO definition functions:
    void IO_Default_Fillfunction();
    void IO_FillFunction_2to6_TradMed_BabyNetwork();
    void IO_FillFunction_3to6_TradMed_BabyNetwork();
    void IO_FillFunction_Signals_StandardNetwork();
    void IO_FillFunction_Signals_MultNetwork();
    void IO_FillFunction_MarkusPolleryd();
    void IO_FillFunction_ElenaHoemann();
    
    void FillOneCluster(R3BSignalCluster* ThisCluster);
    void FillOneEmptyCluster();
    void IO_FillFunction_ScoringPlus();
    void ProcessOneEvent_SingleCluster();
    
    // Error handling:
    void ErrorMessage(TString const Message);
    void PrintAllErrors();
    void DisableErrorPrinting();
    void EnableErrorPrinting();
    Bool_t ContainsNoErrors();
    
    // Other functions:
    TString RoundOff(Double_t const our_input, Int_t const significance);
    TString DoubleIntTotString(Double_t const Input);
    Bool_t DoesFileExist(const std::string &s);
    Double_t GetPMTPossition(R3BSignal* ThisSignal);
    void SetTheThreads(Int_t const nT, Int_t const cT);
    
    // Get-functions:
    TString GetFileFormat()            {return FileOption;}
    TString GetInformationFormat()     {return InfoUse;}
    Bool_t AreNeutronsLinked();         
    Bool_t AreSignalsLinked(Int_t const Index);  
    Bool_t AreDigisLinked(Int_t const Index);    
    Bool_t AreClustersLinked(Int_t const Index); 
    Bool_t AreFilteredClustersLinked(Int_t const Index);
    Bool_t AreMultsLinked(Int_t const Index);
    Bool_t AreDNNMultsLinked(Int_t const Index);
    std::vector<Int_t> GetStep2_MaxNClusters() {return Step2_MaxNClusters;}
    
    // Other functions:
    
private:
    // Class content:
    
    // TClonesArray so that the class gains direct access to the objects:
    FairRootManager* ioman;                     Bool_t IO_Manager_IsLinked;
    TClonesArray** fArraySignals;               Bool_t* Signals_IsLinked;
    TClonesArray** fArrayDigis;                 Bool_t* Digis_IsLinked;
    TClonesArray** fArrayClusters;              Bool_t* Clusters_IsLinked;
    TClonesArray** fArrayFilteredClusters;      Bool_t* Filtered_IsLinked;
    TClonesArray* fArrayNeutronTracks;          Bool_t NeutronTracks_IsLinked;
    TClonesArray** fArrayDNNMult;               Bool_t* DNNMults_AreLinked;
    TClonesArray** fArrayGunMult;               Bool_t* Mults_AreLinked;
    TClonesArray** fArrayNeuMult;
    TClonesArray** fArrayDetMult;
    Int_t nDigiRuns;
    Int_t CurrentDigiRun;
    
    // Class vectors for DNN inputs & outputs:
    Int_t Nevents;
    Int_t N_inputs;
    Int_t N_outputs;
    std::vector<Double_t> InputNeurons;
    std::vector<Double_t> OutputNeurons;
    std::vector<Double_t> Tags;
    Double_t Input_ISYES;
    Double_t Input_ISNO;
    Double_t Output_ISYES;
    Double_t Output_ISNO;
    
    // Information that should be used in the network:
    TString InfoUse_Step2;      // NOTE: Specifies the network structure for the cluster identification.
    TString InfoUse;            // NOTE: "", "IO_2to6_TradMed_BabyNetwork", "IO_3to6_TradMed_BabyNetwork", "IO_Signals_StandardNetwork", ...
    Int_t NmaxSignals;          // NOTE: Number of digitizer signals where we place our cutoff.
    Int_t NmaxClusters;         // NOTE: Number of digitizer signals where we place our cutoff.
    
    // TextFile:
    TString SubFolder;
    TString FileName;
    std::ofstream TheTextFile;
    std::ofstream OutTextFile;
    TString FileOption;         // NOTE: "", "FANN", ...
    Bool_t FileTest;
    Int_t EventCounter;
    Int_t FillCounter;
    Int_t BatchCounter;
    Int_t IO_Significance;
    Bool_t ValidationMode;
    
    // Inclusion of the target:
    Bool_t CompressBatches;
    Bool_t IncludeTarget;
    Double_t Target_X;
    Double_t Target_Y;
    Double_t Target_Z;
    Double_t Target_T;
    Double_t NeuLAND_Center_X;
    Double_t NeuLAND_Center_Y;
    Double_t NeuLAND_Front_Z;
    Double_t NeuLAND_Rot_X;
    Double_t NeuLAND_Rot_Y;
    Double_t NeuLAND_Rot_Z;
    Double_t NeuLAND_Active_Bar_Thickness;
    Double_t NeuLAND_Total_Bar_Length;
    Double_t NeuLAND_TotalBarThicknessXY;
    Double_t NeuLAND_TotalBarThicknessZ;
    
    // Error handling:
    Bool_t PrintErrors;           
    std::vector<TString> Errors;  
    Bool_t ConsistencyCheck;
    
    // Input parameters:
    R3BInputClass* Inputs;
    Nuclei* TheNuclei;
    Int_t MaxMultiplicity;
    Int_t NbarsPerPlane;
    Int_t NDoublePlanes;
    Double_t NeutronMass;
    Bool_t MarkusFormat_AddRestNeutrons;
    Int_t EventBatch;
    Bool_t SimulationData_IsAvailable;
    TString PreselectionMethod;
    TString ParticleType;
    TString Step2_MultiplicityChoice;
    Double_t TimeNormalization_LeftBoundary;
    Double_t TimeNormalization_RightBoundary;
    TString TimeNormalization_Option;
    TString TimeNormalization_NormName;
    TString ParentOutputPath;
    Bool_t ComputeSingleClusterRatio;
    Bool_t DataTimeNormaliation_UsePrimTimes;
    
    // Scoring+ method:
    AllScorers* TheScorers;
    TH1D* TotalClusterScore_All;
    TH1D* TotalClusterScore_Prim;
    
    // Second reconstruction step:
    Int_t Select_Determined_Multiplicity;
    Bool_t Step2_IsSelected;
    std::vector<Int_t> Step2_MaxNClusters;
    
    // Multithreading:
    Int_t NumberOfThreads;
    Int_t CurrentThread;
    
    // Elena Hoemann network options:
    Bool_t Include_HorPaddleCoordinate;
    Bool_t Include_ZeroMult_Output;
    Bool_t Include_ZeroMultEvents;
    Bool_t Include_Multiplicity_MisAligned_Events;
    Bool_t Include_SigneHitClusters;
    Bool_t Include_TotalScores;
    Bool_t Include_SignalScores;
    Bool_t Include_SignalBetas;
    Bool_t Eliminate_Tails_InNormalization;
    Bool_t CutOff_ClusterVector;
    Bool_t ReShuffle_ClusterVector;
    Bool_t Kickout_EventsWithLostPrimClusters;
    Bool_t Step2_UseDetectedMultiplicity;
    Int_t nOutputFlags_PerCluster;
    
    // Normalization classes:
    NormalizationClass* DefaultNorms;       // Applies the normalizations from the default settings.
    NormalizationClass* LearnAdvancedNorms; // Learns to adapt the required norms during the loop.
    NormalizationClass* ApplyAdvancedNorms; // Applies the norms learned from a previous loop.
    NormalizationClass* TheNormalization;   // Pointer to one of the above three classes: the one we are using.
    NormalizationClass* ElenaNorms;         // Applies the normalizations consistent with Elena's normalizations.
    Bool_t ExecuteNormalization;            // Decides whether we actually apply the normalization or not.
    
    // Generate ROOT dictionary:
    ClassDef(R3BTextFileGenerator, 1);
};

#endif
