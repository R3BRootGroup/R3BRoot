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
// -----                        R3BStartrackRawHit                         -----
// -----                           Version 0.1                             -----
// -----                    Created 28.05.2014 by M. Labiche               -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#ifndef R3BSTARTRACKRAWHIT_H
#define R3BSTARTRACKRAWHIT_H

#include "TObject.h"

class R3BStartrackRawHit : public TObject
{
  public:
    // Default Constructor
    R3BStartrackRawHit();

    /** Standard Constructor
     *@param moduleId    module unique identifier
     *@param side        module Side unique identifier
     *@param asicId      asic unique identifier
     *@param stripId     Strip unique identifier
     *@param adc         Total energy deposited on the crystal [ch]
     *@param time        Time since event start [ns]
     **/

    // R3BStartrackRawHit( UInt_t WRvhb, UInt_t WRhb , UInt_t WRlb, UInt_t wordtype, UInt_t hitbit, UInt_t moduleId,
    // UInt_t side, UInt_t asicId, UInt_t stripId, UInt_t adc_data, UInt_t timevhb, UInt_t timehb, UInt_t timelb, UInt_t
    // timeExtvhb, UInt_t timeExthb, UInt_t timeExtlb, UInt_t infofield, UInt_t infocode);
    R3BStartrackRawHit(UInt_t WRvhb,
                       UInt_t WRhb,
                       UInt_t WRlb,
                       UInt_t wordtype,
                       UInt_t hitbit,
                       UInt_t moduleId,
                       UInt_t side,
                       UInt_t asicId,
                       UInt_t stripId,
                       UInt_t adc_data,
                       ULong_t timevhb,
                       ULong_t timehb,
                       UInt_t timelb,
                       UInt_t timeExtvhb,
                       UInt_t timeExthb,
                       UInt_t timeExtlb,
                       UInt_t time47lb,
                       UInt_t infofield,
                       UInt_t infocode);

    // vhb stands for very high bit
    // hb stands for  high bit
    // lb stands for  low bit

    // Destructor
    ~R3BStartrackRawHit() {}

    // Getters
    inline const UInt_t& GetWRvhb() const { return fWRvhb; }
    inline const UInt_t& GetWRhb() const { return fWRhb; }
    inline const UInt_t& GetWRlb() const { return fWRlb; }
    inline const UInt_t& GetWordType() const { return fWordType; }
    inline const UInt_t& GetHitBit() const { return fHitBit; }
    inline const UInt_t& GetModuleId() const { return fModuleId; }
    inline const UInt_t& GetSide() const { return fSide; }
    inline const UInt_t& GetAsicId() const { return fAsicId; }
    inline const UInt_t& GetStripId() const { return fStripId; }
    inline const UInt_t& GetADCdata() const { return fADCdata; }
    // inline const UInt_t&  GetTimevhb()      const { return fTimevhb;      }
    inline const ULong_t& GetTimevhb() const
    {
        return fTimevhb;
    } // to be used if Time stamp is reconstructed in the unpacker (otherwise UInt_t is sufficient)
    // inline const UInt_t&  GetTimehb()      const { return fTimehb;      }
    inline const ULong_t& GetTimehb() const
    {
        return fTimehb;
    } // to be used if Time stamp is reconstructed in the unpacker (otherwise UInt_t is sufficient)
    inline const UInt_t& GetTimelb() const { return fTimelb; }
    inline const UInt_t& GetTimeExtvhb() const { return fTimeExtvhb; }
    inline const UInt_t& GetTimeExthb() const { return fTimeExthb; }
    inline const UInt_t& GetTimeExtlb() const { return fTimeExtlb; }
    inline const UInt_t& GetTime47lb() const { return fTime47lb; }
    inline const UInt_t& GetInfoField() const { return fInfoField; }
    inline const UInt_t& GetInfoCode() const { return fInfoCode; }

  protected:
    // ULong_t  fWR;        //header White Rabbit time (all bits) of the interaction
    UInt_t fWRvhb;    // header White Rabbit time (48 - 63 bits) of the interaction
    UInt_t fWRhb;     // header White Rabbit time (28-47 bits) of the interaction
    UInt_t fWRlb;     // header White Rabbit time (low bit) of the interaction
    UInt_t fWordType; //
    UInt_t fHitBit;   //
    UInt_t fModuleId; // module unique identifier
    UInt_t fSide;     // module side unique identifier (128 strip per chip)
    UInt_t fAsicId;   // chip unique identifier
    UInt_t fStripId;  // strip unique identifier
    UInt_t fADCdata;  // total energy in the crystal
    // ULong_t  fTime;        //time (all 64 bits) of the interaction
    // UInt_t  fTimevhb;        //time (48 - 63 bits) of the interaction
    ULong_t fTimevhb; // time (48 - 63 bits) of the interaction // to be used if Time stamp is reconstructed in the
                      // unpacker (otherwise UInt_t is sufficient)
    // UInt_t  fTimehb;        //time ( 28-47 bits) of the interaction
    ULong_t fTimehb; // time ( 28-47 bits) of the interaction // to be used if Time stamp is reconstructed in the
                     // unpacker (otherwise UInt_t is sufficient)
    UInt_t fTimelb;     // time (low bit) of the interaction
    UInt_t fTimeExtvhb; // time (48 - 63 bits) of the external input
    UInt_t fTimeExthb;  // time (28-47  bits) of the external input
    UInt_t fTimeExtlb;  // time (low bit) of the external input
    UInt_t fTime47lb;   // time (low bit) of the wordtype2 input
    UInt_t fInfoField;  // time of the interaction
    UInt_t fInfoCode;   // time of the interaction

  public:
    ClassDef(R3BStartrackRawHit, 1)
};

#endif
