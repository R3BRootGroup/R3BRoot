#ifndef R3BPSPXHITDATA_H
#define R3BPSPXHITDATA_H

#include "TObject.h"

// for the data analysis of the s438b PSPX detectors. 
// Introduced by Ina Syndikus, April 2016

class R3BPspxHitData : public TObject
{
  public:
    // Default Constructor
    R3BPspxHitData();

    // Standard Constructor
    R3BPspxHitData(UChar_t detector, Float_t u, Float_t v, Float_t x, Float_t y, Float_t energy, UInt_t multiplicity);
    
    // Destructor
    virtual ~R3BPspxHitData() { }

    // Getters
    inline const UChar_t& GetDetector() const { return fDetector; }
    inline const Float_t& GetU()   const { return fU;   }
    inline const Float_t& GetV()   const { return fV;   }
    inline const Float_t& GetX()   const { return fX;   }
    inline const Float_t& GetY()   const { return fY;   }
    inline const Float_t& GetEnergy()   const { return fEnergy;   }
    inline const UInt_t& GetMultiplicity()   const { return fMultiplicity;   }
    
  private:
    UChar_t fDetector; // 1..5
    Float_t fU; // between -1,1
    Float_t fV; // between -1,1
    Float_t fX; // between -l/2,l/2
    Float_t fY; // between -l/2,l/2
    Float_t fEnergy; // 
    UInt_t fMultiplicity; // 
        
  public:
    ClassDef(R3BPspxHitData, 3)
};

#endif
