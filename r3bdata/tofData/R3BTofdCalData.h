#ifndef R3BTOFDCALITEM_H
#define R3BTOFDCALITEM_H

#include "TObject.h"

class R3BTofdCalData: public TObject
{
  public:
    R3BTofdCalData();
    R3BTofdCalData(UInt_t, UInt_t);

    UInt_t GetDetectorId()      const;
    UInt_t GetBarId()           const;
    Double_t GetTimeBL_ns()     const;
    Double_t GetTimeBT_ns()     const;
    Double_t GetTimeTL_ns()     const;
    Double_t GetTimeTT_ns()     const;
    Double_t GetTime_ns(UInt_t) const;

    Bool_t SetTimeBL_ns(Double_t, Double_t);
    Bool_t SetTimeBT_ns(Double_t, Double_t);
    Bool_t SetTimeTL_ns(Double_t, Double_t);
    Bool_t SetTimeTT_ns(Double_t, Double_t);
    Bool_t SetTime_ns(UInt_t, Double_t, Double_t);

  private:
    Double_t GetTimeDiff(Double_t, Double_t) const;
    void Reset();

    UInt_t fDetector;       // 1..4
    UInt_t fBar;            // 1..44
    Double_t fTime_ns[4]; // 1=bl, 2=bt, 3=tl, 4=tt
    // Inclusive interval of times.
    Double_t fFirst;
    Double_t fLast;

    ClassDef(R3BTofdCalData, 1)
};

#endif
