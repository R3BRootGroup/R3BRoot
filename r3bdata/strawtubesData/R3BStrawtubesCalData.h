#ifndef R3BSTRAWTUBESCALITEM_H
#define R3BSTRAWTUBESCALITEM_H

#include "TObject.h"

class R3BStrawtubesCalData: public TObject
{
  public:
    R3BStrawtubesCalData(UChar_t, UChar_t);
    virtual ~R3BStrawtubesCalData();
    UChar_t GetPlane() const;
    UChar_t GetStraw() const;
    Double_t GetTime(UChar_t) const;
    void SetTime(UChar_t, Double_t);

  private:
    UChar_t fPlane;
    UChar_t fStraw;
    Double_t fTime_ns[2];

  public:
    ClassDef(R3BStrawtubesCalData, 0)
};

#endif


