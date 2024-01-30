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

// ----------------------------------------------------------------------
// -----           R3BFootHitData source file                       -----
// ----------------------------------------------------------------------

#include "R3BFootHitData.h"

// -----   Default constructor   ----------------------------------------
R3BFootHitData::R3BFootHitData()
    : FairMultiLinkedData()
    , fDetId(-1)
    , fNbHit(0)
    , fMulStrip(0)
    , fPos(0)
    , fEta(0)
    , fTheta(0)
    , fPhi(0)
    , fmaster(0, 0, 0)
    , fEnergy(0)
{
}

// -----   Standard constructor   ---------------------------------------
R3BFootHitData::R3BFootHitData(Int_t detid, Int_t nbhit, Double_t pos, Double_t eta, TVector3 master, Double_t energy, Int_t mulstrip)
    : FairMultiLinkedData()
    , fDetId(detid)
    , fNbHit(nbhit)
    , fMulStrip(mulstrip)
    , fPos(pos)
    , fEta(eta)
    , fTheta(master.Theta())
    , fPhi(master.Phi())
    , fmaster(master)
    , fEnergy(energy)
{
}

// ----------------------------------------------------------------------
R3BFootHitData::R3BFootHitData(const R3BFootHitData& right)
    : FairMultiLinkedData(right)
    , fDetId(right.fDetId)
    , fNbHit(right.fNbHit)
    , fMulStrip(right.fMulStrip)
    , fPos(right.fPos)
    , fEta(right.fEta)
    , fTheta(right.fmaster.Theta())
    , fPhi(right.fmaster.Phi())
    , fmaster(right.fmaster)
    , fEnergy(right.fEnergy)
{
}

ClassImp(R3BFootHitData);
