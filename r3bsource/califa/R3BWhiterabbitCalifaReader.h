/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2025 Members of R3B Collaboration                     *
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

struct EXT_STR_h101_WRCALIFA_t;
typedef struct EXT_STR_h101_WRCALIFA_t EXT_STR_h101_WRCALIFA;

class TClonesArray;
class R3BEventHeader;

class R3BWhiterabbitCalifaReader : public R3BReader
{
  public:
    // Standard constructor
    R3BWhiterabbitCalifaReader(EXT_STR_h101_WRCALIFA*,
                               size_t,
                               uint32_t = 0x1a00,
                               uint32_t = 0x1c00,
                               uint32_t = 0x1b00,
                               uint32_t = 0x1d00);

    // Destructor
    virtual ~R3BWhiterabbitCalifaReader();

    // Setup structure information
    Bool_t Init(ext_data_struct_info*) override;

    // Read data from full event structure
    Bool_t R3BRead() override;

    // Reset
    void Reset() override;

    /** Accessor to select online mode **/
    void SetOnline(bool option = true) { fOnline = option; }

  private:
    // An event counter
    uint64_t fNEvent = 0;
    // Reader specific data structure from ucesb
    EXT_STR_h101_WRCALIFA* fData;
    // Offset of detector specific data in full data structure
    size_t fOffset;
    // The whiterabbit subsystem ID
    uint32_t fWhiterabbitId1 = 0;
    uint32_t fWhiterabbitId2 = 0;
    uint32_t fWhiterabbitId3 = 0;
    uint32_t fWhiterabbitId4 = 0;
    // A pointer to the R3BEventHeader structure
    R3BEventHeader* fEventHeader = nullptr;
    // Don't store data for online
    bool fOnline = false;
    // Output array
    TClonesArray* fArray = nullptr;

  public:
    ClassDefOverride(R3BWhiterabbitCalifaReader, 0);
};
