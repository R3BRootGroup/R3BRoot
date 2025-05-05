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

// ---------------------------------------------------------------------
// -----                     R3BHmpReader                          -----
// -----             Created 17/04/2025 by L. Rose                 -----
// ---------------------------------------------------------------------

#pragma once

#include "R3BReader.h"
#include <stdint.h>

class TClonesArray;

struct EXT_STR_h101_HMP_t;
typedef struct EXT_STR_h101_HMP_t EXT_STR_h101_HMP;
typedef struct EXT_STR_h101_HMP_onion_t EXT_STR_h101_HMP_onion;
class ext_data_struct_info;

class R3BHmpReader : public R3BReader
{
  public:
    // Standard constructor
    R3BHmpReader(EXT_STR_h101_HMP_onion*, size_t);

    // Destructor
    virtual ~R3BHmpReader();

    // Setup structure information
    virtual Bool_t Init(ext_data_struct_info*) override;

    // Read data from full event structure
    virtual Bool_t R3BRead() override;

    // Reset
    virtual void Reset() override;

    // Accessor to select online mode
    void SetOnline(bool option = true) { fOnline = option; }

  private:
    // An event counter
    unsigned int fNEvent = 0;
    // Number of HMP sensors
    // Reader specific data structure from ucesb
    EXT_STR_h101_HMP_onion* fData;
    // Data offset
    size_t fOffset = 0;
    // Don't store data for online
    bool fOnline = false;
    // Output array
    TClonesArray* fArray = nullptr;

  public:
    ClassDefOverride(R3BHmpReader, 1); // NOLINT
};
