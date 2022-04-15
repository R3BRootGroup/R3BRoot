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

#include "R3BTimeStitch.h"
#include "R3BLogger.h"
#include "TMath.h"
#define IS_NAN(x) TMath::IsNaN(x)

R3BTimeStitch::R3BTimeStitch()
    : fRange(2048 * 5) // ns
{
}

Double_t R3BTimeStitch::GetTime(Double_t time) const
{
 // R3BLOG_IF(ERROR, IS_NAN(time), "Providing NaN time!");
 R3BLOG(DEBUG, "Time: " << time << " , range: " << fRange);
 return fmod(time + fRange + fRange / 2., fRange) - fRange / 2.;
}

ClassImp(R3BTimeStitch);
