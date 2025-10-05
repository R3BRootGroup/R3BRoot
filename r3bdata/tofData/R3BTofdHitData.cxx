/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2025 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "R3BTofdHitData.h"

// for the data analysis of the Tofd detector.
// Introduced by M.Heil, May 2016
// Modified by Pablo González Rusell, Oct 2025

R3BTofdHitData::R3BTofdHitData(Double_t t,
                               Double_t x,
                               Double_t y,
                               Double_t Z,
                               Double_t tdiff,
                               Double_t ELoss,
                               UInt_t ID,
                               UInt_t iBar,
                               Double_t traw,
                               Double_t tof)
    : fTime(t)
    , fXPos(x)
    , fYPos(y)
    , fZCharge(Z)
    , fTimeDiff(tdiff)
    , fELoss(ELoss)
    , fPlaneId(ID)
    , fBarId(iBar)
    , fTimeRaw(traw)
    , fTof(tof)
{
}

ClassImp(R3BTofdHitData)
