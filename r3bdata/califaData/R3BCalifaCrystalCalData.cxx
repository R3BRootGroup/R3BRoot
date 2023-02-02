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

#include "R3BCalifaCrystalCalData.h"


R3BCalifaCrystalCalData::R3BCalifaCrystalCalData(Int_t ident,
                                                 Double_t energy,
                                                 Double_t Nf,
                                                 Double_t Ns,
                                                 uint64_t time,
                                                 Double_t tot_energy)
    : fEnergy(energy)
    , fNf(Nf)
    , fNs(Ns)
    , fToTEnergy(tot_energy)
    , fWrts(time)
    , fCrystalId(ident)
{
}


ClassImp(R3BCalifaCrystalCalData);
