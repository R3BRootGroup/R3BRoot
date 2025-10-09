/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2025 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// ------------------------------------------------------------------------
// -----                      R3BWRData                               -----
// -----          Created 28/02/2019 by J.L. Rodriguez                -----
// ------------------------------------------------------------------------

#pragma once

#include <Rtypes.h>
#include <TObject.h>

class R3BWRData : public TObject
{
  public:
    // Default Constructor
    R3BWRData() = default;

    /** Standard Constructor
     *@param timestamp
     *@param id
     **/
    explicit R3BWRData(uint64_t timestamp, UInt_t id = 0);

    // Destructor
    virtual ~R3BWRData() = default;

    // Getters
    inline uint64_t GetTimeStamp() const { return fTimeStamp; }
    inline UInt_t GetId() const { return fId; }

  protected:
    UInt_t fId = 1;
    uint64_t fTimeStamp = 0; // Timestamp

  public:
    ClassDef(R3BWRData, 1); // NOLINT
};
