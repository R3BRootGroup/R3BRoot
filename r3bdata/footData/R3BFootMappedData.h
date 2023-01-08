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

// -----------------------------------------------------------------
// -----                   R3BFootMappedData                   -----
// -----          Created 16/07/2021 by J.L. Rodriguez         -----
// -----------------------------------------------------------------

#ifndef R3BFootMappedData_H
#define R3BFootMappedData_H

#include "TObject.h"

class R3BFootMappedData : public TObject
{
  public:
    // Default Constructor
    R3BFootMappedData();

    /** Standard Constructor
     *@param detId       Detector unique identifier
     *@param stripId     Strip unique identifier
     *@param energy      Total energy deposited in the strip
     **/
    R3BFootMappedData(Int_t detId, Int_t stripId, Int_t energy);

    // Destructor
    virtual ~R3BFootMappedData() {}

    // Getters
    inline const Int_t& GetDetId() const { return fDetId; }
    inline const Int_t& GetStripId() const { return fStripId; }
    inline const Int_t& GetEnergy() const { return fEnergy; }

  protected:
    Int_t fDetId;   // Detector unique identifiers
    Int_t fStripId; // Strip unique identifiers
    Int_t fEnergy;  // Energy per strip

  public:
    ClassDef(R3BFootMappedData, 1)
};

#endif /* R3BFootMappedData */
