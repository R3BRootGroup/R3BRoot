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

#include "R3BMusliCalData.h"

// -----   Default constructor   -------------------------------------------
R3BMusliCalData::R3BMusliCalData()
    : fNbAnodes(0)
    , fIndex(0)
    , fSignal(0)
    , fDT(-1.)
    , fE(-1.)
{
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
R3BMusliCalData::R3BMusliCalData(UInt_t n, UInt_t index, UInt_t signal, Double_t drifttime, Double_t energy)
    : fNbAnodes(n)
    , fIndex(index)
    , fSignal(index)
    , fDT(drifttime)
    , fE(energy)
{
}
// -------------------------------------------------------------------------

ClassImp(R3BMusliCalData);
