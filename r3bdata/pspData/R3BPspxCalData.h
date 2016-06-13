#ifndef R3BPSPXCALDATA_H
#define R3BPSPXCALDATA_H

#include "TObject.h"

// for the data analysis of the s438b PSPX detectors. 
// Introduced by Ralf, Jan 2016
// Changed by Ina Syndikus, April 2016

class R3BPspxCalData : public TObject
{
  public:
    // Default Constructor
    R3BPspxCalData();

    // Standard Constructor
    R3BPspxCalData(UChar_t detector, UChar_t strip, Float_t energy1, Float_t energy2);
    
    // Destructor
    virtual ~R3BPspxCalData() { }

    // Getters
    inline const UChar_t& GetDetector() const { return fDetector; }
    inline const UChar_t& GetStrip()  const { return fStrip;  }
    inline const Float_t& GetEnergy1()   const { return fEnergy1;   }
    inline const Float_t& GetEnergy2()   const { return fEnergy2;   }

  private:
    UChar_t fDetector; // 1..5
    UChar_t fStrip;  // 1..17
    Float_t fEnergy1;   // 0xfff is energy data, Anything in 0xfffff000 indicates an error or overflow
    Float_t fEnergy2;   // 0xfff is energy data, Anything in 0xfffff000 indicates an error or overflow
    
  public:
    ClassDef(R3BPspxCalData, 5)
};

#endif
