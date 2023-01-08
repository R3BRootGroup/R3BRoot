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

// ---------------------------------------------------------------------------------------
// -----                               R3BMfiUnpack                                  -----
// -----                      Created 30-07-2014 by D.Kresan                         -----
// ---------------------------------------------------------------------------------------

#ifndef R3BMFIUNPACK_H
#define R3BMFIUNPACK_H

#include "FairUnpack.h"

class TClonesArray;

class R3BMfiUnpack : public FairUnpack
{
  public:
    // Constructor
    R3BMfiUnpack(Short_t type = 97,
                 Short_t subType = 9700,
                 Short_t procId = 1,
                 Short_t subCrate = 0,
                 Short_t control = 9);

    // Destructor
    virtual ~R3BMfiUnpack();

    // Fair specific
    virtual Bool_t Init();
    virtual Bool_t DoUnpack(Int_t* data, Int_t size);
    virtual void Reset();

  protected:
    virtual void Register();

  private:
    TClonesArray* fRawData;
    Int_t fNHits;

  public:
    // Class definition
    ClassDef(R3BMfiUnpack, 0)
};

#endif
