#ifndef R3BTOFDHITDATA_H
#define R3BTOFDHITDATA_H

#include "R3BHit.h"

// for the data analysis of the Tofd detectors. 
// Introduced by M.Heil, May 2016

class R3BTofdHitData : public R3BHit
{
  public:
    // Default Constructor
    R3BTofdHitData();

    // Standard Constructor
    R3BTofdHitData(Double_t t,Double_t x, Double_t y, Double_t Z, Double_t tdiff, Double_t ELoss =0, Double_t ID=0); 
    
    // Destructor
    virtual ~R3BTofdHitData() { }

    ClassDef(R3BTofdHitData, 2)
};

#endif


