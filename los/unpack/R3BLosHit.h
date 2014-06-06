// ---------------------------------------------------------
// -----                    R3BLosHit                  -----
// -----          Created 29-04-2014 by D.Kresan       -----
// ---------------------------------------------------------

#ifndef R3BLOSHIT
#define R3BLOSHIT

#include "TObject.h"

class R3BLosHit : public TObject
{
  public:
    R3BLosHit();

    R3BLosHit(Int_t channel, Double_t time);

    virtual ~R3BLosHit();

    inline Int_t GetChannel() const
    {
        return fChannel;
    }
    inline Double_t GetTime() const
    {
        return fTime;
    }

  private:
    Int_t fChannel;
    Double_t fTime;

  public:
    ClassDef(R3BLosHit, 1);
};

#endif
