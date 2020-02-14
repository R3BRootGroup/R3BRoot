// Include this class header:
#include "CrossSections_7LiPN.h"

// Default Constructor:
CrossSections_7LiPN::CrossSections_7LiPN()
{
    // Assign all parameters:
    PrintErrors = kTRUE;
    SignalFileName = "./sig.root";
    BackgroundFileName = "./back.root";
    BranchTitle = "TheNeutronTracks";
    SignalHists.clear();
    BackgroundHists.clear();
    SubtractedHists.clear();
    PeakAreas.clear();
    PeakAreaErrors.clear();
    MonteCarlo_PeakAreas.clear();
    MonteCarlo_PeakAreaErrors.clear();
    Epsilon.clear();
    Epsilon_Error.clear();
    Epsilon_Systematic.clear();
    MC_Detection_Efficiency = 1.0;
    MC_Detection_Efficiency_Error = 0.0;
    Experiment_Is110MeV = kFALSE;
    Experiment_Is250MeV = kTRUE;
    Inputs_IsLinked = kFALSE;
    Generator = 0;
    Experiment_IsValidation = kFALSE;
    TheCrossSection = 0;
    TheSystErrorBand = 0;
    TheSystErrorPlot = 0;
    OutputPath = "./";
    UseExternalEfficiencies = kFALSE;
    External_EffFile = "./stupid.root";
    BandPosition = 1.0;
    UseCrossLogScale = kTRUE;
    
    // Build nucler database:
    TheNuclei = new Nuclei();
    TheNuclei->BuildDataBase();
}
    
// Destructor:
CrossSections_7LiPN::~CrossSections_7LiPN() 
{
    // Clear all vectors:
    Errors.clear();
    SignalHists.clear();
    BackgroundHists.clear();
    SubtractedHists.clear();
    PeakAreas.clear();
    PeakAreaErrors.clear();
    MonteCarlo_PeakAreas.clear();
    MonteCarlo_PeakAreaErrors.clear();
    Epsilon.clear();
    Epsilon_Error.clear();
    Epsilon_Systematic.clear();
    
    // Delete all objects:
    delete TheNuclei;
    delete TheCrossSection;
    delete TheSystErrorBand;
}

// Member function definitions:
#include "ErrorMessage.h"
#include "RoundOff.h"
#include "LinkInputClass.h"
#include "SubtractHists.h"
#include "DrawHistDoumaArray.h"
#include "DrawGraphDouma.h"
#include "LoadHistograms.h"
#include "FitHistograms.h"
#include "ComputeMCAreas.h"
#include "ResetHistogramVectors.h"
#include "ComputeEfficiencies.h"
#include "ReadEfficiencies.h"
#include "ComputeTheCrossSection.h"
#include "Analysis.h"

// Generate ROOT dictionary:
ClassImp(CrossSections_7LiPN);
