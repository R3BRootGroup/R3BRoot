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

// ----------------------------------------------------------------------
// -----                                                            -----
// -----                         R3BAmsReader                       -----
// -----             Created 12/05/18  by J.L. Rodriguez-Sanchez    -----
// ----------------------------------------------------------------------

#ifndef R3BAMSREADER_H
#define R3BAMSREADER_H

#include "R3BReader.h"

class TClonesArray;

struct EXT_STR_h101_AMS_t;
typedef struct EXT_STR_h101_AMS_t EXT_STR_h101_AMS;
typedef struct EXT_STR_h101_AMS_onion_t EXT_STR_h101_AMS_onion;
class ext_data_struct_info;

class R3BAmsReader : public R3BReader
{
  public:
    // Standard constructor
    R3BAmsReader(EXT_STR_h101_AMS_onion*, UInt_t);
    virtual ~R3BAmsReader();

    Bool_t Init(ext_data_struct_info*);
    Bool_t Read();
    void Reset();

    // Accessor to select online mode
    void SetOnline(Bool_t option) { fOnline = option; }

  private:
    // An event counter
    unsigned int fNEvent;
    // Reader specific data structure from ucesb
    EXT_STR_h101_AMS_onion* fData;
    // Data offset
    UInt_t fOffset;
    // Don't store data for online
    Bool_t fOnline;
    // Output array
    TClonesArray* fArray;

  public:
    ClassDef(R3BAmsReader, 0);
};

#endif
