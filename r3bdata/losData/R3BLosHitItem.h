#ifndef R3BLOSHITITEM_H
#define R3BLOSHITITEM_H

#include "TObject.h"

// for the data analysis of the LOS detectors. 
// Introduced by Ralf, FEB 2016

class R3BLosHitItem : public TObject
{
  public:
    // Default Constructor
    R3BLosHitItem();

    // Standard Constructor
    R3BLosHitItem(UChar_t detector,Double_t t,Double_t diff); 

    // Destructor
    virtual ~R3BLosHitItem() { }

    // Getters
    inline const UChar_t&  GetDetector()  const { return fDetector; }

  private:
    UChar_t  fDetector;		   // 1..n
    Double_t fTime_ns;         // average time of the scintillators
    Double_t fTimeDiff2Ref_ns; // difference to master trigger time

  public:

    ClassDef(R3BLosHitItem, 2)
};

#endif


