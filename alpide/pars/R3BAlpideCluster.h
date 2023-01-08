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
// -----                   R3BAlpideCluster                    -----
// -----          Created 28/01/2022 by J.L. Rodriguez         -----
// -----------------------------------------------------------------

#ifndef R3BAlpideCluster_H
#define R3BAlpideCluster_H 1

#include "TObject.h"
#include <stdint.h>

class R3BAlpideCluster : public TObject
{
  public:
    // Default Constructor
    R3BAlpideCluster();

    // Standard Constructors
    R3BAlpideCluster(UShort_t sensorid, Int_t col, Int_t row, Bool_t active = true);

    R3BAlpideCluster(UShort_t sensorid, Int_t clusterid, Int_t col, Int_t row);

    // Destructor
    virtual ~R3BAlpideCluster() {}

    // Getters
    const UShort_t& GetSensorId() const { return fSensorId; }
    const Int_t& GetClusterId() const { return fClusterId; }
    const Int_t& GetCol() const { return fCol; }
    const Int_t& GetRow() const { return fRow; }
    const Bool_t& GetAct() const { return fActive; }

  protected:
    UShort_t fSensorId;
    Int_t fClusterId;
    Int_t fCol;
    Int_t fRow;
    Bool_t fActive;

  public:
    ClassDef(R3BAlpideCluster, 1)
};

#endif /* R3BAlpideCluster */
