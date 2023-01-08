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

#ifndef R3BLOSREADER_H
#define R3BLOSREADER_H 1

#include "R3BReader.h"
#include <Rtypes.h>

class TClonesArray;

struct EXT_STR_h101_LOS_t;
typedef struct EXT_STR_h101_LOS_t EXT_STR_h101_LOS;
class ext_data_struct_info;

class R3BLosReader : public R3BReader
{
  public:
    // Standard constructor
    R3BLosReader(EXT_STR_h101_LOS*, size_t);

    // Destructor
    virtual ~R3BLosReader();

    // Setup structure information
    virtual Bool_t Init(ext_data_struct_info*) override;

    // Read data from full event structure
    virtual Bool_t Read() override;

    // Reset
    virtual void Reset() override;

    // Accessor to select online mode
    void SetOnline(Bool_t option) { fOnline = option; }

    // Accessor to skip trigger times
    void SetSkipTriggerTimes() { fSkiptriggertimes = kTRUE; }

  private:
    // An event counter
    unsigned int fNEvents;
    //  Reader specific data structure from ucesb
    EXT_STR_h101_LOS* fData;
    // Data offset
    size_t fOffset;
    // Output array of R3BLosxMappedItem
    TClonesArray* fArray;
    TClonesArray* fArrayTrigger;
    // Don't store data for online
    Bool_t fOnline;
    // Skip trigger times
    Bool_t fSkiptriggertimes;
    // EventHeader
    R3BEventHeader* header;

  public:
    ClassDefOverride(R3BLosReader, 0);
};

#endif /* R3BLOSREADER_H */
