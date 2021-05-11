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

#ifndef R3BFI33READER_H
#define R3BFI33READER_H

#include "R3BFiberMAPMTReader.h"

typedef struct EXT_STR_h101_FIBEE_t EXT_STR_h101_FIBEE;
typedef struct EXT_STR_h101_FIBEE_onion_t EXT_STR_h101_FIBEE_onion;

class R3BFi33Reader : public R3BFiberMAPMTReader
{
  public:
    R3BFi33Reader(EXT_STR_h101_FIBEE*, UInt_t);
    Bool_t Init(ext_data_struct_info*);

  private:
    EXT_STR_h101_FIBEE_onion* fData;

  public:
    ClassDef(R3BFi33Reader, 1);
};

#endif
