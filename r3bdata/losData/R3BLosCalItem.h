#ifndef R3BLOSCALITEM_H
#define R3BLOSCALITEM_H

#include "TObject.h"

// for the data analysis of the LOS detectors. 
// Introduced by Ralf, FEB 2016

class R3BLosCalItem : public TObject
{
  public:
    // Default Constructor
    R3BLosCalItem();

    // Standard Constructor
    R3BLosCalItem(UChar_t detector); //, Double_t r,Double_t t,Double_t l,Double_t b,Double_t tref);

    // Destructor
    virtual ~R3BLosCalItem() { }

    // Getters
    inline const UChar_t&  GetDetector()  const { return fDetector; }

  private:
    UChar_t  fDetector;

  public:
    Double_t fTime_r_ns;   // right
    Double_t fTime_t_ns;   // top
    Double_t fTime_l_ns;   // left
    Double_t fTime_b_ns;   // bottom
    Double_t fTime_ref_ns; // time reference (e.g. master trigger)

    ClassDef(R3BLosCalItem, 2)
};

#endif


