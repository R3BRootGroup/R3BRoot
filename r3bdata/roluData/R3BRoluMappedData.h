#ifndef R3BROLUMAPPEDITEM_H
#define R3BROLUMAPPEDITEM_H

#include "TObject.h"

// for the data analysis of ROLU following LOS

class R3BRoluMappedData : public TObject
{
  public:
    // Default Constructor
    R3BRoluMappedData();

    // Standard Constructor
    R3BRoluMappedData(UInt_t, UInt_t, UInt_t, UInt_t, UInt_t);

    UInt_t GetDetector()   const ;
    UInt_t GetChannel()    const ;
    UInt_t GetType()       const ;
    UInt_t GetTimeFine()   const ;
    UInt_t GetTimeCoarse() const ;

  public:
    UInt_t fDetector; // 1..n
    UInt_t fChannel;  // 1..n (n=4 )
    UInt_t fType;     // 0 (TAMEX leading), 1 (TAMEX trailing)
    UInt_t fTimeCoarse;
    UInt_t fTimeFine;

  public:
    ClassDef(R3BRoluMappedData, 1) 
};

#endif
