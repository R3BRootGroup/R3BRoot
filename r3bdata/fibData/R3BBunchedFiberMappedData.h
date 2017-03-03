// ----------------------------------------------------------------
// -----              R3BBunchedFiberMappedData                -----
// -----         Created Jan 2018 by M.Heil        -----
// ----------------------------------------------------------------

//
// This class contains mapped data for one fiber electronics channel and
// logical signal edge, not for one fiber!
//

#ifndef R3BBUNCHEDFIBERMAPPEDDATA_H
#define R3BBUNCHEDFIBERMAPPEDDATA_H

#include "TObject.h"

class R3BBunchedFiberMappedData: public TObject
{
  public:
    R3BBunchedFiberMappedData();
    R3BBunchedFiberMappedData(Bool_t, Int_t, Bool_t, Int_t, Int_t);
    virtual ~R3BBunchedFiberMappedData();

    Int_t GetChannel() const;
    Int_t GetCoarse() const;
    Int_t GetFine() const;
    Bool_t IsMAPMT() const;
    Bool_t IsSPMT() const;
    Bool_t IsLeading() const;
    Bool_t IsTrailing() const;

  protected:
    Bool_t fIsMAPMT;
    Int_t fChannel;
    Bool_t fIsLeading;
    Int_t fCoarse;
    Int_t fFine;

  public:
    ClassDef(R3BBunchedFiberMappedData, 1)
};

#endif
