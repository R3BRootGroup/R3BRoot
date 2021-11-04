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

#ifndef R3BWhiterabbitMusicReader_H
#define R3BWhiterabbitMusicReader_H

#include "R3BReader.h"
#include <Rtypes.h>

struct EXT_STR_h101_WRMUSIC_t;
typedef struct EXT_STR_h101_WRMUSIC_t EXT_STR_h101_WRMUSIC;

class TClonesArray;
class R3BEventHeader;

/**
 * A reader of MUSIC white rabbit data with UCESB.
 * Receives mapped raw data and converts it to R3BRoot objects.
 * @author J.L. Rodriguez, H.T. Toernqvist
 * @since Jul 5, 2021
 */

class R3BWhiterabbitMusicReader : public R3BReader
{
  public:
    // Standard constructor
    R3BWhiterabbitMusicReader(EXT_STR_h101_WRMUSIC*, size_t, UInt_t);

    // Destructor
    virtual ~R3BWhiterabbitMusicReader();

    // Setup structure information
    virtual Bool_t Init(ext_data_struct_info*) override;

    // Read data from full event structure
    virtual Bool_t Read() override;

    // Reset
    virtual void Reset() override;

    // Accessor to select online mode
    void SetOnline(Bool_t option) { fOnline = option; }

  private:
    // An event counter
    UInt_t fNEvent;
    // Reader specific data structure from ucesb
    EXT_STR_h101_WRMUSIC* fData;
    // Offset of detector specific data in full data structure
    size_t fOffset;
    // The whiterabbit subsystem ID
    UInt_t fWhiterabbitId;
    // A pointer to the R3BEventHeader structure
    R3BEventHeader* fEventHeader;
    // Don't store data for online
    Bool_t fOnline;
    // Output array
    TClonesArray* fArray;

  public:
    ClassDefOverride(R3BWhiterabbitMusicReader, 0);
};
#endif // R3BWhiterabbitMusicReader_H
