#ifndef R3BLOSCALITEM_H
#define R3BLOSCALITEM_H

#include "TObject.h"

// for the data analysis of the LOS detectors. 
// Introduced by Ralf, FEB 2016

class R3BLosCalData : public TObject
{
  public:
    // Default Constructor
    R3BLosCalData();

    // Standard Constructor
    R3BLosCalData(UChar_t detector); //, Double_t r,Double_t t,Double_t l,Double_t b,Double_t tref);

    // Destructor
    virtual ~R3BLosCalData() { }

    // Getters
    inline const UChar_t&  GetDetector()  const { return fDetector; }

    Double_t GetMeanTime();

  private:
    UChar_t  fDetector;

  public:
    Double_t fTime_r_ns;   // right
    Double_t fTime_t_ns;   // top
    Double_t fTime_l_ns;   // left
    Double_t fTime_b_ns;   // bottom
    Double_t fTime_ref_ns; // time reference (e.g. master trigger)

    Double_t fTime_cherenkov_l_ns;
    Double_t fTime_cherenkov_r_ns;

    ClassDef(R3BLosCalData, 3)
};

#endif


