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

// -------------------------------------------------------------------------
// -----            R3BAmsHitData source file                          -----
// -------------------------------------------------------------------------

#include "R3BAmsHitData.h"
#include "FairLogger.h"
#include <iostream>

// -----   Default constructor   -------------------------------------------
R3BAmsHitData::R3BAmsHitData()
    : FairMultiLinkedData()
    , fDetId(-1)
    , fNumHit(0)
    , fX(0)
    , fY(0)
    , fTheta(0)
    , fPhi(0)
    , fmaster(0, 0, 0)
    , fEnergyX(0)
    , fEnergyY(0)
{
}

// -----   Standard constructor   ------------------------------------------
R3BAmsHitData::R3BAmsHitData(Int_t detid,
                             Int_t numhit,
                             Double_t x,
                             Double_t y,
                             TVector3 master,
                             Double_t energy_x,
                             Double_t energy_y)
    : FairMultiLinkedData()
    , fDetId(detid)
    , fNumHit(numhit)
    , fX(x)
    , fY(y)
    , fTheta(master.Theta())
    , fPhi(master.Phi())
    , fmaster(master)
    , fEnergyX(energy_x)
    , fEnergyY(energy_y)
{
}
// -------------------------------------------------------------------------

R3BAmsHitData::R3BAmsHitData(const R3BAmsHitData& right)
    : FairMultiLinkedData(right)
    , fDetId(right.fDetId)
    , fNumHit(right.fNumHit)
    , fX(right.fX)
    , fY(right.fY)
    , fTheta(right.fmaster.Theta())
    , fPhi(right.fmaster.Phi())
    , fmaster(right.fmaster)
    , fEnergyX(right.fEnergyX)
    , fEnergyY(right.fEnergyY)
{
}

// -----   Destructor   ----------------------------------------------------
R3BAmsHitData::~R3BAmsHitData() {}

// -----   Public method Print   -------------------------------------------
void R3BAmsHitData::Print(const Option_t* opt) const
{
    LOG(INFO) << "-I- R3BAmsHitData: a HitData in detector " << fDetId << " number of hit " << fNumHit
              << " position [X,Y] = " << fX << ", " << fY;
    LOG(INFO) << " Energy [X,Y] = " << fEnergyX << " , " << fEnergyY << " (GeV in sim)";
}
// -------------------------------------------------------------------------
