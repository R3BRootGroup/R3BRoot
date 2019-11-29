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

#ifndef R3BNEULANDTAMEXREADER_H
#define R3BNEULANDTAMEXREADER_H

#include "R3BReader.h"
class TClonesArray;

struct EXT_STR_h101_raw_nnp_tamex_t;
typedef struct EXT_STR_h101_raw_nnp_tamex_t EXT_STR_h101_raw_nnp_tamex;

class R3BNeulandTamexReader : public R3BReader
{
  public:
    R3BNeulandTamexReader(EXT_STR_h101_raw_nnp_tamex*, UInt_t);
    ~R3BNeulandTamexReader() override;

    Bool_t Init(ext_data_struct_info*) override;
    Bool_t Read() override;
    void Reset() override;

  private:
    EXT_STR_h101_raw_nnp_tamex* fData; // Reader specific data structure from ucesb
    UInt_t fOffset;                    // Data offset
    TClonesArray* fArray;              // Output array
    const UInt_t fNofPlanes;

  public:
    ClassDefOverride(R3BNeulandTamexReader, 0);
};

#endif
