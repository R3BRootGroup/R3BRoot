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

#ifndef R3BSCI2READER_H
#define R3BSCI2READER_H

#include "FairTask.h"
#include "R3BReader.h"

struct EXT_STR_h101_SCI2_t;
typedef struct EXT_STR_h101_SCI2_t EXT_STR_h101_SCI2;
class ext_data_struct_info;

class R3BSci2Reader : public R3BReader
{
  public:
    R3BSci2Reader(EXT_STR_h101_SCI2*, UInt_t);
    ~R3BSci2Reader();

    Bool_t Init(ext_data_struct_info*);
    Bool_t Read();
    void Reset();
    virtual void FinishTask();

  private:
    /* Reader specific data structure from ucesb */
    EXT_STR_h101_SCI2* fData;
    /* Data offset */
    UInt_t fOffset;
    /* the structs of type R3BSci2xMappedItem */
    TClonesArray* fArray; /**< Output array. */
    Int_t fNEvents = 0;

  public:
    ClassDef(R3BSci2Reader, 0);
};

#endif
