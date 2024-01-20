#ifndef R3BFRSSCITCALITEM_H
#define R3BFRSSCITCALITEM_H

#include "TObject.h"

class R3BFrsSciTcalData : public TObject
{
  public:
    // Default Constructor
    R3BFrsSciTcalData();

    // Standard Constructor
    R3BFrsSciTcalData(UShort_t, UShort_t, Double_t);

    // Destructor
    virtual ~R3BFrsSciTcalData() {}

    // Getters
    inline const UShort_t& GetDetector() const { return fDetector; }
    inline const UShort_t& GetPmt() const { return fPmt; }
    inline const Double_t& GetRawTimeNs() const { return fRawTimeNs; }

  private:
    UShort_t fDetector;  // 1..n with n number of sci from S2 to cave C
    UShort_t fPmt;       // 1..3
    Double_t fRawTimeNs; // (5*coarse_time - fine_time)

  public:
    ClassDef(R3BFrsSciTcalData, 2)
};

#endif
