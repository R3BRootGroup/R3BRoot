/******************************************************************************
 *   Copyright (C) 2026 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2026 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#pragma once

#include "R3BReader.h"
#include <Rtypes.h>

class TClonesArray;

/* The structure containing the data layout of PW detector */
struct EXT_STR_h101_PW_t;
typedef struct EXT_STR_h101_PW_t EXT_STR_h101_PW;
typedef struct EXT_STR_h101_PW_onion_t EXT_STR_h101_PW_onion;
class ext_data_struct_info;

class R3BPWReader : public R3BReader
{
  public:
    /* Default constructor / destructor */
    R3BPWReader(EXT_STR_h101_PW*, size_t);
    virtual ~R3BPWReader() = default;

    /* Initialise the detector specific data items in the ext_data_struct_info list of items*/
    auto Init(ext_data_struct_info*) -> Bool_t override;

    /*Copy data from the ucesb data stream to native data containers, called by R3BUcesbSource */
    auto R3BRead() -> Bool_t override;

    /* Reset the internal output array.  */
    void Reset() override;

    // Accessor to select online mode
    void SetOnline(bool option = true) { fOnline = option; }

  private:
    /* Reader specific data structure from ucesb */
    EXT_STR_h101_PW* fData;
    TClonesArray* fArrayPW; /**< Output array. */
    Bool_t fOnline;
    size_t fOffset;
    unsigned int fNEvent;

  public:
    ClassDefOverride(R3BPWReader, 0);
};
