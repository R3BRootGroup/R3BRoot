#ifndef R3BLOSTCALITEM_H
#define R3BLOSTCALITEM_H

#include "TObject.h"

// for the data analysis of the LOS detectors.

class R3BLosTCalData : public TObject
{
  public:
    // Default Constructor
    R3BLosTCalData();

    // Standard Constructor
    R3BLosTCalData(UInt_t d, UInt_t c, UInt_t typ, Double_t t);

    // Destructor
    virtual ~R3BLosTCalData() {}

    // Getters
    inline const UInt_t& GetDetector() const { return fDetector; }
    inline const UInt_t& GetChannel() const { return fChannel; }
    inline const UInt_t& GetType() const { return fType; }
    inline const Double_t& GetRawTimeNs() const { return fRawTimeNs; }

  private:
    UInt_t fDetector;
    UInt_t fChannel;
    UInt_t fType;
    Double_t fRawTimeNs;

    ClassDef(R3BLosTCalData, 1)
};

#endif
