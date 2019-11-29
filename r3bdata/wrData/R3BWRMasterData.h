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
// -----                      R3BWRMasterData                            -----
// -----                  Created 28/02/2019 by J.L. Rodriguez           -----
// -----                                                                 -----
// ---------------------------------------------------------------------------

#ifndef R3BWRMasterData_H
#define R3BWRMasterData_H
#include "TObject.h"

class R3BWRMasterData : public TObject
{

  public:
    // Default Constructor
    R3BWRMasterData();

    /** Standard Constructor
     *@param timestamp
     **/
    R3BWRMasterData(uint64_t timestamp);

    // Destructor
    ~R3BWRMasterData() {}

    // Getters
    inline const uint64_t GetTimeStamp() const { return fTimeStamp; }

  protected:
    uint64_t fTimeStamp; // timestamp for master

  public:
    ClassDef(R3BWRMasterData, 1)
};

#endif
