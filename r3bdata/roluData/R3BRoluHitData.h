#ifndef R3BROLUHITITEM_H
#define R3BROLUHITITEM_H

#include "TObject.h"

// for the data analysis of the ROLU following LOS

class R3BRoluHitData : public TObject
{
  public:
    // Default Constructor
    R3BRoluHitData();

    // Standard Constructor
    R3BRoluHitData(UChar_t detector,Double_t tot_r,Double_t tot_o, Double_t
    tot_l, Double_t tot_u); 

    // Destructor
    virtual ~R3BRoluHitData() { }

    // Getters
    inline const UChar_t&  GetDetector()  const { return fDetector; }

  private:
    UChar_t  fDetector;		   // 1..n

  public:
    Double_t ftot_r;         
    Double_t ftot_o;			   	
    Double_t ftot_l;
    Double_t ftot_u;               

    ClassDef(R3BRoluHitData, 1)
};

#endif


