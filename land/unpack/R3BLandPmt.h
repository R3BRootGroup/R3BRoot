// -----------------------------------------------------------------------
// -----                          R3BLandPmt                         -----
// -----                Created 22-04-2014 by D.Kresan               -----
// -----------------------------------------------------------------------

#ifndef R3BLANDPMT
#define R3BLANDPMT

#include "TObject.h"

class R3BLandPmt : public TObject
{
  public:
    R3BLandPmt();
    R3BLandPmt(Int_t barId, Int_t side, Double_t time, Int_t qdc);
    virtual ~R3BLandPmt();

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
    ClassDef(R3BLandPmt, 1)
};

#endif
