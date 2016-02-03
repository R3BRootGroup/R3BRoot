#ifndef R3BPSPXCALITEM_H
#define R3BPSPXCALITEM_H

#include "TObject.h"

// for the data analysis of the s438b PSPX detectors. 
// Introduced by Ralf, Jan 2016

class R3BPspxCalItem : public TObject
{
  public:
    // Default Constructor
    R3BPspxCalItem();

    // Standard Constructor
    R3BPspxCalItem(UChar_t detector, UChar_t channel, Float_t energy);

    // Destructor
    virtual ~R3BPspxCalItem() { }

    // Getters
    inline const UChar_t& GetDetector() const { return fDetector; }
    inline const UChar_t& GetChannel()  const { return fChannel;  }
    inline const Float_t& GetEnergy()   const { return fEnergy;   }

  private:
	UChar_t fDetector; // 0..4
    UChar_t fChannel;  // 0..64
    Float_t fEnergy;   // 0xfff is energy data, Anything in 0xfffff000 indicates an error or overflow

  public:
    ClassDef(R3BPspxCalItem, 2)
};

#endif
