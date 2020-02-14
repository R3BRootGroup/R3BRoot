// Every CXX-file includes its own header:
#include "ExValue.h"

// Default Constructor definition:
ExValue::ExValue()
{
    ENeutron = 0.0;
    Ex = 0.0;
    Alpha = 0.0;
    Alpha_LAB = 0.0;
    Phi = 0.0;
    Eder = 0.0;
    EdepTotal = 0.0;
    nClusters = 0;
}

// Copy Constructor definition:
ExValue::ExValue(ExValue* Other)
{
    ENeutron = Other->ENeutron;
    Ex = Other->Ex;
    Alpha = Other->Alpha;
    Alpha_LAB = Other->Alpha_LAB;
    Phi = Other->Phi;
    Eder = Other->Eder;
    EdepTotal = Other->EdepTotal;
    nClusters = Other->nClusters;
}

// Full content constructor definition:
ExValue::ExValue(Double_t const En, Double_t const E, Double_t const A, Double_t const Al, Double_t const f, Double_t const Eprime, Double_t const Edep, Int_t const nClus)
{
    ENeutron = En;
    Ex = E;
    Alpha = A;
    Alpha_LAB = Al;
    Phi = f;
    Eder = Eprime;
    EdepTotal = Edep;
    nClusters = nClus;
}

// Destructor definition:
ExValue::~ExValue() {}

// Member function definitions:


// Generation of a ROOT-dictionary:
ClassImp(ExValue)


