#ifndef R3BTOFDHITDATA_H
#define R3BTOFDHITDATA_H

#include "TObject.h"

// for the data analysis of the Tofd detectors. 
// Introduced by M.Heil, May 2016

class R3BTofdHitData : public TObject
{
  public:
    // Default Constructor
    R3BTofdHitData();

    // Standard Constructor
    R3BTofdHitData(Double_t t,Double_t x, Double_t y, Double_t Z, Double_t tdiff); 

    // Destructor
    virtual ~R3BTofdHitData() { }

    // Getters
//    inline const UChar_t&  GetDetector()  const { return fDetector; }

  private:
    Double_t fTime_ns;         // average time of the scintillators and planes
    Double_t fXpos_cm;         // x-position of the scintillators
    Double_t fYpos_cm;         // y-position of the scintillators 
    Double_t fZ;               // Z of the scintillators 
    Double_t fTimeDiff2Ref_ns; // difference to master trigger time

  public:

    ClassDef(R3BTofdHitData, 1)
};

#endif


