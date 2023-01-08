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

// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                           R3BStartrackUnpack                            -----
// -----                           Version 0.1                             -----
// -----                    Created 28.05.2014 by M.Labiche               -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#ifndef R3BCSTARTRACKUNPACK_H
#define R3BSTARTRACKUNPACK_H

#include "FairUnpack.h"

class TClonesArray;

// static const UInt_t gosip_header_size = 16;
// static const UInt_t gosip_sub_header_size = 8;
// static const Int_t  event_t_size = 44;
// static const Int_t  trace_head_t_size = 8;

class R3BStartrackUnpack : public FairUnpack
{
  public:
    // Constructor
    R3BStartrackUnpack(char* strCalDir,
                       Short_t type = 10,
                       Short_t subType = 1,
                       Short_t procId = 1,
                       Short_t subCrate = 0,
                       Short_t control = 37);

    // Destructor
    virtual ~R3BStartrackUnpack();

    // Fair specific
    virtual Bool_t Init();
    virtual Bool_t DoUnpack(Int_t* data, Int_t size);
    virtual Bool_t DoUnpack2(Int_t* data_wd0, Int_t* data_wd1, Int_t size);
    virtual void Reset();

  protected:
    virtual void Register();

  private:
    TClonesArray* fRawData;
    Int_t fNHits;

    // The information is split into 2 words of 32 bits (4 byte).
    // The words can by of two types: A or B.
    // Type A words have word_0 with msb=10.
    // Type B words have word_0 with msb=11.
    UInt_t word_0A;
    UInt_t word_1A;
    UInt_t word_0B;
    UInt_t word_1B;

  public:
    // Class definition
    ClassDef(R3BStartrackUnpack, 0)
};

#endif
