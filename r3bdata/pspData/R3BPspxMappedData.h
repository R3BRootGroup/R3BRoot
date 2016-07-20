#ifndef R3BPSPXMAPPEDDATA_H
#define R3BPSPXMAPPEDDATA_H

#include "TObject.h"

// for the data analysis of the s438b PSPX detectors. 
// Introduced by Ralf, Jan 2016

class R3BPspxMappedData : public TObject
{
  public:
    // Default Constructor
    R3BPspxMappedData();

    // Standard Constructor
    R3BPspxMappedData(UShort_t detector, UShort_t channel, UInt_t energy);

    // Destructor
    virtual ~R3BPspxMappedData() { }

    // Getters
    inline const UShort_t& GetDetector() const { return fDetector; }
    inline const UShort_t& GetChannel()  const { return fChannel;  }
    inline const UInt_t&  GetEnergy()   const { return fEnergy;   }

  private:
	UShort_t fDetector; // 1..5
    UShort_t fChannel;  // 1..65
    UInt_t  fEnergy;   // 0xfff is energy data, Anything in 0xfffff000 indicates an error or overflow

  public:
    ClassDef(R3BPspxMappedData, 2)
};

#endif
