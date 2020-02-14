// Include this class header:
#include "R3BTradMethClass.h"

// Declaration of required global parameters:
Int_t     GLOBAL_Max_Multiplicity;
TH2D**    GLOBAL_MultHistograms;
Double_t* GLOBAL_Ethresholds;
Double_t  GLOBAL_Kappa;

// Inlusion of non-member functions:
#include "GLOBAL_HistIntegral.h"
#include "SquaredWasted.h"
#include "SquaredWasted_EqualDist.h"

// Default Constructor:
R3BTradMethClass::R3BTradMethClass()
{
    Max_Multiplicity = 0;
    MultHistograms = 0;
    PrintErrors = kTRUE;
    Errors.clear();
    Inputs = 0;
    Histograms_Loaded = kFALSE;
    Ethresholds = 0;
    Avg_Ethresholds = 0;
    Kappa = 0.0;
    Avg_Kappa = 0.0;
    Cuts_Initialized = kFALSE;
    Include_Zero = kFALSE;
    MinimizerStatus = -10;
}
    
// Destructor:
R3BTradMethClass::~R3BTradMethClass() 
{
    Max_Multiplicity = 0;
    delete MultHistograms;
    Errors.clear();
    delete Inputs;
    delete Ethresholds;
    delete Avg_Ethresholds;
}

// Member function definitions:
#include "RoundOff.h"
#include "DrawHistDouma_2D_flatArray.h"
#include "LoadHistograms.h"
#include "ErrorMessage.h"
#include "LinkInputClass.h"
#include "InitializeCuts.h"
#include "HistIntegral.h"
#include "PlotHistograms.h"
#include "OptimizeCuts.h"
#include "OptimizeCuts_EqualDist.h"
#include "Illustrate_EqualDist.h"
#include "SaveCuts.h"
#include "PrintMatrix.h"
#include "DoMultipleOptimizations.h"

// Generate ROOT dictionary:
ClassImp(R3BTradMethClass);
