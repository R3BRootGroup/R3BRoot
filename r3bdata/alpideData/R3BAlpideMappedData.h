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
// -----                   R3BAlpideMappedData                 -----
// -----          Created 11/10/2021 by J.L. Rodriguez         -----
// -----------------------------------------------------------------

#ifndef R3BAlpideMappedData_H
#define R3BAlpideMappedData_H 1

#include "TObject.h"
#include <stdint.h>

class R3BAlpideMappedData : public TObject
{
  public:
    // Default Constructor
    R3BAlpideMappedData();

    /** Standard Constructor
     *@param senId    Sensor unique identifier
     *@param reg      Region identifier
     *@param ads      Address identifier
     *@param chip     Chip identifier
     *@param row      Row identifier
     *@param col      Column identifier
     **/
    R3BAlpideMappedData(UShort_t senId, UShort_t reg, UShort_t ads, UShort_t chip, UShort_t row, UShort_t col);

    // Destructor
    virtual ~R3BAlpideMappedData() {}

    // Getters
    inline const UShort_t GetSensorId() const { return fSenId; }
    inline const UShort_t GetReg() const { return fReg; }
    inline const UShort_t GetAds() const { return fAds; }
    inline const UShort_t GetChip() const { return fChip; }
    inline const UShort_t GetRow() const { return fRow; }
    inline const UShort_t GetCol() const { return fCol; }

  protected:
    UShort_t fSenId; // Sensor unique identifier
    UShort_t fReg;   // Region identifier from 1 to 32
    UShort_t fAds;   // Address identifier from 1 to 1024
    UShort_t fChip;  // Chip identifier
    UShort_t fRow;   // Row identifier from 1 to 512
    UShort_t fCol;   // Column identifier from 1 to 1024

  public:
    ClassDef(R3BAlpideMappedData, 2)
};

#endif /* R3BAlpideMappedData */
