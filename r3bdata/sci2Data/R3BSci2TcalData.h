#ifndef R3BSCI2TCALITEM_H
#define R3BSCI2TCALITEM_H

#include "TObject.h"

// for the data analysis of the Sci2 detectors.

class R3BSci2TcalData : public TObject
{
  public:
    // Default Constructor
    R3BSci2TcalData();

    // Standard Constructor
    R3BSci2TcalData(UInt_t d, UInt_t c, Double_t t);

    // Destructor
    virtual ~R3BSci2TcalData() {}

    // Getters
    inline const UInt_t& GetDetector() const { return fDetector; }
    inline const UInt_t& GetChannel() const { return fChannel; }
    inline const Double_t& GetRawTimeNs() const { return fRawTimeNs; }

  private:
    UInt_t fDetector;
    UInt_t fChannel;
    Double_t fRawTimeNs;

    ClassDef(R3BSci2TcalData, 1)
};

#endif
