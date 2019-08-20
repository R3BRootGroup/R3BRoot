#ifndef R3BSAMPLERMAPPEDITEM_H
#define R3BSAMPLERMAPPEDITEM_H

#include "TObject.h"

// For the data analysis of the SAMPLER.

class R3BSamplerMappedData : public TObject
{
  public:
    // Default Constructor
    R3BSamplerMappedData();

    // Standard Constructor
    R3BSamplerMappedData(UInt_t);

    UInt_t GetTime() const;

  public:
    UInt_t fTime;

  public:
    ClassDef(R3BSamplerMappedData, 1)
};

#endif
