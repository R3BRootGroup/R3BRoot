// ---------------------------------------------------------------------------------------
// -----                                                                             -----
// -----                           R3BLandHitTmp                                     -----
// -----                           Version 0.1                                       -----
// -----                      Created  @ 03.2014 by M.I. Cherciu                     -----
// -----                                                                             -----
// ---------------------------------------------------------------------------------------

#ifndef R3BLANDHITTMP_H
#define R3BLANDHITTMP_H

#include "TObject.h"


class R3BLandHitTmp : public TObject {
public:
  // Default Constructor
  R3BLandHitTmp();
  
  /** Standard Constructor
   *@param bar_id   tac mod address
   *@param time     tac channel no
   *@param charge   calibration bit
   **/
  R3BLandHitTmp(Int_t ibar_id, Int_t dtime, Int_t icharge);
  
  //Destructor
  virtual ~R3BLandHitTmp() { }
  
  //Getters
  inline const Int_t& GetBarId()    const { return barid;  }
  inline const Int_t& GetTime()     const { return time;   }
  inline const Int_t& GetCharge()   const { return charge; }

private:
  Int_t barid;    //... tac mod address
  Int_t time;      //... tac channel no
  Int_t charge;      //... calibration bit 
  
public:
  ClassDef(R3BLandHitTmp,1)
};

#endif
