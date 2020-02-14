// Every CXX-file includes its own header:
#include "MassValue.h"

// Default Constructor definition:
MassValue::MassValue()
{
    TheMass = 0.0;
}

// Copy Constructor definition:
MassValue::MassValue(MassValue* Other)
{
    TheMass = Other->TheMass;
}

// Full content constructor definition:
MassValue::MassValue(Double_t const m)
{
    TheMass = m;
}

// Destructor definition:
MassValue::~MassValue() {}

// Member function definitions:


// Generation of a ROOT-dictionary:
ClassImp(MassValue)


