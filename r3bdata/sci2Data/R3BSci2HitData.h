#ifndef R3BSCI2HITITEM_H
#define R3BSCI2HITITEM_H

#include "TObject.h"

// for the data analysis of the SCI2 detectors.

class R3BSci2HitData : public TObject
{
  public:
    // Default Constructor
    R3BSci2HitData();

    // Standard Constructor
    R3BSci2HitData(UChar_t detector, Double_t t, Double_t x, Double_t Z);

    // Destructor
    virtual ~R3BSci2HitData() {}

    // Getters
    inline const UChar_t& GetDetector() const { return fDetector; }

  private:
    UChar_t fDetector; // 1..n
    Double_t fTime_ns; // average time of the scintillators
    Double_t fX_cm;    // position in cm
    Double_t fZ;       // Charge

  public:
    ClassDef(R3BSci2HitData, 1)
};

#endif
