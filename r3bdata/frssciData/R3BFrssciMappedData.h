#ifndef R3BFRSSCIMAPPEDITEM_H
#define R3BFRSSCIMAPPEDITEM_H

#include "TObject.h"

class R3BFrssciMappedData : public TObject
{
  public:
    // Default Constructor
    R3BFrssciMappedData();

    // Standard Constructor
    R3BFrssciMappedData(UShort_t detector, UShort_t pmt, UInt_t timeFine, UInt_t timeCoarse);

    // Destructor
    virtual ~R3BFrssciMappedData() {}

    // Getters
    inline const UShort_t& GetDetector() const { return fDetector; }
    inline const UShort_t& GetPmt() const { return fPmt; }
    inline const UInt_t& GetTimeFine() const { return fTimeFine; }
    inline const UInt_t& GetTimeCoarse() const { return fTimeCoarse; }

    // private:
  public:
    UShort_t fDetector; // fDetector1 = S2, fDetector2 = S8, fDetector3 = CaveC
    UShort_t fPmt;      // Pmt1 = RIGHT, Pmt2 = LEFT, Pmt3 = TREF, Pmt4=reserved for sync signal
    UInt_t fTimeCoarse;
    UInt_t fTimeFine;
    Int_t fNEvents = 0;

  public:
    ClassDef(R3BFrssciMappedData, 1)
};

#endif
