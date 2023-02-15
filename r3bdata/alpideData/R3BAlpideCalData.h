/******************************************************************************
 *   Copyright (C) 2022 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2022-2023 Members of R3B Collaboration                     *
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
// -----                   R3BAlpideCalData                    -----
// -----          Created 28/01/2022 by J.L. Rodriguez         -----
// -----------------------------------------------------------------

#ifndef R3BAlpideCalData_H
#define R3BAlpideCalData_H 1

#include "TObject.h"
#include <stdint.h>

class R3BAlpideCalData : public TObject
{
  public:
    // Default Constructor
    R3BAlpideCalData();

    /** Standard Constructor
     *@param senId    Sensor unique identifier
     *@param col    Longitudinal position
     *@param row    Transverse position
     **/
    R3BAlpideCalData(UShort_t senId, Int_t col, Int_t row);

    // Destructor
    virtual ~R3BAlpideCalData() {}

    // Getters
    inline UShort_t GetSensorId() const { return fSenId; }
    inline Int_t GetCol() const { return fCol; }
    inline Int_t GetRow() const { return fRow; }

  protected:
    UShort_t fSenId;
    Int_t fCol;
    Int_t fRow;

  public:
    ClassDef(R3BAlpideCalData, 1)
};

#endif /* R3BAlpideCalData */
