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

// -----------------------------------------------------------------
// -----                   R3BAlpideMappedData                 -----
// -----          Created 11/10/2021 by J.L. Rodriguez         -----
// -----------------------------------------------------------------

#ifndef R3BAlpideMappedData_H
#define R3BAlpideMappedData_H

#include "TObject.h"

class R3BAlpideMappedData : public TObject
{
  public:
    // Default Constructor
    R3BAlpideMappedData();

    /** Standard Constructor
     *@param detId    Detector unique identifier
     *@param chip     Chip identifier
     *@param reg      Region identifier
     *@param col      Column identifier
     *@param ads      Address identifier
     **/
    R3BAlpideMappedData(Int_t detId, Int_t chip, Int_t reg, Int_t col, Int_t ads);

    // Destructor
    virtual ~R3BAlpideMappedData() {}

    // Getters
    inline const Int_t& GetDetId() const { return fDetId; }
    inline const Int_t& GetChip() const { return fChip; }
    inline const Int_t& GetReg() const { return fReg; }
    inline const Int_t& GetCol() const { return fCol; }
    inline const Int_t& GetAds() const { return fAds; }

  protected:
    Int_t fDetId; // Detector unique identifier
    Int_t fChip;  // Chip identifier
    Int_t fReg;   // Region identifier from 1 to 32
    Int_t fCol;   // Column identifier from 1 to 16
    Int_t fAds;   // Address identifier from 1 to 1024

  public:
    ClassDef(R3BAlpideMappedData, 0)
};

#endif /* R3BAlpideMappedData */
