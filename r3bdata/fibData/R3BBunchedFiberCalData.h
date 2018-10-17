// Introduced by M. Heil, Jan 2018
//
// This class contains calibrated data for one electronic channel in a bunched
// fiber detector, still not one fiber!
//

#ifndef R3BBUNCHEDFIBERCALDATA_H
#define R3BBUNCHEDFIBERCALDATA_H

#include "TObject.h"

class R3BBunchedFiberCalData: public TObject
{
  public:
    R3BBunchedFiberCalData();
    R3BBunchedFiberCalData(Bool_t, Int_t, Bool_t, Double_t);
    virtual ~R3BBunchedFiberCalData();

    Int_t GetChannel() const;
    Double_t GetTime_ns() const;
    Bool_t IsMAPMT() const;
    Bool_t IsSPMT() const;
    Bool_t IsLeading() const;
    Bool_t IsSortable() const;
    Bool_t IsTrailing() const;

  private:
    Bool_t fIsMAPMT;
    Int_t fChannel;
    Bool_t fIsLeading;
    Double_t fTime_ns;

    ClassDef(R3BBunchedFiberCalData, 1)
};

#endif
