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

#ifndef R3BPSPREADER_H
#define R3BPSPREADER_H

#include "R3BReader.h"

struct EXT_STR_h101_PSP;
class FairLogger;

class R3BPspReader : public R3BReader
{
  public:
    R3BPspReader(EXT_STR_h101_PSP*, UInt_t);
    ~R3BPspReader();

    Bool_t Init(ext_data_struct_info*);
    Bool_t Read();
    void Reset();

  private:
    /* An event counter */
    // unsigned int fNEvent;
    /* Reader specific data structure from ucesb */
    EXT_STR_h101_PSP* fData;
    /* Offset */
    UInt_t fOffset;
    /* FairLogger */
    FairLogger* fLogger;
    /* the structs of type R3BPspxMappedItem */
    TClonesArray* fArray; /**< Output array. */

  public:
    ClassDef(R3BPspReader, 0);
};

#endif
