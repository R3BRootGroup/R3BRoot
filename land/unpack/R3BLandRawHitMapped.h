// ---------------------------------------------------------------------------------------
// -----                                                                             -----
// -----                       R3BLandRawHitMapped                                   -----
// -----                           Version 0.1                                       -----
// -----                 Created  @ 03.2014 by M.I. Cherciu                          -----
// -----                                                                             -----
// ---------------------------------------------------------------------------------------

#ifndef R3BLANDHITMAPPED_H
#define R3BLANDHITMAPPED_H

#include "TObject.h"

class R3BLandRawHitMapped : public TObject
{
  public:
    // Default Constructor
    R3BLandRawHitMapped();

    /** Standard Constructor
     *@param barId   detector ID
     *@param side    left or right PMT
     *@param time    tac data
     *@param charge  qdc data
     **/
    R3BLandRawHitMapped(Bool_t is17, Int_t cal, Int_t tacAddr, Int_t barId, Int_t side, Int_t clock, Int_t tacData, Int_t qdcData);

    // Destructor
    virtual ~R3BLandRawHitMapped()
    {
    }

    // Getters
    inline const Bool_t& Is17() const
    {
        return fIs17;
    }
    inline const Int_t& GetTacAddr() const
    {
        return fTacAddr;
    }
    inline const Int_t& GetCal() const
    {
        return fCal;
    }
    inline const Int_t& GetBarId() const
    {
        return fBarId;
    }
    inline const Int_t& GetSide() const
    {
        return fSide;
    }
    inline const Int_t& GetClock() const
    {
        return fClock;
    }
    inline const Int_t& GetTacData() const
    {
        return fTacData;
    }
    inline const Int_t& GetQdcData() const
    {
        return fQdcData;
    }

  private:
    Bool_t fIs17;   //... true if CH 17
    Int_t fCal;     //... triggers
    Int_t fTacAddr; //... TAC address
    Int_t fBarId;   //... detector ID
    Int_t fSide;    //... PMT
    Int_t fClock;   //... clock count
    Int_t fTacData; //... TAC
    Int_t fQdcData; //... QDC

  public:
    ClassDef(R3BLandRawHitMapped, 1)
};

#endif
