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

#ifndef R3BROLUREADER_H
#define R3BROLUREADER_H

#include "R3BReader.h"
#include <Rtypes.h>

class TClonesArray;

struct EXT_STR_h101_ROLU_t;
typedef struct EXT_STR_h101_ROLU_t EXT_STR_h101_ROLU;
class ext_data_struct_info;

class R3BRoluReader : public R3BReader
{
  public:
    // Standard constructor
    R3BRoluReader(EXT_STR_h101_ROLU*, size_t);

    // Destructor
    virtual ~R3BRoluReader();

    // Setup structure information
    virtual Bool_t Init(ext_data_struct_info*) override;

    // Read data from full event structure
    virtual Bool_t Read() override;

    // Reset
    virtual void Reset() override;

    // Accessor to select online mode
    void SetOnline(Bool_t option) { fOnline = option; }

  private:
    // Reader specific data structure from ucesb
    EXT_STR_h101_ROLU* fData;
    // Data offset
    size_t fOffset;
    // Output array
    TClonesArray* fArray;
    TClonesArray* fArrayTrigger;
    // Don't store data for online
    Bool_t fOnline;

    Int_t fNEvents = 0;
    R3BEventHeader* header;

  public:
    ClassDefOverride(R3BRoluReader, 0);
};

#endif // R3BROLUREADER_H
