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

#ifndef R3BNEULANDTAMEXREADER_H
#define R3BNEULANDTAMEXREADER_H

#include "R3BPaddleTamexMappedData.h"
#include "R3BReader.h"
#include <R3BIOConnector.h>
#include <Rtypes.h>

class TClonesArray;

struct EXT_STR_h101_raw_nnp_tamex_t;
typedef struct EXT_STR_h101_raw_nnp_tamex_t EXT_STR_h101_raw_nnp_tamex;
typedef struct EXT_STR_h101_raw_nnp_tamex_onion_t EXT_STR_h101_raw_nnp_tamex_onion;
class ext_data_struct_info;

class R3BNeulandTamexReader : public R3BReader
{
  public:
    // Standard constructor
    R3BNeulandTamexReader(EXT_STR_h101_raw_nnp_tamex_onion*, size_t);

    // Destructor
    virtual ~R3BNeulandTamexReader() override;

    // Setup structure information
    virtual Bool_t Init(ext_data_struct_info*) override;

    // Read data from full event structure
    virtual Bool_t R3BRead() override;

    // Reset
    virtual void Reset() override;

    // Accessor to select online mode
    void SetOnline(Bool_t option) { fOnline = option; }

    // Set the maximum number of planes
    void SetMaxNbPlanes(UInt_t max) { fNofPlanes = max; }

    // Accessor to skip trigger times
    void SetSkipTriggerTimes() { fSkiptriggertimes = kTRUE; }
    void SetT0Enabled(bool is_enabled = true) { is_t0_enabled_ = is_enabled; }

  private:
    EXT_STR_h101_raw_nnp_tamex_onion* fData; // Reader specific data structure from ucesb
    size_t fOffset;                          // Data offset
    TClonesArray* fArray;                    // Output array
    TClonesArray* fArrayTrigger;             // Output array, cards' trigger
    R3B::OutputMapConnector<int, R3BPaddleTamexMappedData> trigger0_data_{ "NeulandTrig0MappedData" };
    UInt_t fNofPlanes; // Number of planes
    bool is_t0_enabled_ = false;
    Bool_t fOnline;           // Don't store data for online
    Bool_t fSkiptriggertimes; // Skip trigger times

    void fill_trigger0();

  public:
    ClassDefOverride(R3BNeulandTamexReader, 0);
};

#endif
