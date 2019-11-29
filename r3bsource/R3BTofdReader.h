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

#ifndef R3BTOFDREADER_H
#define R3BTOFDREADER_H

#include "R3BReader.h"

struct EXT_STR_h101_TOFD_t;
typedef struct EXT_STR_h101_TOFD_t EXT_STR_h101_TOFD;
class FairLogger;

class R3BTofdReader : public R3BReader
{
  public:
    R3BTofdReader(EXT_STR_h101_TOFD*, UInt_t);
    ~R3BTofdReader();

    Bool_t Init(ext_data_struct_info*);
    Bool_t Read();
    void Reset();

  private:
    /* Reader specific data structure from ucesb */
    EXT_STR_h101_TOFD* fData;
    /* Data offset */
    UInt_t fOffset;
    /* FairLogger */
    FairLogger* fLogger;
    /* the structs of type R3BTofdxMappedItem */
    TClonesArray* fArray; /**< Output array. */

  public:
    ClassDef(R3BTofdReader, 0);
};

#endif
