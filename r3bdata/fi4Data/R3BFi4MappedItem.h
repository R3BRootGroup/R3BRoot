#ifndef R3BFI4MAPPEDITEM_H
#define R3BFI4MAPPEDITEM_H

#include "TObject.h"

// for the data analysis of the s438b PSPX detectors. 
// Introduced by Ralf, Jan 2016

class R3BFi4MappedItem : public TObject
{
  public:
    // Default Constructor
    R3BFi4MappedItem();

    // Standard Constructor
    R3BFi4MappedItem(UChar_t detector, UInt_t mppc, UInt_t energy, UInt_t time);

    // Destructor
    virtual ~R3BFi4MappedItem() { }

    // Getters
    inline const UChar_t& GetDetector() const { return fDetector; }
    inline const UChar_t& GetChannel()  const { return fMPPC;  }
    inline const UInt_t&  GetEnergy()   const { return fEnergy;   }
    inline const UInt_t&  GetTime()     const { return fTime;     }

  private:
    UChar_t fDetector; // 0..4
    UChar_t fMPPC   ;  // 0..64
    UInt_t  fEnergy;   // 0xfff is energy data, Anything in 0xfffff000 indicates an error or overflow
    UInt_t  fTime;
  public:
    ClassDef(R3BFi4MappedItem, 4)
};

#endif
