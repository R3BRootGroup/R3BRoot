// Include this class header:
#include "R3B_DNN_Plots.h"

// Default Constructor:
R3B_DNN_Plots::R3B_DNN_Plots()
{
    OutPutPath = "./";
    ScaleFactor = 4.0;
    LogScaleMinimum = 0.3;
    
    MCFile = 0;          MCFile_Exists = kFALSE;
    ParFile = 0;         ParFile_Exists = kFALSE;
    VisFile = 0;         VisFile_Exists = kFALSE;
    DigiFile = 0;        DigiFile_Exists = kFALSE;
    JanFile = 0;         JanFile_Exists = kFALSE;
    SignalFile = 0;      SignalFile_Exists = kFALSE;
    ClusterFile = 0;     ClusterFile_Exists = kFALSE;
    FilteredFile = 0;    FilteredFile_Exists = kFALSE;
    TradMedFile = 0;     TradMedFile_Exists = kFALSE;
    ScoreFile = 0;       ScoreFile_Exists = kFALSE;
    DistFile = 0;        DistFile_Exists = kFALSE;
    DNNFile = 0;         DNNFile_Exists = kFALSE;
    FinalFile = 0;       FinalFile_Exists = kFALSE;
    
    Include_ErelSpectrum_MCData = kTRUE;
    Include_ErelSpectrum_Perfect_Signals = kTRUE;
    Include_ErelSpectrum_Perfect_Clusters = kTRUE;
    Include_ErelSpectrum_TradMed_Clusters_CutsMult = kTRUE;
    Include_ErelSpectrum_TradMed_Clusters_DNNMult = kTRUE;
    Include_ErelSpectrum_TradMed_Clusters_PerfectMult = kTRUE;
    Include_ErelSpectrum_ScoringPlus_Signals_CutsMult = kTRUE;
    Include_ErelSpectrum_ScoringPlus_Signals_DNNMult = kTRUE;
    Include_ErelSpectrum_ScoringPlus_Signals_PerfectMult = kTRUE;
    Include_ErelSpectrum_ScoringPlus_Clusters_CutsMult = kTRUE;
    Include_ErelSpectrum_ScoringPlus_Clusters_DNNMult = kTRUE;
    Include_ErelSpectrum_ScoringPlus_Clusters_PerfectMult = kTRUE;
    Include_ErelSpectrum_DNNScoringPlus = kTRUE;
    Include_ErelSpectrum_DNNScoringPlus_Max = kTRUE;
    
    DNNTextFile_Exists = kFALSE;
    InputsFile_Exists = kFALSE;
    CutsFile_Exists = kFALSE;
    
    PrintErrors = kTRUE;
    Errors.clear();
    
    Inputs = 0;
}
    
// Destructor:
R3B_DNN_Plots::~R3B_DNN_Plots() 
{
    delete MCFile;
    delete ParFile;
    delete VisFile;
    delete DigiFile;
    delete JanFile;
    delete SignalFile;
    delete ClusterFile;
    delete FilteredFile;
    delete TradMedFile;
    delete ScoreFile;
    delete DistFile;
    delete DNNFile;
    delete FinalFile;
    
    MCFile = 0;          MCFile_Exists = kFALSE;
    ParFile = 0;         ParFile_Exists = kFALSE;
    VisFile = 0;         VisFile_Exists = kFALSE;
    DigiFile = 0;        DigiFile_Exists = kFALSE;
    JanFile = 0;         JanFile_Exists = kFALSE;
    SignalFile = 0;      SignalFile_Exists = kFALSE;
    ClusterFile = 0;     ClusterFile_Exists = kFALSE;
    FilteredFile = 0;    FilteredFile_Exists = kFALSE;
    TradMedFile = 0;     TradMedFile_Exists = kFALSE;
    ScoreFile = 0;       ScoreFile_Exists = kFALSE;
    DistFile = 0;        DistFile_Exists = kFALSE;
    DNNFile = 0;         DNNFile_Exists = kFALSE;
    FinalFile = 0;       FinalFile_Exists = kFALSE;
    
    DNNTextFile.close();
    InputsFile.close();
    CutsFile.close();
    
    Errors.clear();
}

// Member function definitions:
#include "ErrorMessage.h"
#include "LinkInputClass.h"
#include "CheckFiles.h"
#include "DrawHistDouma.h"
#include "PutErrorsToZero.h"
#include "ErelSpect.h"
#include "AddErelPlot.h"
#include "CompareHist.h"
#include "Plot2Hists.h"
#include "Plot3HistsSum.h"
#include "EdepHists.h"
#include "ScatterHists.h"
#include "DrawGraphDouma.h"
#include "Plot2Graphs.h"
#include "CompareGraph.h"
#include "Compare3Graphs.h"
#include "OneScatterHist.h"
#include "OneEdepHist.h"

// Generate ROOT dictionary:
ClassImp(R3B_DNN_Plots);
