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

class R3BLandRawHit : public TObject
{
  public:
    // Default Constructor
    R3BLandRawHit();

    /** Standard Constructor
     *@param Tac_addr   tac mod address
     *@param Tac_ch     tac channel no
     *@param Cal        calibration bit
     *@param clock      clock data
     *@param Tac_data   tac data
     *@param Qdc_data   qdc data
     **/
    R3BLandRawHit(UShort_t sam, UShort_t gtb, UShort_t tacAddr, UShort_t tacCh, UShort_t cal, UShort_t clock, UShort_t tacData, UShort_t qdcData);
    
    R3BLandRawHit(const R3BLandRawHit&);

    // Destructor
    virtual ~R3BLandRawHit()
    {
    }

    inline const UShort_t& GetSam() const
    {
        return fSam;
    }
    inline const UShort_t& GetGtb() const
    {
        return fGtb;
    }
    inline const UShort_t& GetTacCh() const
    {
        return fTacCh;
    }
    inline const UShort_t& GetTacAddr() const
    {
        return fTacAddr;
    }
    inline const UShort_t& GetCal() const
    {
        return fCal;
    }
    inline const UShort_t& GetClock() const
    {
        return fClock;
    }
    inline const UShort_t& GetTacData() const
    {
        return fTacData;
    }
    inline const UShort_t& GetQdcData() const
    {
        return fQdcData;
    }

  protected:
    UShort_t fSam;
    UShort_t fGtb;
    UShort_t fTacAddr; //... TAC module address
    UShort_t fTacCh;   //... TAC channel number
    UShort_t fCal;     //... Calibration bit
    UShort_t fClock;   //... Clock data
    UShort_t fTacData; //... TAC data
    UShort_t fQdcData; //... QDC data

  public:
    ClassDef(R3BLandRawHit, 1)
};

#endif
