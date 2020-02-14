// Include this class header:
#include "DNNDataPoint.h"

// Default Constructor:
DNNDataPoint::DNNDataPoint()
{
    // Inputs:
    INCLXX_INCLXX_Point = 0.0;
    INCLXX_INCLXX_2ndPoint = 0.0;
    INCLXX_BERT_Point = 0.0;
    BERT_INCLXX_Point = 0.0;
    BERT_BERT_Point = 0.0;
    
    INCLXX_INCLXX_Error = 0.0;
    INCLXX_INCLXX_2ndError = 0.0;
    INCLXX_BERT_Error = 0.0;
    BERT_INCLXX_Error = 0.0;
    BERT_BERT_Error = 0.0;
    
    // Outputs:
    Avg_Point = 0.0;
    Stat_Error = 0.0;
    Phys_Error = 0.0;
}
    
// Destructor:
DNNDataPoint::~DNNDataPoint() {}

// Member function definitions:
void DNNDataPoint::ComputeAvgPoint()
{
    // Computation of the average point:
    Avg_Point = 0.0;
    Avg_Point = Avg_Point + INCLXX_INCLXX_Point;
    Avg_Point = Avg_Point + INCLXX_BERT_Point;
    Avg_Point = Avg_Point + BERT_INCLXX_Point;
    Avg_Point = Avg_Point + BERT_BERT_Point;
    Avg_Point = Avg_Point/4.0;
    
    // Find the max. Poisson contribution:
    Double_t Poisson_Error = 0.0;
    if (Poisson_Error<INCLXX_INCLXX_Error) {Poisson_Error = INCLXX_INCLXX_Error;}
    if (Poisson_Error<INCLXX_INCLXX_2ndError) {Poisson_Error = INCLXX_INCLXX_2ndError;}
    if (Poisson_Error<INCLXX_BERT_Error) {Poisson_Error = INCLXX_BERT_Error;}
    if (Poisson_Error<BERT_INCLXX_Error) {Poisson_Error = BERT_INCLXX_Error;}
    if (Poisson_Error<BERT_BERT_Error) {Poisson_Error = BERT_BERT_Error;}
    
    // Compute the DNN random contribution:
    Double_t DNNRand = TMath::Abs(INCLXX_INCLXX_Point - INCLXX_INCLXX_2ndPoint);
    
    // Compute the full statistics error:
    Stat_Error = TMath::Sqrt(Poisson_Error*Poisson_Error + DNNRand*DNNRand);
    
    // Compute the physics error:
    Double_t Maximum = -1.0*1e99;
    Double_t Minimum = 1e99;
    
    // Find minimum of the four:
    if (INCLXX_INCLXX_Point<Minimum) {Minimum = INCLXX_INCLXX_Point;}
    if (INCLXX_BERT_Point<Minimum) {Minimum = INCLXX_BERT_Point;}
    if (BERT_INCLXX_Point<Minimum) {Minimum = BERT_INCLXX_Point;}
    if (BERT_BERT_Point<Minimum) {Minimum = BERT_BERT_Point;}
    
    // Find maximum of the four:
    if (INCLXX_INCLXX_Point>Maximum) {Maximum = INCLXX_INCLXX_Point;}
    if (INCLXX_BERT_Point>Maximum) {Maximum = INCLXX_BERT_Point;}
    if (BERT_INCLXX_Point>Maximum) {Maximum = BERT_INCLXX_Point;}
    if (BERT_BERT_Point>Maximum) {Maximum = BERT_BERT_Point;}
    
    // Then, the error is:
    Phys_Error = 0.5*TMath::Abs(Maximum-Minimum);
    
    // Done.
}

// Generate ROOT dictionary:
ClassImp(DNNDataPoint);
