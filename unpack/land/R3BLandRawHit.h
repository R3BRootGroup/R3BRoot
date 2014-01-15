// ---------------------------------------------------------------------------------------
// -----                                                                             -----
// -----                           R3BLandUnpack                                     -----
// -----                           Version 0.1                                       -----
// -----         Adapted by M.I. Cherciu @ 01.2014 after Y.Gonzalez code             -----
// -----                                                                             -----
// ---------------------------------------------------------------------------------------

#ifndef R3BLANDRAWHIT_H
#define R3BLANDRAWHIT_H

#include "TObject.h"

class R3BLandRawHit : public TObject {
public:
  // Default Constructor
  R3BLandRawHit();
  
  /** Standard Constructor
   *@param Tac_addr   tac mod address
   *@param Tac_ch     tac channel no
   *@param Cal        calibration bit
   *@param Cntrl_data cntrl data
   *@param Tac_data   tac data
   *@param Qdc_data   qdc data
   **/
  R3BLandRawHit(UShort_t Tac_addr, UShort_t Tac_ch, UShort_t Cal, UShort_t Cntrl_data, UShort_t Tac_data, UShort_t Qdc_data);
  
  //Destructor
  ~R3BLandRawHit() { }
  
  //Getters
//  inline const UShort_t& GetTacAddr()    const { return ftaddr;  }
//  inline const UShort_t& GetTacCh()      const { return ftch;    }
//  inline const ULong_t&  GetCal()        const { return fcal;    }
//  inline const ULong_t&  GetCntrlData()  const { return fcntdata;}
//  inline const ULong_t&  GetTacData()    const { return ftdata;  }

  UShort_t ftaddr;    //... tac mod address
  UShort_t ftch;      //... tac channel no
  UShort_t fcal;      //... calibration bit
  UShort_t fcntdata;  //... cntrl data
  UShort_t ftdata;    //... tac data
  UShort_t fqdata;

  inline const UShort_t&  GetQdcData()    const { return fqdata;  }
  
  
public:
  ClassDef(R3BLandRawHit,1)
};

#endif
