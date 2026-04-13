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

#include "R3BEventHeader.h"
#include "R3BReader.h"
#include <Rtypes.h>
#include <memory>

struct EXT_STR_h101_WRPW_t;
typedef struct EXT_STR_h101_WRPW_t EXT_STR_h101_WRPW;
typedef struct EXT_STR_h101_WRPW_onion_t EXT_STR_h101_WRPW_onion;

class TClonesArray;
class ext_data_struct_info;

class R3BWhiterabbitPWReader : public R3BReader
{
  public:
    // Standard constructor
    R3BWhiterabbitPWReader(EXT_STR_h101_WRPW_onion*, size_t, UInt_t);

    // Destructor
    virtual ~R3BWhiterabbitPWReader() = default;

    // Setup structure information
    auto Init(ext_data_struct_info*) -> Bool_t override;

    // Read data from full event structure
    auto R3BRead() -> Bool_t override;

    // Reset
    void Reset() override;

    // Accessor to select online mode
    void SetOnline(bool option = true) { fOnline = option; }

  private:
    // An event counter
    uint64_t fNEvent = 0;
    // Reader specific data structure from ucesb
    EXT_STR_h101_WRPW_onion* fData;
    // Offset of detector specific data in full data structure
    size_t fOffset = 0;
    // The whiterabbit subsystem ID
    UInt_t fWhiterabbitId = 0;
    // A pointer to the R3BEventHeader structure
    R3BEventHeader* fEventHeader = nullptr;
    // Don't store data for online
    bool fOnline = false;
    // Output array
    std::unique_ptr<TClonesArray> fArray;

  public:
    ClassDefOverride(R3BWhiterabbitPWReader, 0);
};
