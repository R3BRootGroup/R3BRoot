#ifndef R3BTOFDMAPPEDITEM_H
#define R3BTOFDMAPPEDITEM_H

#include "TObject.h"

// for the data analysis of the TOFD. 
// Introduced by Ralf, March 2016

class R3BTofdMappedItem : public TObject
{
  public:
    // Default Constructor
    R3BTofdMappedItem();

    // Standard Constructor
    R3BTofdMappedItem(UChar_t paddle, UChar_t pm, UInt_t timeCoarse, UInt_t timeFine);

    // Destructor
    virtual ~R3BTofdMappedItem() { }
/*
    // Getters
    inline const UChar_t& GetPaddle()     const { return fPaddle;  }
    inline const UChar_t& GetPm()         const { return fPm;  }
    inline const UInt_t&  GetTimeCoarse() const { return fTimeCoarse;   }
    inline const UInt_t&  GetTimeFine()   const { return fTimeFine;   }
*/
  private:
    UChar_t fPaddle; // 1..n
    UChar_t fPm;     // 1 or 2
    UInt_t  fTimeCoarse;   
    UInt_t  fTimeFine;

  public:
    ClassDef(R3BTofdMappedItem, 2)
};

#endif
