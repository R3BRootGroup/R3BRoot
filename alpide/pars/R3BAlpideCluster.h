/******************************************************************************
 *   Copyright (C) 2022 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2022-2024 Members of R3B Collaboration                     *
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

#pragma once

#include <TObject.h>
#include <stdint.h>

class R3BAlpideCluster : public TObject
{
  public:
    // Default Constructor
    R3BAlpideCluster() = default;

    // Standard Constructors
    R3BAlpideCluster(UShort_t sensorid, int col, int row, bool active = true);

    R3BAlpideCluster(UShort_t sensorid, int clusterid, int col, int row);

    // Destructor
    virtual ~R3BAlpideCluster() = default;

    // Getters
    UShort_t GetSensorId() const { return fSensorId; }
    Int_t GetClusterId() const { return fClusterId; }
    Int_t GetCol() const { return fCol; }
    Int_t GetRow() const { return fRow; }
    Bool_t GetAct() const { return fActive; }

  protected:
    UShort_t fSensorId = 0;
    int fClusterId = 0;
    int fCol = 0;
    int fRow = 0;
    bool fActive = false;

  public:
    ClassDefOverride(R3BAlpideCluster, 1)
};
