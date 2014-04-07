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


class R3BLandRawHitMapped : public TObject {
public:
  // Default Constructor
  R3BLandRawHitMapped();
  
  /** Standard Constructor
   *@param barId   detector ID
   *@param side    left or right PMT
   *@param time    tac data
   *@param charge  qdc data
   **/
  R3BLandRawHitMapped(Bool_t is17, Int_t barId, Int_t side, Int_t time, Int_t charge, Int_t clock);
  
  //Destructor
  virtual ~R3BLandRawHitMapped() { }
  
  //Getters
  inline const Bool_t& Is17()       const { return fIs17;   }
  inline const Int_t&  GetBarId()   const { return fBarId;  }
  inline const Int_t&  GetSide()    const { return fSide;   }
  inline const Int_t&  GetTime()    const { return fTime;   }
  inline const Int_t&  GetCharge()  const { return fCharge; }
  inline const Int_t&  GetClock()   const { return fClock;  }

private:
  Bool_t fIs17;     //... true if CH 17
  Int_t  fBarId;    //... detector ID
  Int_t  fSide;     //... PMT
  Int_t  fTime;     //... TAC
  Int_t  fCharge;   //... QDC
  Int_t  fClock;    //... clock count
  
public:
  ClassDef(R3BLandRawHitMapped,1)
};

#endif
