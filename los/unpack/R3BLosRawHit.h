// ---------------------------------------------------------------------------------------
// -----                                R3BLosRawHit                                 -----
// -----                      Created 14-04-2014 by D.Kresan                         -----
// ---------------------------------------------------------------------------------------

#ifndef R3BLOSRAWHIT_H
#define R3BLOSRAWHIT_H

#include "TObject.h"

class R3BLosRawHit : public TObject
{
  public:
    // Default Constructor
    R3BLosRawHit();

    // Standard Constructor
    R3BLosRawHit(UShort_t channel, UShort_t tdc, UShort_t clock);

    // Destructor
    virtual ~R3BLosRawHit()
    {
    }

    inline const UShort_t& GetChannel() const
    {
        return fChannel;
    }
    inline const UShort_t& GetTdc() const
    {
        return fTdc;
    }
    inline const UShort_t& GetClock() const
    {
        return fClock;
    }

  private:
    UShort_t fChannel;
    UShort_t fTdc;
    UShort_t fClock;

  public:
    ClassDef(R3BLosRawHit, 1)
};

#endif
