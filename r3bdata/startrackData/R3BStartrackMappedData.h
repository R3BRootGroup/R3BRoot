/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2023 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                       R3BStartrackMappedData                      -----
// -----                   Created 15/05/2017 by M. Labiche                -----
// -----                    based on initial R3B unpacker:                 -----
// -----                 startrack/unpack/StartrackRawHit class            -----
// -----------------------------------------------------------------------------

#ifndef R3BSTARTRACKMAPPEDDATA_H
#define R3BSTARTRACKMAPPEDDATA_H
#include "TObject.h"

class R3BStartrackMappedData : public TObject
{

  public:
    // Default Constructor
    R3BStartrackMappedData();

    /** Standard Constructor
     *@param ts          full time stamp from strip hit
     *@param tsext       full time stamp from external signal
     *@param wordtype    Type of 32bit word (10=0x2 or 11=0x3)
     *@param hitbit      hitbit control. (should be =1)
     *@param ladderId    ladder identifiation (1-18)
     *@param side        side identification (n or p) of the ladder
     *@param asicId      asic identification of the ladder (0-11 for inner and  0-15 for outer)
     *@param stripId     strip identification (0-127)
     *@param ADCdata     Total energy deposited in the crystal [a.u.]
     *@param timevhb     less significant bit of the strip hit time stamp (very high bit: 48 to 63)  -> 16 bits
     *@param timehb      intermediate bits of the strip hit time stamp (very high bit: 28 to 47)     -> 20 bits in total
     *@param timelb      more significant bit of the strip hit time stamp (very high bit: 0 to 27)   -> 28 bits in total
     *@param timevhb     less significant bit of the external signal time stamp (very high bit: 48 to 63)  -> 16 bits
     *@param timehb      intermediate bits of the external signal time stamp (very high bit: 28 to 47)     -> 20 bits in
     *total
     *@param timelb      more significant bit of the external signal time stamp (very high bit: 0 to 27)   -> 28 bits in
     *total
     *@param ADCdata     Total energy deposited in the crystal [a.u.]
     *@param infofield   other information
     *@param infocode    Code associated to the type of the above information (infofield)
     **/
    //  R3BStartrackMappedData(UInt_t wrvhb, UInt_t wrhb, UInt_t wrlb, Int_t wordtype, Int_t hitbit, Int_t ladderId,
    //  Int_t side, Int_t asicId, Int_t stripId, UInt_t adc_data, UInt_t timevhb, UInt_t timehb, UInt_t timelb, UInt_t
    //  timeExtvhb, UInt_t timeExthb, UInt_t timeExtlb, Int_t infofield, Int_t infocode);
    R3BStartrackMappedData(ULong_t ts,
                           ULong_t tsext,
                           Int_t wordtype,
                           Int_t hitbit,
                           Int_t ladderId,
                           Int_t side,
                           Int_t asicId,
                           Int_t stripId,
                           UInt_t adc_data,
                           UInt_t timevhb,
                           UInt_t timehb,
                           UInt_t timelb,
                           UInt_t timeExtvhb,
                           UInt_t timeExthb,
                           UInt_t timeExtlb,
                           Int_t infofield,
                           Int_t infocode);

    // Destructor
    ~R3BStartrackMappedData() {}

    // Getters
    //  inline const UInt_t&   GetWRTimevhb()  const { return fWRvhb;   }
    //  inline const UInt_t&   GetWRTimehb()   const { return fWRhb;    }
    //  inline const UInt_t&   GetWRTimelb()   const { return fWRlb;    }
    inline const ULong_t& GetTS() const { return fTS; }
    inline const ULong_t& GetTSExt() const { return fTSExt; }
    inline const Int_t& GetWordtype() const { return fWordtype; }
    inline const Int_t& GetHitbit() const { return fHitbit; }
    inline const Int_t& GetLadderId() const { return fLadderId; }
    inline const Int_t& GetSide() const { return fSide; }
    inline const Int_t& GetAsicId() const { return fAsicId; }
    inline const Int_t& GetStripId() const { return fStripId; }
    inline const UInt_t& GetADCdata() const { return fADCdata; } // Energy non calibrated
    inline const UInt_t& GetTimevhb() const { return fTimevhb; }
    inline const UInt_t& GetTimehb() const { return fTimehb; }
    inline const UInt_t& GetTimelb() const { return fTimelb; }
    inline const UInt_t& GetTimeExtvhb() const { return fTimeExtvhb; }
    inline const UInt_t& GetTimeExthb() const { return fTimeExthb; }
    inline const UInt_t& GetTimeExtlb() const { return fTimeExtlb; }
    inline const Int_t& GetInfocode() const { return fInfoCode; }
    inline const Int_t& GetInfoField() const { return fInfoField; }

  protected:
    //  UInt_t  fWRvhb;      // White rabbit time stamp (very high bits)
    //  UInt_t  fWRhb;       // White rabbit time stamp (high bits)
    //  UInt_t  fWRlb;       // White rabbit time stamp (low bits)
    ULong_t fTS;        // full reconstructed time stamp
    ULong_t fTSExt;     // full reconstructed time stamp of external signal
    Int_t fWordtype;    // Word type
    Int_t fHitbit;      // hit type
    Int_t fLadderId;    // LadderId
    Int_t fSide;        // total slow amplitude in the crystal
    Int_t fAsicId;      // total slow amplitude in the crystal
    Int_t fStripId;     // total slow amplitude in the crystal
    UInt_t fADCdata;    // total slow amplitude in the crystal
    UInt_t fTimevhb;    // time-stamp (common to all the hits in the event)
    UInt_t fTimehb;     // time-stamp (common to all the hits in the event)
    UInt_t fTimelb;     // time-stamp (common to all the hits in the event)
    UInt_t fTimeExtvhb; // time-stamp (common to all the hits in the event)
    UInt_t fTimeExthb;  // time-stamp (common to all the hits in the event)
    UInt_t fTimeExtlb;  // time-stamp (common to all the hits in the event)
    Int_t fInfoCode;    // time-stamp (common to all the hits in the event)
    Int_t fInfoField;   // time-stamp (common to all the hits in the event)

  public:
    ClassDef(R3BStartrackMappedData, 1)
};

#endif
