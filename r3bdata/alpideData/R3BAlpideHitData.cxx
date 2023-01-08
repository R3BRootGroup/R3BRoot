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

// -------------------------------------------------------------------
// -----            R3BAlpideHitData source file                 -----
// -------------------------------------------------------------------

#include "R3BAlpideHitData.h"

R3BAlpideHitData::R3BAlpideHitData()
    : fTrack(0., 0., 0.)
    , fSensorId(0)
    , fClustersize(0)
    , fTheta(NAN)
    , fPhi(NAN)
    , fX(NAN)
    , fY(NAN)
    , fZ(NAN)
{
}

R3BAlpideHitData::R3BAlpideHitData(UInt_t sensorId, UInt_t clustersize, Double_t x, Double_t y, Double_t z)
    : fSensorId(sensorId)
    , fClustersize(clustersize)
    , fX(x)
    , fY(y)
    , fZ(z)
{
    fTrack.SetXYZ(x, y, z);
    fTheta = fTrack.Theta();
    fPhi = fTrack.Phi();
}

ClassImp(R3BAlpideHitData);
