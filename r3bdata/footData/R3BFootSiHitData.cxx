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

// ----------------------------------------------------------------------
// -----           R3BFootSiHitData source file                     -----
// ----------------------------------------------------------------------

#include "R3BFootSiHitData.h"

// -----   Default constructor   ----------------------------------------
R3BFootSiHitData::R3BFootSiHitData()
    : FairMultiLinkedData()
    , fDetId(-1)
    , fNbHit(0)
    , fMulStrip(0)
    , fX(0)
    , fY(0)
    , fTheta(0)
    , fPhi(0)
    , fmaster(0, 0, 0)
    , fEnergy(0)
{
}

// -----   Standard constructor   ---------------------------------------
R3BFootSiHitData::R3BFootSiHitData(Int_t detid,
                                   Int_t nbhit,
                                   Double_t x,
                                   Double_t y,
                                   TVector3 master,
                                   Double_t energy,
                                   Int_t mulstrip)
    : FairMultiLinkedData()
    , fDetId(detid)
    , fNbHit(nbhit)
    , fMulStrip(mulstrip)
    , fX(x)
    , fY(y)
    , fTheta(master.Theta())
    , fPhi(master.Phi())
    , fmaster(master)
    , fEnergy(energy)
{
}

// ----------------------------------------------------------------------
R3BFootSiHitData::R3BFootSiHitData(const R3BFootSiHitData& right)
    : FairMultiLinkedData(right)
    , fDetId(right.fDetId)
    , fNbHit(right.fNbHit)
    , fMulStrip(right.fMulStrip)
    , fX(right.fX)
    , fY(right.fY)
    , fTheta(right.fmaster.Theta())
    , fPhi(right.fmaster.Phi())
    , fmaster(right.fmaster)
    , fEnergy(right.fEnergy)
{
}

ClassImp(R3BFootSiHitData);
