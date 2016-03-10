// -----------------------------------------------------------------------
// -----                    R3BNeulandCalData                        -----
// -----              Created 22-04-2014 by D.Kresan                 -----
// -----------------------------------------------------------------------

#ifndef R3BNEULANDCALDATA_H
#define R3BNEULANDCALDATA_H

#include "TObject.h"

class R3BNeulandCalData : public TObject
{
  public:
    R3BNeulandCalData();
    R3BNeulandCalData(Int_t barId, Int_t side, Double_t time, Int_t qdc);
    virtual ~R3BNeulandCalData();

    inline const Int_t& GetBarId() const
    {
        return fBarId;
    }
    inline const Int_t& GetSide() const
    {
        return fSide;
    }
    inline const Double_t& GetTime() const
    {
        return fTime;
    }
    inline const Int_t& GetQdc() const
    {
        return fQdc;
    }

  private:
    Int_t fBarId;
    Int_t fSide;
    Double_t fTime;
    Int_t fQdc;

  public:
    ClassDef(R3BNeulandCalData, 1)
};

#endif
