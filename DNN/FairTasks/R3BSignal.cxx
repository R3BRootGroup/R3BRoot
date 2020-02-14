// Every CXX-file includes its own header:
#include "R3BSignal.h"

// Default Constructor definition:
R3BSignal::R3BSignal()
{
    Position_X = 0.0;
    Position_Y = 0.0;
    Position_Z = 0.0;
    Time = 0.0;
    Energy = 0.0;
    IsPrimary_Simulation = kFALSE;
    IsPrimary_Experimental = kFALSE;
    CrystalIndex = 1;
}

// Copy Constructor definition:
R3BSignal::R3BSignal(R3BSignal* Other)
{
    Position_X = Other->Position_X;
    Position_Y = Other->Position_Y;
    Position_Z = Other->Position_Z;
    Time = Other->Time;
    Energy = Other->Energy;
    IsPrimary_Simulation = Other->IsPrimary_Simulation;
    IsPrimary_Experimental = Other->IsPrimary_Experimental;
    CrystalIndex = Other->CrystalIndex;
}

// Full content constructor definition:
R3BSignal::R3BSignal(Double_t const xx, Double_t const yy, Double_t const zz, Double_t const tt, Double_t const EE, Bool_t const SimPrim, Bool_t const ExpPrim, Int_t const Cryst)
{
    Position_X = xx;
    Position_Y = yy;
    Position_Z = zz;
    Time = tt;
    Energy = EE;
    IsPrimary_Simulation = SimPrim;
    IsPrimary_Experimental = ExpPrim;
    CrystalIndex = Cryst;
}

// Constructor for the creation of experimental primaries:
R3BSignal::R3BSignal(Double_t const xx, Double_t const yy, Double_t const zz, Double_t const tt)
{
    Position_X = xx;
    Position_Y = yy;
    Position_Z = zz;
    Time = tt;
    Energy = 0.0;
    IsPrimary_Simulation = kFALSE;
    IsPrimary_Experimental = kTRUE;
    CrystalIndex = -1;
}

// Destructor definition:
R3BSignal::~R3BSignal() {}

// Member function definitions:
Bool_t R3BSignal::IsHorPaddle(TString const TheCase, Int_t const nPaddlesPerPlane)
{
    // This function only applies to NeuLAND-signals.
    Bool_t Answer = kFALSE;
    
    if (TheCase=="NeuLAND")
    {
        Int_t CurrentIndex = CrystalIndex-1; // because CrystalIndex is 1-3000!
        if (CurrentIndex<0) {CurrentIndex = 0;}
        Int_t CurrentPlane = -1; // To properly start the while-loop.
        
        while (CurrentIndex>=0) // It starts at 0!
        {
            CurrentIndex = CurrentIndex - nPaddlesPerPlane;
            CurrentPlane = CurrentPlane + 1;
        }
        
        // Now, CurrentPlane is the correct plane, when starting at 0.
        // This means that if CurrentPlane is even (or zero),
        // The paddles are horizontal.
        if (CurrentPlane%2==0) {Answer = kTRUE;}
        else                   {Answer = kFALSE;}
        
        // Done.
    }
    else
    {
        std::cout << "\n\n==> R3BSignal::IsHorPaddle() should only be used for NeuLAND-situations!\n\n";
    }
    
    return Answer;
}        

Int_t R3BSignal::GetCurrentPlane(TString const TheCase, Int_t const nPaddlesPerPlane)
{
    // This function only applies to NeuLAND-signals.
    Int_t Answer = -1;
    
    if (TheCase=="NeuLAND")
    {
        Int_t CurrentIndex = CrystalIndex-1; // because CrystalIndex is 1-3000!
        if (CurrentIndex<0) {CurrentIndex = 0;}
        Int_t CurrentPlane = -1; // To properly start the while-loop.
        
        while (CurrentIndex>=0) // It starts at 0!
        {
            CurrentIndex = CurrentIndex - nPaddlesPerPlane;
            CurrentPlane = CurrentPlane + 1;
        }
        
        // Now, CurrentPlane is the correct plane, starting at 0.
        Answer = CurrentPlane+1;
    }
    else
    {
        std::cout << "\n\n==> R3BSignal::GetCurrentPlane() should only be used for NeuLAND-situations!\n\n";
    }
    
    // NOTE: First plane starts at 1!
    return Answer;
}        

Double_t R3BSignal::GetBeta(Double_t const Target_T, Double_t const Target_X, Double_t const Target_Y, Double_t const Target_Z)
{
    // Compute relativistic beta:
    Double_t Distance = 0.0;
    Distance = Distance + (Position_X - Target_X)*(Position_X - Target_X);
    Distance = Distance + (Position_Y - Target_Y)*(Position_Y - Target_Y);
    Distance = Distance + (Position_Z - Target_Z)*(Position_Z - Target_Z);
    Distance = TMath::Sqrt(Distance);
    
    Double_t TOF = TMath::Abs(Time - Target_T);
    Double_t cLight = 29.9792458;
    
    Double_t Beta = Distance/(TOF*cLight);
    return Beta;
}    

// Copy function:
void R3BSignal::Duplicate(R3BSignal* Other)
{
    Position_X = Other->Position_X;
    Position_Y = Other->Position_Y;
    Position_Z = Other->Position_Z;
    Time = Other->Time;
    Energy = Other->Energy;
    IsPrimary_Simulation = Other->IsPrimary_Simulation;
    IsPrimary_Experimental = Other->IsPrimary_Experimental;
    CrystalIndex = Other->CrystalIndex;
}

// Generation of a ROOT-dictionary:
ClassImp(R3BSignal)


