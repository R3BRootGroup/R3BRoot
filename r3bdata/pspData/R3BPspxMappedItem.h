#ifndef R3BPSPXMAPPEDITEM_H
#define R3BPSPXMAPPEDITEM_H

#include "TObject.h"

// for the data analysis of the s438b PSPX detectors. 
// Introduced by Ralf, Jan 2016

class R3BPspxMappedItem : public TObject
{
  public:
    // Default Constructor
    R3BPspxMappedItem();

    // Standard Constructor
    R3BPspxMappedItem(UChar_t detector, UChar_t channel, UInt_t energy);

    // Destructor
    virtual ~R3BPspxMappedItem() { }

    // Getters
    inline const UChar_t& GetDetector() const { return fDetector; }
    inline const UChar_t& GetChannel()  const { return fChannel;  }
    inline const UInt_t&  GetEnergy()   const { return fEnergy;   }

  private:
	UChar_t fDetector; // 0..4
    UChar_t fChannel;  // 0..64
    UInt_t  fEnergy;   // 0xfff is energy data, Anything in 0xfffff000 indicates an error or overflow

  public:
    ClassDef(R3BPspxMappedItem, 2)
};

#endif
