#ifndef R3BBEAMMONITORMAPPEDITEM_H
#define R3BBEAMMONITORMAPPEDITEM_H

#include "TObject.h"


class R3BBeamMonitorMappedData : public TObject
{
  public:
    // Default Constructor
    R3BBeamMonitorMappedData();

    // Standard Constructor
    R3BBeamMonitorMappedData(UInt_t, UInt_t, UInt_t);

    UInt_t GetIC()   const ;
    UInt_t GetSEETRAM()    const ;
    UInt_t GetTOFDOR()       const ;

  public:
    UInt_t fIC;
    UInt_t fSEETRAM; 
    UInt_t fTOFDOR; 

  public:
    ClassDef(R3BBeamMonitorMappedData, 1) 
};

#endif
