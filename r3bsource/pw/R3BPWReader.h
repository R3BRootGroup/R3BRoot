/******************************************************************************
 *   Copyright (C) 2026 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2026 Members of R3B Collaboration                          *
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
#include <memory>

class TClonesArray;

/* The structure containing the data layout of PW detector */
struct EXT_STR_h101_PW_t;
typedef struct EXT_STR_h101_PW_t EXT_STR_h101_PW;
typedef struct EXT_STR_h101_PW_onion_t EXT_STR_h101_PW_onion;
class ext_data_struct_info;

class R3BPWReader : public R3BReader
{
  public:
    // Standard constructor
    R3BPWReader(EXT_STR_h101_PW*, size_t);

    // Destructor
    virtual ~R3BPWReader() = default;

    // Setup structure information
    auto Init(ext_data_struct_info*) -> Bool_t override;

    // Read data from full event structure
    auto R3BRead() -> Bool_t override;

    // Reset
    void Reset() override;

    // Accessor to select online mode
    void SetOnline(bool option = true) { fOnline = option; }

  private:
    // Event counter
    uint64_t fNEvent = 0;
    // Reader specific data structures from ucesb
    EXT_STR_h101_PW* fData;
    // Output array
    std::unique_ptr<TClonesArray> fArrayPW;
    // Don't store data for online
    bool fOnline = false;
    // Data offset
    size_t fOffset = 0;

  public:
    ClassDefOverride(R3BPWReader, 0);
};
