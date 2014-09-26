// ---------------------------------------------------------------------------------------
// -----                                                                             -----
// -----                       R3BStarTrackRawHitMapped                              -----
// -----                       from R3BLandRawHitMapped                              -----
// -----                 Created  @ 03.2014 by M.I. Cherciu                          -----
// -----                                                                             -----
// ---------------------------------------------------------------------------------------

#ifndef R3BSTARTRACKHITMAPPED_H
#define R3BSTARTRACKHITMAPPED_H

#include "R3BStarTrackRawHit.h"

class R3BStartrackRawHitMapped : public R3BStarTrackRawHit
{
  public:
    // Default Constructor
    R3BStarTrackRawHitMapped();

    /** Standard Constructor
     **/
    R3BStarTrackRawHitMapped( UShort_t adc_data, ULong_t time, UShort_t moduleId, UShort_t side, UShort_t asicId, UShort_t stripId);

    // Destructor
    virtual ~R3BLandRawHitMapped()
    {
    }

    // Getters
    inline const Int_t& GetModuleId() const
    {
        return fModuleId;
    }
    inline const Int_t& GetSide() const
    {
        return fSide;
    }
    inline const Int_t& GetAsicId() const
    {
        return fAsicId;
    }
    inline const Int_t& GetStrip() const
    {
        return fStrip;
    }


  private:
    UShort_t fModuleId;   //... detector ID
    Ushort_t fSide;       //...
    Ushort_t fAsicId;     //... 
    Ushort_t fStrip;      //... 

  public:
    ClassDef(R3BStarTrackRawHitMapped, 2)
};

#endif
