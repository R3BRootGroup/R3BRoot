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
// -----            R3BAlpideCluster source file               -----
// -----------------------------------------------------------------

#include "R3BAlpideCluster.h"

R3BAlpideCluster::R3BAlpideCluster(UShort_t sensorid, int col, int row, bool active)
    : fSensorId(sensorid)
    , fCol(col)
    , fRow(row)
    , fActive(active)
{
}

R3BAlpideCluster::R3BAlpideCluster(UShort_t sensorid, int clusterid, int col, int row)
    : fSensorId(sensorid)
    , fClusterId(clusterid)
    , fCol(col)
    , fRow(row)
{
}

ClassImp(R3BAlpideCluster)
