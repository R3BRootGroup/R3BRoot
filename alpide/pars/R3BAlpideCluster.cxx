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
// -----            R3BAlpideCluster source file               -----
// -----------------------------------------------------------------

#include "R3BAlpideCluster.h"

R3BAlpideCluster::R3BAlpideCluster()
    : fSensorId(0)
    , fClusterId(0)
    , fCol(0)
    , fRow(0)
    , fActive(false)
{
}

R3BAlpideCluster::R3BAlpideCluster(UShort_t sensorid, Int_t col, Int_t row, Bool_t active)
    : fSensorId(sensorid)
    , fCol(col)
    , fRow(row)
    , fActive(active)
{
}

R3BAlpideCluster::R3BAlpideCluster(UShort_t sensorid, Int_t clusterid, Int_t col, Int_t row)
    : fSensorId(sensorid)
    , fClusterId(clusterid)
    , fCol(col)
    , fRow(row)
{
}

ClassImp(R3BAlpideCluster);
