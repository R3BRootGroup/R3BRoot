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

#ifndef R3BGadastReader_H
#define R3BGadastReader_H

#include "R3BReader.h"
#include <Rtypes.h>

class TClonesArray;

#define NUM_GADAST_ANODES 2
// anodes 0 to 7 : energy and time
// anode 8 and 9 : reference times

struct EXT_STR_h101_GADAST_t;
typedef struct EXT_STR_h101_GADAST_t EXT_STR_h101_GADAST;
typedef struct EXT_STR_h101_GADAST_onion_t EXT_STR_h101_GADAST_onion;

class R3BGadastReader : public R3BReader
{
  public:
    // Standard constructor
    R3BGadastReader(EXT_STR_h101_GADAST_onion*, size_t);

    // Destructor
    virtual ~R3BGadastReader();

    // Setup structure information
    virtual Bool_t Init(ext_data_struct_info*) override;

    // Read data from full event structure
    virtual Bool_t R3BRead() override;

    // Reset
    virtual void Reset() override;

    /** Accessor to select online mode **/
    void SetOnline(Bool_t option) { fOnline = option; }

  private:
    Bool_t ReadData();
    uint32_t multPerAnode[NUM_GADAST_ANODES];
    // Don't store data for online
    Bool_t fOnline;
    // Reader specific data structure from ucesb
    EXT_STR_h101_GADAST_onion* fData;
    // Data offset
    size_t fOffset;
    // Output array of type R3BGADASTMappedData
    TClonesArray* fArray;

  public:
    ClassDefOverride(R3BGadastReader, 0);
};

#endif // R3BGadastReader_H
