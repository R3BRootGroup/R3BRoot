// ---------------------------------------------------------------------------------------
// -----                                                                             -----
// -----                       R3BLandRawHitMapped                                   -----
// -----                           Version 0.1                                       -----
// -----                 Created  @ 03.2014 by M.I. Cherciu                          -----
// -----                                                                             -----
// ---------------------------------------------------------------------------------------

#ifndef R3BLANDHITMAPPED_H
#define R3BLANDHITMAPPED_H

#include "R3BLandRawHit.h"

class R3BLandRawHitMapped : public R3BLandRawHit
{
  public:
    // Default Constructor
    R3BLandRawHitMapped();

    /** Standard Constructor
     **/
    R3BLandRawHitMapped(Int_t sam, Int_t gtb, Int_t tacAddr, Int_t cal, Int_t clock, Int_t tacData, Int_t qdcData, Int_t barId, Int_t side, Bool_t is17);

    // Destructor
    virtual ~R3BLandRawHitMapped()
    {
    }

    // Getters
    inline const Int_t& GetBarId() const
    {
        return fBarId;
    }
    inline const Int_t& GetSide() const
    {
        return fSide;
    }
    inline const Bool_t& Is17() const
    {
        return fIs17;
    }

  private:
    Int_t fBarId;   //... detector ID
    Int_t fSide;    //... PMT
    Bool_t fIs17;   //... true if CH 17

  public:
    ClassDef(R3BLandRawHitMapped, 2)
};

#endif
