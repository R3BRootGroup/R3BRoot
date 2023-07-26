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
    explicit R3BSamplerMappedData(int);

    [[nodiscard]] int GetTime() const;

    int fTime;

    ClassDef(R3BSamplerMappedData, 1)
};

#endif
