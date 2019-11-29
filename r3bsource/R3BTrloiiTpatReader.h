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

#ifndef R3BTRLOIITPATREADER_H
#define R3BTRLOIITPATREADER_H
#include "R3BReader.h"

struct EXT_STR_h101_TPAT_t;
typedef struct EXT_STR_h101_TPAT_t EXT_STR_h101_TPAT;

class FairLogger;
class R3BEventHeader;

class R3BTrloiiTpatReader : public R3BReader
{
  public:
    R3BTrloiiTpatReader(EXT_STR_h101_TPAT*, UInt_t);
    ~R3BTrloiiTpatReader();

    Bool_t Init(ext_data_struct_info*);
    Bool_t Read();
    void Reset();

  private:
    /* An event counter */
    UInt_t fNEvent;
    /* Reader specific data structure from ucesb */
    EXT_STR_h101_TPAT* fData;
    /* Offset of detector specific data in full data structure */
    UInt_t fOffset;
    /* A pointer to the R3BEventHeader structure */
    R3BEventHeader* fEventHeader;

  public:
    ClassDef(R3BTrloiiTpatReader, 0);
};

#endif
