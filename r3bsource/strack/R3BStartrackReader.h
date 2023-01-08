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

// Reader for the Silicon Tracker (STarTrack) Detector
// Created 26/05/2017 - Marc Labiche (marc.labiche@stfc.ac.uk)
//
#ifndef R3BSTARTRACKREADER_H
#define R3BSTARTRACKREADER_H

#include "R3BReader.h"
class TClonesArray;
class FairLogger;

struct EXT_STR_h101_STAR_t;
typedef struct EXT_STR_h101_STAR_t EXT_STR_h101_STAR;
class ext_data_struct_info;

class R3BStartrackReader : public R3BReader
{
  public:
    R3BStartrackReader(EXT_STR_h101_STAR*, UInt_t);
    ~R3BStartrackReader();

    Bool_t Init(ext_data_struct_info*);
    Bool_t Read();
    void Reset();

  private:
    /* An event counter */
    // unsigned int fNEvent;
    /* Reader specific data structure from ucesb */
    EXT_STR_h101_STAR* fData;
    /* Offset */
    UInt_t fOffset;
    /* FairLogger */
    FairLogger* fLogger;
    /* the structs of type R3BPspxMappedItem */
    TClonesArray* fArray; /**< Output array. */

    UInt_t fts_vhb;
    UInt_t fts_hb;

  public:
    ClassDef(R3BStartrackReader, 0);
};

#endif
