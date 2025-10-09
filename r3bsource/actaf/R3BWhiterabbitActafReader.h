/******************************************************************************
 *   Copyright (C) 2025 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2025 Members of R3B Collaboration                          *
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
#include <vector>

struct EXT_STR_h101_WRACTAF_t;
typedef struct EXT_STR_h101_WRACTAF_t EXT_STR_h101_WRACTAF;
typedef struct EXT_STR_h101_WRACTAF_onion_t EXT_STR_h101_WRACTAF_onion;

class TClonesArray;
class ext_data_struct_info;
class R3BEventHeader;

/**
 * A reader of ACTAF white rabbit data with UCESB.
 * Receives mapped raw data and converts it to R3BRoot objects.
 * @author Pablo González Rusell
 * @since Oct 8, 2025
 */

class R3BWhiterabbitActafReader : public R3BReader
{
  public:
    // Standard constructor
    R3BWhiterabbitActafReader(EXT_STR_h101_WRACTAF_onion*, size_t, std::vector<UInt_t>);

    // Destructor
    virtual ~R3BWhiterabbitActafReader();

    // Setup structure information
    virtual Bool_t Init(ext_data_struct_info*) override;

    // Read data from full event structure
    virtual Bool_t R3BRead() override;

    // Reset
    virtual void Reset() override;

    // Accessor to select online mode
    void SetOnline(bool opt = true) { fOnline = opt; }

  private:
    // An event counter
    UInt_t fNEvent = 0;
    // Reader specific data structure from ucesb
    EXT_STR_h101_WRACTAF_onion* fData;
    // Offset of detector specific data in full data structure
    size_t fOffset;
    // The whiterabbit subsystem ID
    std::vector<UInt_t> fWhiterabbitId;
    // Don't store data for online
    bool fOnline = false;
    // A pointer to the R3BEventHeader structure
    R3BEventHeader* fEventHeader = nullptr;
    // Output array
    TClonesArray* fArray = nullptr;

  public:
    ClassDefOverride(R3BWhiterabbitActafReader, 0);
};
