/******************************************************************************
 *   Copyright (C) 2022 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2022-2025 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "R3BFrsSciTcalData.h"

R3BFrsSciTcalData::R3BFrsSciTcalData(UShort_t detector, UShort_t pmt, Double_t tns, UInt_t tc)
    : fDetector(detector)
    , fPmt(pmt)
    , fRawTimeNs(tns)
    , fTimeCoarse(tc)
{
}
ClassImp(R3BFrsSciTcalData)
