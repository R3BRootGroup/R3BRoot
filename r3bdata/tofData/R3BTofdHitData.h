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

#ifndef R3BTOFDHITDATA_H
#define R3BTOFDHITDATA_H

#include "R3BHit.h"

// for the data analysis of the Tofd detectors.
// Introduced by M.Heil, May 2016

class R3BTofdHitData : public R3BHit
{
  public:
    // Default Constructor
    R3BTofdHitData();

    // Standard Constructor
    R3BTofdHitData(Double_t t,
                   Double_t x,
                   Double_t y,
                   Double_t Z,
                   Double_t tdiff,
                   Double_t ELoss = 0,
                   Double_t ID = 0,
                   Int_t iBar = 0,
                   Double_t traw = -1000);

    // Destructor
    virtual ~R3BTofdHitData() {}

    Int_t GetBarId() const;
    Double_t GetTimeRaw() const; // Get paddle time without trigger

  private:
    Int_t fBarId;
    Double_t fTimeRaw;

    ClassDef(R3BTofdHitData, 2)
};

#endif
