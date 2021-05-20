/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019 Members of R3B Collaboration                          *
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
#include "FairTask.h"

struct EXT_STR_h101_TOFI_t;
typedef struct EXT_STR_h101_TOFI_t EXT_STR_h101_TOFI;
class FairLogger;

class R3BTofiReader : public R3BReader
{
  public:
    R3BTofiReader(EXT_STR_h101_TOFI*, UInt_t);
    ~R3BTofiReader();

    Bool_t Init(ext_data_struct_info*);
    Bool_t Read();
    void Reset();

  private:
    /* Reader specific data structure from ucesb */
    EXT_STR_h101_TOFI* fData;
    /* Data offset */
    UInt_t fOffset;
    /* FairLogger */
    FairLogger* fLogger;
    /* the structs of type R3BTofixMappedItem */
    TClonesArray* fArray; /**< Output array. */
    TClonesArray* fArrayTrigger; /**< Output array for triggers. */
    R3BEventHeader* header;

  public:
    ClassDef(R3BTofiReader, 0);
};

#endif
