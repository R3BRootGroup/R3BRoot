/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2024 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#ifndef R3BTOFIREADER_H
#define R3BTOFIREADER_H

#include "R3BReader.h"
#include <Rtypes.h>

class TClonesArray;

struct EXT_STR_h101_TOFI_t;
typedef struct EXT_STR_h101_TOFI_t EXT_STR_h101_TOFI;
typedef struct EXT_STR_h101_TOFI_onion_t EXT_STR_h101_TOFI_onion;
class ext_data_struct_info;

class R3BTofiReader : public R3BReader
{
  public:
    // Standard constructor
    R3BTofiReader(EXT_STR_h101_TOFI*, size_t);

    // Destructor
    virtual ~R3BTofiReader();

    // Setup structure information
    virtual Bool_t Init(ext_data_struct_info*) override;

    // Read data from full event structure
    virtual Bool_t R3BRead() override;

    // Reset
    virtual void Reset() override;

  private:
    // Reader specific data structure from ucesb
    EXT_STR_h101_TOFI* fData;
    // Data offset
    size_t fOffset;
    // Don't store data for online
    Bool_t fOnline;
    // Output array
    TClonesArray* fArray;
    TClonesArray* fArrayTrigger; /**< Output array for triggers. */
    R3BEventHeader* header;

  public:
    ClassDefOverride(R3BTofiReader, 0);
};

#endif // R3BTOFIREADER_H
