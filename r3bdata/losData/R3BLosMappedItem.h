#ifndef R3BLOSMAPPEDITEM_H
#define R3BLOSMAPPEDITEM_H

#include "TObject.h"

// for the data analysis of the LOS detectors. 
// Introduced by Ralf, Jan 2016

class R3BLosMappedItem : public TObject
{
  public:
    // Default Constructor
    R3BLosMappedItem();

    // Standard Constructor
    R3BLosMappedItem(UChar_t detector, UChar_t channel, UInt_t timeCoarse, UInt_t timeFine);

    // Destructor
    virtual ~R3BLosMappedItem() { }

    // Getters
    inline const UChar_t& GetDetector()   const { return fDetector;  }
    inline const UChar_t& GetChannel()    const { return fChannel;  }
    inline const UInt_t&  GetTimeCoarse() const { return fTimeCoarse;   }
    inline const UInt_t&  GetTimeFine()   const { return fTimeFine;   }

  private:
    UChar_t fDetector;
    UChar_t fChannel;  
    UInt_t  fTimeCoarse;   
    UInt_t  fTimeFine;

  public:
    ClassDef(R3BLosMappedItem, 2)
};

#endif
