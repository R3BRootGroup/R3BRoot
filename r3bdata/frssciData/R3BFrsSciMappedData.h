#ifndef R3BFRSSCIMAPPEDITEM_H
#define R3BFRSSCIMAPPEDITEM_H

#include "TObject.h"

class R3BFrsSciMappedData : public TObject
{
  public:
    // Default Constructor
    R3BFrsSciMappedData();

    // Standard Constructor
    R3BFrsSciMappedData(UShort_t, UShort_t, UInt_t, UInt_t);

    // Destructor
    virtual ~R3BFrsSciMappedData() {}

    // Getters
    inline const UShort_t& GetDetector() const { return fDetector; }
    inline const UShort_t& GetPmt() const { return fPmt; }
    inline const UInt_t& GetTimeCoarse() const { return fTimeCoarse; }
    inline const UInt_t& GetTimeFine() const { return fTimeFine; }

    // private:
  public:
    UShort_t fDetector; // fDetector1 = S2, fDetector2 = S8, fDetector3 = CaveC
    UShort_t fPmt;      // Pmt1 = RIGHT, Pmt2 = LEFT, Pmt3 = TREF, Pmt4=reserved for sync signal
    UInt_t fTimeCoarse;
    UInt_t fTimeFine;
    Int_t fNEvents = 0;

  public:
    ClassDef(R3BFrsSciMappedData, 1)
};

#endif
