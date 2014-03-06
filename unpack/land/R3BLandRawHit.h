// ---------------------------------------------------------------------------------------
// -----                                                                             -----
// -----                           R3BLandRawHit                                     -----
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
  R3BLandRawHit(UShort_t Sam, UShort_t Gtb, UShort_t Tac_addr, UShort_t Tac_ch,
                UShort_t Cal, UShort_t Cntrl_data, UShort_t Tac_data, UShort_t Qdc_data);
  R3BLandRawHit(UShort_t Tac_addr, UShort_t Tac_ch, UShort_t Cal,
                UShort_t Cntrl_data, UShort_t Tac_data, UShort_t Qdc_data);
  
  //Destructor
  virtual ~R3BLandRawHit() { }
  
  inline const UShort_t&  GetTacCh()    const { return ftch;  }
  inline const UShort_t&  GetTacAddr()    const { return ftaddr;  }
  inline const UShort_t&  GetQdcData()    const { return fqdata;  }
  inline const UShort_t&  GetTacData()    const { return ftdata;  }
  inline const UShort_t&  GetSam()    const { return fsam;  }
  inline const UShort_t&  GetGtb()    const { return fgtb;  }

private:
  UShort_t ftaddr;    //... tac mod address
  UShort_t ftch;      //... tac channel no
  UShort_t fcal;      //... calibration bit
  UShort_t fcntdata;  //... cntrl data
  UShort_t ftdata;    //... tac data
  UShort_t fqdata;
  UShort_t fsam;
  UShort_t fgtb;
  
public:
  ClassDef(R3BLandRawHit,1)
};

#endif
