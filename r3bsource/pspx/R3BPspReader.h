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

#ifndef R3BPSPREADER_H
#define R3BPSPREADER_H 1

#include <TClonesArray.h>
#include "R3BReader.h"
#include <Rtypes.h>

struct EXT_STR_h101_PSP_t;
typedef struct EXT_STR_h101_PSP_t EXT_STR_h101_PSP;
typedef struct EXT_STR_h101_PSP_onion_t EXT_STR_h101_PSP_onion;
class ext_data_struct_info;

class R3BPspReader : public R3BReader
{
  public:
    // Standard constructor
    R3BPspReader(EXT_STR_h101_PSP*, size_t);

    // Destructor
    virtual ~R3BPspReader();

    // Setup structure information
    virtual Bool_t Init(ext_data_struct_info*) override;

    // Read data from full event structure
    virtual Bool_t Read() override;

    // Reset
    virtual void Reset() override;

  private:
    /* An event counter */
    // unsigned int fNEvent;
    /* Reader specific data structure from ucesb */
    EXT_STR_h101_PSP* fData;
    /* Offset */
    size_t fOffset;
    /* the structs of type R3BPspxMappedItem */
    TClonesArray* fArray; /**< Output array. */

  public:
    ClassDefOverride(R3BPspReader, 0);
};

#endif /* R3BPSPREADER_H */
