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

#ifndef R3BLOSHITITEM_H
#define R3BLOSHITITEM_H

#include "TObject.h"

// for the data analysis of the LOS detectors.
// Introduced by Ralf, FEB 2016

class R3BLosHitData : public TObject
{
  public:
    // Default Constructor
    R3BLosHitData();

    // Standard Constructor
    R3BLosHitData(UChar_t detector, Double_t t, Double_t x, Double_t y, Double_t Z);

    // Destructor
    virtual ~R3BLosHitData() {}

    // Getters
    inline const UChar_t& GetDetector() const { return fDetector; }

  private:
    UChar_t fDetector; // 1..n
                       //    Double_t fTimeDiff2Ref_ns; // difference to master trigger time

  public:
    Double_t fTime_ns; // average time of the scintillators
    Double_t fX_cm;    // position in cm
    Double_t fY_cm;
    Double_t fZ; // Charge

    ClassDef(R3BLosHitData, 1)
};

#endif
