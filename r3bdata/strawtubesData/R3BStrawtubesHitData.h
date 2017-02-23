#ifndef R3BSTRAWTUBESHITDATA_H
#define R3BSTRAWTUBESHITDATA_H

#include "TObject.h"

class R3BStrawtubesCalData;

class R3BStrawtubesHitData: public TObject
{
  public:
    R3BStrawtubesHitData(R3BStrawtubesCalData const &); 
    virtual ~R3BStrawtubesHitData();

  private:
    UChar_t fPlane;
    UChar_t fStraw;
    Double_t fAverage_ns;
    Double_t fDiff_ns;

  public:
    ClassDef(R3BStrawtubesHitData, 0)
};

#endif
