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

// ---------------------------------------------------------------------------
// -----                                                                 -----
// -----                      R3BWRPspData                               -----
// -----                  Created 28/06/2021 by J.L. Rodriguez           -----
// -----                                                                 -----
// ---------------------------------------------------------------------------

#ifndef R3BWRPspData_H
#define R3BWRPspData_H
#include "TObject.h"

class R3BWRPspData : public TObject
{

  public:
    // Default Constructor
    R3BWRPspData();

    /** Standard Constructor
     *@param timestamp
     **/
    R3BWRPspData(uint64_t timestamp);

    // Destructor
    virtual ~R3BWRPspData() {}

    // Getters
    inline const uint64_t GetTimeStamp() const { return fTimeStamp; }

  protected:
    uint64_t fTimeStamp; // timestamp for master

  public:
    ClassDef(R3BWRPspData, 1)
};

#endif
