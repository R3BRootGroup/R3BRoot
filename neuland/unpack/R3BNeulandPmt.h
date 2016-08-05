// -----------------------------------------------------------------------
// -----                         R3BNeulandPmt                       -----
// -----                Created 27-01-2016 by M.Heil                 -----
// -----------------------------------------------------------------------

#ifndef R3BNEULANDPMT
#define R3BNEULANDPMT

#include "TObject.h"

class R3BNeulandPmt : public TObject
{
  public:
    R3BNeulandPmt();
    R3BNeulandPmt(Int_t planeId, Int_t barId, Int_t side, Double_t time, Double_t charge);
    virtual ~R3BNeulandPmt();

    inline const Int_t& GetPlaneId() const
    {
        return fPlaneId;
    }
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
    inline const Double_t& GetCharge() const
    {
        return fCharge;
    }

  private:
    Int_t fPlaneId;
    Int_t fBarId;
    Int_t fSide;
    Double_t fTime;
    Double_t fCharge;

  public:
    ClassDef(R3BNeulandPmt, 1)
};

#endif
