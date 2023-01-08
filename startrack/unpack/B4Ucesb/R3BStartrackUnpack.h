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
// -----                           R3BStartrackUnpack                            -----
// -----                           Version 0.1                             -----
// -----                    Created 28.05.2014 by M.Labiche               -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#ifndef R3BSTARTRACKUNPACK_H
#define R3BSTARTRACKUNPACK_H

#include "FairUnpack.h"

class TClonesArray;

class R3BStartrackUnpack : public FairUnpack
{
  public:
    // Constructor
    R3BStartrackUnpack(char* strCalDir,
                       Short_t type = 104,
                       Short_t subType = 10400,
                       Short_t procId = 1,
                       Short_t subCrate = 0,
                       Short_t control = 37);

    // Type, SubTypde, procId, subcrate, control are parameters that are unique to the Silicon Tracker.

    // Destructor
    virtual ~R3BStartrackUnpack();

    // Fair specific
    virtual Bool_t Init();
    virtual Bool_t DoUnpack(Int_t* data, Int_t size);
    // virtual Bool_t DoUnpack2(Int_t* data_wd0, Int_t* data_wd1, Int_t size);
    virtual void Reset();

  protected:
    virtual void Register();

  private:
    TClonesArray* fRawData;
    Int_t fNHits;
    Int_t nblock;

    // The information is split into 2 words of 32 bits (4 byte).
    // The words can by of two types: A or B.
    // Type A words have word_0 with msb=10.
    // Type B words have word_0 with msb=11.
    UInt_t word_0A;
    UInt_t word_1A;
    UInt_t word_0B;
    UInt_t word_1B;

    UInt_t wordtype; //
    UInt_t info_field;
    UInt_t info_code;

    // UInt_t ts_vhb;
    ULong_t ts_vhb; // to be used if Time stamp is reconstructed in the unpacker (otherwise UInt_t is sufficient)
    // UInt_t ts_hb;
    ULong_t ts_hb;        // to be used if Time stamp is reconstructed in the unpacker (otherwise UInt_t is sufficient)
    ULong_t ts_code47_lb; // to be used if Time stamp is reconstructed in the unpacker (otherwise UInt_t is sufficient)
    UInt_t ts_lb;
    UInt_t ts_lb_part1;
    UInt_t ts_lb_part2;
    UInt_t ts_lb_part2_inv;
    UInt_t ts_lb_part3;
    // long long ts=0;

    // Time information from an external input (ie: other than Si tracker)
    UInt_t tsExt_vhb; // 63:48 in Si (16 bits)
    UInt_t tsExt_hb;  // 47:28 in Si (20 bits)
    UInt_t tsExt_lb;  // 27:0 in Si (28 bits)
    // long long tsExt=0;

    UInt_t hitbit;    // real values are: 0 or 1
    UInt_t module_id; // module id, real values are  1 to 30
    UInt_t side;      // real values are  0 or 1
    UInt_t asic_id;   // Chip id, real values are 0 to 15
    UInt_t strip_id;  // strip id, real values are 0 to 127
    UInt_t adcData;   // adc value for energy loss in Si

  public:
    // Class definition
    ClassDef(R3BStartrackUnpack, 0)
};

#endif
