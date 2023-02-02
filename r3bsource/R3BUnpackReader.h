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

#ifndef R3BUNPACKREADER_H
#define R3BUNPACKREADER_H 1

#include "R3BReader.h"
#include <Rtypes.h>

struct EXT_STR_h101_unpack_t;
typedef struct EXT_STR_h101_unpack_t EXT_STR_h101_unpack;

class ext_data_struct_info;
class FairLogger;
class R3BEventHeader;

class R3BUnpackReader : public R3BReader
{
  public:
    R3BUnpackReader(EXT_STR_h101_unpack*, UInt_t);
    
    virtual ~R3BUnpackReader();

    virtual Bool_t Init(ext_data_struct_info*) override;
    virtual Bool_t Read() override;
    virtual void Reset() override;

  private:
    /* An event counter */
    UInt_t fNEvent;
    /* Reader specific data structure from ucesb */
    EXT_STR_h101_unpack* fData;
    /* Offset of detector specific data in full data structure */
    UInt_t fOffset;
    /* FairLogger */
    FairLogger* fLogger;
    R3BEventHeader* fHeader;

  public:
    ClassDefOverride(R3BUnpackReader, 0);
};

#endif
