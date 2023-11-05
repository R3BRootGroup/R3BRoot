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

#include "R3BTDCCyclicCorrector.h"
#include "R3BLogger.h"

#include "TMath.h"
#define IS_NAN(x) TMath::IsNaN(x)

// Standard constructur
double R3BTDCCyclicCorrector::GetVFTXTime(double time)
{
    range = rangeVftx;
    R3BLOG(debug, "Time: " << time << " , range1(VFTX): " << range;);
    return fmod(time + 4 * range + range / 2., range) - range / 2.;
}

double R3BTDCCyclicCorrector::GetTAMEXTime(double time)
{
    range = rangeTamex;
    R3BLOG(debug, "Time: " << time << " , range1(TAMEX): " << range;);
    return fmod(time + 4 * range + range / 2., range) - range / 2.;
}

double R3BTDCCyclicCorrector::GetTRBTime(double time)
{
    range = rangeTrb;
    R3BLOG(debug, "Time: " << time << " , range1(TRB): " << range;);
    return fmod(time + 4 * range + range / 2., range) - range / 2.;
}

double R3BTDCCyclicCorrector::GetKilomTime(double time)
{
    range = rangeKilom;
    R3BLOG(debug, "Time: " << time << " , range1(Kilom): " << range;);
    return fmod(time + 4 * range + range / 2., range) - range / 2.;
}

double R3BTDCCyclicCorrector::GetClockTDCTime(double time)
{
    range = rangeClockTDC;
    R3BLOG(debug, "Time: " << time << " , range1(ClockTDC): " << range;);
    return fmod(time + 4 * range + range / 2., range) - range / 2.;
}

double R3BTDCCyclicCorrector::GetClockTDColdTime(double time)
{
    range = rangeClockTDC150;
    R3BLOG(debug, "Time: " << time << " , range1(ClockTDC - old FW): " << range;);
    return fmod(time + 4 * range + range / 2., range) - range / 2.;
}

ClassImp(R3BTDCCyclicCorrector);
