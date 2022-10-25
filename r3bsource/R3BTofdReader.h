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
class R3BEventHeader;

struct EXT_STR_h101_TOFD_t;
typedef struct EXT_STR_h101_TOFD_t EXT_STR_h101_TOFD;
typedef struct EXT_STR_h101_TOFD_onion_t EXT_STR_h101_TOFD_onion;
class ext_data_struct_info;

class R3BTofdReader : public R3BReader
{
  public:
    R3BTofdReader(EXT_STR_h101_TOFD*, UInt_t);
    ~R3BTofdReader();

    Bool_t Init(ext_data_struct_info*);
    Bool_t Read();
    void Reset();

    /** Accessor to select online mode **/
    void SetOnline(Bool_t option) { fOnline = option; }

  private:
    R3BEventHeader* header; /**< Event header. */
    /* Reader specific data structure from ucesb */
    EXT_STR_h101_TOFD* fData;
    /* Data offset */
    UInt_t fOffset;
    // Don't store data for online
    Bool_t fOnline;
    /* the structs of type R3BTofdxMappedItem */
    TClonesArray* fArray;        /**< Output array. */
    TClonesArray* fArrayTrigger; /**< Output array for triggers. */
	Int_t fNEvents = 0;
    Int_t ieventnumer[1000];

  public:
    ClassDef(R3BTofdReader, 0);
};

#endif
