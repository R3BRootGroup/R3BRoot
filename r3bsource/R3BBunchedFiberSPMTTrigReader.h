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

//
// SPMT trigger reader, since many fiber detectors share TAMEX cards and we
// don't want to recalibrate channels in many places.
//

#ifndef R3BBUNCHEDFIBERSPMTTRIGREADER_H
#define R3BBUNCHEDFIBERSPMTTRIGREADER_H

#include "R3BReader.h"
#include "TString.h"

struct EXT_STR_h101_FIB_t;
typedef struct EXT_STR_h101_FIB_t EXT_STR_h101_FIB;
class TClonesArray;

class R3BBunchedFiberSPMTTrigReader : public R3BReader
{
  public:
    R3BBunchedFiberSPMTTrigReader(EXT_STR_h101_FIB*, UInt_t);
    Bool_t Init(ext_data_struct_info*);
    Bool_t Read();
    void Reset();

  private:
    EXT_STR_h101_FIB* fData;
    UInt_t fOffset;
    TClonesArray* fMappedArray;

  public:
    ClassDef(R3BBunchedFiberSPMTTrigReader, 0);
};

#endif
