#ifndef R3BLOSMAPPEDITEM_H
#define R3BLOSMAPPEDITEM_H

#include "TObject.h"

// for the data analysis of the LOS detectors. 
// Introduced by Ralf, Jan 2016

class R3BLosMappedData : public TObject
{
  public:
    // Default Constructor
    R3BLosMappedData();

    // Standard Constructor
    R3BLosMappedData(UChar_t detector, UChar_t channel, UInt_t timeCoarse, UInt_t timeFine);

    // Destructor
    virtual ~R3BLosMappedData() { }

    // Getters
    inline const UChar_t& GetDetector()   const { return fDetector;  }
    inline const UChar_t& GetChannel()    const { return fChannel;  }
    inline const UInt_t&  GetTimeCoarse() const { return fTimeCoarse;   }
    inline const UInt_t&  GetTimeFine()   const { return fTimeFine;   }

  private:
    UChar_t fDetector; // 1..n
    UChar_t fChannel;  // 1..n
    UInt_t  fTimeCoarse;   
    UInt_t  fTimeFine;

  public:
    ClassDef(R3BLosMappedData, 2)
};

#endif
