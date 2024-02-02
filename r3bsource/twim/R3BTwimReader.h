/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2024 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#ifndef R3BTWIMREADER_H
#define R3BTWIMREADER_H 1

#include "R3BReader.h"
#include "TClonesArray.h"
#include <Rtypes.h>

struct EXT_STR_h101_SOFTWIM_t;
typedef struct EXT_STR_h101_SOFTWIM_t EXT_STR_h101_SOFTWIM;
typedef struct EXT_STR_h101_SOFTWIM_onion_t EXT_STR_h101_SOFTWIM_onion;
class ext_data_struct_info;

class R3BTwimReader : public R3BReader
{
  public:
    // Standard constructor
    R3BTwimReader(EXT_STR_h101_SOFTWIM*, size_t);

    // Destructor
    virtual ~R3BTwimReader();

    // Setup structure information
    virtual Bool_t Init(ext_data_struct_info*) override;

    // Read data from full event structure
    virtual Bool_t R3BRead() override;

    // Reset
    virtual void Reset() override;

    // Accessor to select online mode
    void SetOnline(Bool_t option) { fOnline = option; }

    // Accessor to select events without pileup
    void SetSkipPileup() { fPileup = kTRUE; }

    void SetNumSections(Int_t num) { fSections = num; }
    void SetNumAnodes(Int_t num) { fAnodes = num; }
    void SetNumTref(Int_t num) { fTref = num; }
    void SetNumTtrig(Int_t num) { fTtrig = num; }

  private:
    Bool_t ReadData(EXT_STR_h101_SOFTWIM_onion*, UShort_t);

    /* Reader specific data structure from ucesb */
    EXT_STR_h101_SOFTWIM* fData;
    /* Data offset */
    size_t fOffset;
    // Don't store data for online
    Bool_t fOnline;
    /* the structs of type R3BTwimMappedData Item */
    TClonesArray* fArray; /**< Output array. */
    /* Geometry */
    Int_t fSections;
    Int_t fAnodes;
    Int_t fTref;
    Int_t fTtrig;
    Bool_t fPileup;

  public:
    ClassDefOverride(R3BTwimReader, 0);
};

#endif // R3BTWIMREADER_H
