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

#ifndef R3BROLUHITITEM_H
#define R3BROLUHITITEM_H

#include "TObject.h"

// for the data analysis of the ROLU following LOS

class R3BRoluHitData : public TObject
{
  public:
    // Default Constructor
    R3BRoluHitData();

    // Standard Constructor
    R3BRoluHitData(UChar_t detector, Double_t tot_r, Double_t tot_o, Double_t tot_l, Double_t tot_u);

    // Destructor
    virtual ~R3BRoluHitData() {}

    // Getters
    inline const UChar_t& GetDetector() const { return fDetector; }

  private:
    UChar_t fDetector; // 1..n

  public:
    Double_t ftot_r;
    Double_t ftot_o;
    Double_t ftot_l;
    Double_t ftot_u;

    ClassDef(R3BRoluHitData, 1)
};

#endif
