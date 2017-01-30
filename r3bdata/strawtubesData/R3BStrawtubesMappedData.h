#ifndef R3BSTRAWTUBESMAPPEDITEM_H
#define R3BSTRAWTUBESMAPPEDITEM_H

#include "TObject.h"

class R3BStrawtubesMappedData: public TObject
{
  public:
    R3BStrawtubesMappedData(UChar_t, UChar_t, UChar_t, UInt_t, UInt_t);
    virtual ~R3BStrawtubesMappedData();
    UChar_t GetPlane() const      { return fPlane; }
    UChar_t GetSide() const       { return fSide; }
    UChar_t GetStraw() const      { return fStraw; }
    UInt_t  GetTimeCoarse() const { return fTimeCoarse; }
    UInt_t  GetTimeFine() const   { return fTimeFine; }

  private:
    UChar_t fPlane;
    UChar_t fStraw;
    UChar_t fSide;
    UInt_t fTimeCoarse;
    UInt_t fTimeFine;

  public:
    ClassDef(R3BStrawtubesMappedData, 0)
};

#endif
