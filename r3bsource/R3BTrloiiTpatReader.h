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

#ifndef R3BTRLOIITPATREADER_H
#define R3BTRLOIITPATREADER_H

#include "R3BReader.h"
#include <Rtypes.h>

struct EXT_STR_h101_TPAT_t;
typedef struct EXT_STR_h101_TPAT_t EXT_STR_h101_TPAT;

class R3BEventHeader;

class R3BTrloiiTpatReader : public R3BReader
{
  public:
    R3BTrloiiTpatReader(EXT_STR_h101_TPAT*, size_t);

    // Destructor
    virtual ~R3BTrloiiTpatReader();

    // Setup structure information
    virtual Bool_t Init(ext_data_struct_info*) override;

    // Read data from full event structure
    virtual Bool_t Read() override;

    // Reset
    virtual void Reset() override;

  private:
    // An event counter
    UInt_t fNEvent;
    // Reader specific data structure from ucesb
    EXT_STR_h101_TPAT* fData;
    // Offset of detector specific data in full data structure
    size_t fOffset;
    // A pointer to the R3BEventHeader structure
    R3BEventHeader* fEventHeader;

  public:
    ClassDefOverride(R3BTrloiiTpatReader, 0);
};

#endif /* R3BTRLOIITPATREADER_H */
