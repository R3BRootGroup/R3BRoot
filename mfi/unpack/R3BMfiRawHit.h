// ---------------------------------------------------------------------------------------
// -----                               R3BMfiRawHit                                  -----
// -----                      Created 30-07-2014 by D.Kresan                         -----
// ---------------------------------------------------------------------------------------

#ifndef R3BMFIRAWHIT_H
#define R3BMFIRAWHIT_H

#include "TObject.h"

class R3BMfiRawHit : public TObject
{
  public:
    // Default Constructor
    R3BMfiRawHit();

    // Standard Constructor
    R3BMfiRawHit(UInt_t nxtId, UInt_t gemexId, UInt_t trigger, ULong_t triggerTs, UInt_t adc, UInt_t channelId, UInt_t nxtTs, UInt_t epoch, UInt_t index);

    // Destructor
    virtual ~R3BMfiRawHit()
    {
    }

    inline const UInt_t& GetNxyterId() const
    {
        return fNxtId;
    }
    inline const UInt_t& GetGemexId() const
    {
        return fGemexId;
    }
    inline const UInt_t& GetTrigger() const
    {
        return fTrigger;
    }
    inline const ULong_t& GetTriggerTime() const
    {
        return fTriggerTs;
    }
    inline const UInt_t& GetAdc() const
    {
        return fAdc;
    }
    inline const UInt_t& GetChannelId() const
    {
        return fChannelId;
    }
    inline const UInt_t& GetNxyterTime() const
    {
        return fNxtTs;
    }
    inline const UInt_t& GetEpochTime() const
    {
        return fEpoch;
    }
    inline const UInt_t& GetIndex() const
    {
        return fIndex;
    }

  protected:
    UInt_t fNxtId;
    UInt_t fGemexId;
    UInt_t fTrigger;
    ULong_t fTriggerTs;
    UInt_t fAdc;
    UInt_t fChannelId;
    UInt_t fNxtTs;
    UInt_t fEpoch;
    UInt_t fIndex;

  public:
    ClassDef(R3BMfiRawHit, 1)
};

#endif
