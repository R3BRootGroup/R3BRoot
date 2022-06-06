#ifndef R3BSAMPLOSMAPPEDITEM_H
#define R3BSAMPLOSMAPPEDITEM_H

#include "TObject.h"

// For the data analysis of the SAMPLER with two components

class R3BSampLosMappedData : public TObject
{
  public:
    // Default Constructor
    R3BSampLosMappedData();

    // Standard Constructor
    R3BSampLosMappedData(UInt_t, UInt_t);

    UInt_t GetTimeL() const;
    UInt_t GetTimeH() const;

  public:
    UInt_t fTimeL;
    UInt_t fTimeH;

  public:
    ClassDef(R3BSampLosMappedData, 1)
};

#endif
