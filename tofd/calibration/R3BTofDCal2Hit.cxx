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

// ------------------------------------------------------------
// -----                 R3BTofDCal2Hit                   -----
// -----    Created 18/03/22 by J.L. Rodriguez-Sanchez    -----
// ------------------------------------------------------------

#include "R3BTofDCal2Hit.h"
#include "R3BEventHeader.h"
#include "R3BLogger.h"
#include "R3BTCalEngine.h"
#include "R3BTofDHitModulePar.h"
#include "R3BTofDHitPar.h"
#include "R3BTofDMappingPar.h"
#include "R3BTofdCalData.h"
#include "R3BTofdHitData.h"

#include "FairLogger.h"
#include "FairRuntimeDb.h"
#include "TH1F.h"
#include "TH2F.h"
#include "THnSparse.h"

#include "TClonesArray.h"
#include "TMath.h"

#include <iostream>
#include <numeric>
#include <sstream>
#include <string>

using namespace std;
#define IS_NAN(x) TMath::IsNaN(x)

namespace
{
    double c_range_ns = 2048 * 5;
    double c_bar_coincidence_ns = 20; // nanoseconds.
    uint64_t n1, n2;
} // namespace

R3BTofDCal2Hit::R3BTofDCal2Hit()
    : R3BTofDCal2Hit("TofdCal2Hit", 1)
{
}

R3BTofDCal2Hit::R3BTofDCal2Hit(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fCalItems(NULL)
    , fCalTriggerItems(NULL)
    , fHitItems(new TClonesArray("R3BTofdHitData"))
    , fNofHitItems(0)
    , fNofHitPars(0)
    , fHitPar(NULL)
    , fMapPar(NULL)
    , fTrigger(-1)
    , fTpat1(-1)
    , fTpat2(-1)
    , fNofPlanes(5)
    , fPaddlesPerPlane(44)
    , fTofdQ(1)
    , fTofdHisto(true)
    , fTofdTotPos(true)
    , fnEvents(0)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
    , maxevent(0)
    , wrongtrigger(0)
    , wrongtpat(0)
    , notpat(0)
    , headertpat(0)
    , events_in_cal_level(0)
    , inbarcoincidence(0)
    , eventstore(0)
    , singlehit(0)
    , multihit(0)
    , bars_with_multihit(0)
    , events_wo_tofd_hits(0)
    , lasttpatevent(0)
    , goodpair(0)
    , goodpair4(0)
    , goodpair3(0)
    , goodpair1(0)
    , goodpair2(0)
    , goodpair5(0)
    , goodpair6(0)
    , goodpair7(0)
    , fOnline(kFALSE)
{
    if (fTofdHisto)
    {
        //  fhTpat = NULL;
        fhNoTpat = NULL;
        for (Int_t i = 0; i < N_TOFD_HIT_PLANE_MAX; i++)
        {
            fhQ[i] = NULL;
            fhxy[i] = NULL;
            fhQvsEvent[i] = NULL;
            fhTdiff[i] = NULL;
            fhTsync[i] = NULL;
            fhQ0Qt[i] = NULL;
            fhTvsQ[i] = NULL;
            for (Int_t j = 0; j < N_TOFD_HIT_PADDLE_MAX; j++)
            {
                fhQvsPos[i][j] = NULL;
                // fhQvsTHit[i][j] = NULL;
                // fhTvsTHit[i][j] = NULL;
            }
        }
    }

    /*
        g_tofd_trig_map[0][0][0] = 0;
        g_tofd_trig_map[0][0][0] = 0;
        g_tofd_trig_map[0][0][1] = 0;
        g_tofd_trig_map[0][0][1] = 0;
        g_tofd_trig_map[0][0][2] = 0;
        g_tofd_trig_map[0][0][2] = 0;
        g_tofd_trig_map[0][0][3] = 0;
        g_tofd_trig_map[0][0][3] = 0;
        g_tofd_trig_map[0][0][4] = 0;
        g_tofd_trig_map[0][0][4] = 0;
        g_tofd_trig_map[0][0][5] = 0;
        g_tofd_trig_map[0][0][5] = 0;
        g_tofd_trig_map[0][0][6] = 0;
        g_tofd_trig_map[0][0][6] = 0;
        g_tofd_trig_map[0][0][7] = 0;
        g_tofd_trig_map[0][0][7] = 0;
        g_tofd_trig_map[0][0][8] = 0;
        g_tofd_trig_map[0][0][8] = 0;
        g_tofd_trig_map[0][0][9] = 0;
        g_tofd_trig_map[0][0][9] = 0;
        g_tofd_trig_map[0][0][10] = 0;
        g_tofd_trig_map[0][0][10] = 0;
        g_tofd_trig_map[0][0][11] = 0;
        g_tofd_trig_map[0][0][11] = 0;
        g_tofd_trig_map[0][0][12] = 0;
        g_tofd_trig_map[0][0][12] = 0;
        g_tofd_trig_map[0][0][13] = 0;
        g_tofd_trig_map[0][0][13] = 0;
        g_tofd_trig_map[0][0][14] = 0;
        g_tofd_trig_map[0][0][14] = 0;
        g_tofd_trig_map[0][0][15] = 0;
        g_tofd_trig_map[0][0][15] = 0;
        g_tofd_trig_map[0][0][16] = 1;
        g_tofd_trig_map[0][0][16] = 1;
        g_tofd_trig_map[0][0][17] = 1;
        g_tofd_trig_map[0][0][17] = 1;
        g_tofd_trig_map[0][0][18] = 1;
        g_tofd_trig_map[0][0][18] = 1;
        g_tofd_trig_map[0][0][19] = 1;
        g_tofd_trig_map[0][0][19] = 1;
        g_tofd_trig_map[0][0][20] = 1;
        g_tofd_trig_map[0][0][20] = 1;
        g_tofd_trig_map[0][0][21] = 1;
        g_tofd_trig_map[0][0][21] = 1;
        g_tofd_trig_map[0][0][22] = 1;
        g_tofd_trig_map[0][0][22] = 1;
        g_tofd_trig_map[0][0][23] = 1;
        g_tofd_trig_map[0][0][23] = 1;
        g_tofd_trig_map[0][0][24] = 1;
        g_tofd_trig_map[0][0][24] = 1;
        g_tofd_trig_map[0][0][25] = 1;
        g_tofd_trig_map[0][0][25] = 1;
        g_tofd_trig_map[0][0][26] = 1;
        g_tofd_trig_map[0][0][26] = 1;
        g_tofd_trig_map[0][0][27] = 1;
        g_tofd_trig_map[0][0][27] = 1;
        g_tofd_trig_map[0][0][28] = 1;
        g_tofd_trig_map[0][0][28] = 1;
        g_tofd_trig_map[0][0][29] = 1;
        g_tofd_trig_map[0][0][29] = 1;
        g_tofd_trig_map[0][0][30] = 1;
        g_tofd_trig_map[0][0][30] = 1;
        g_tofd_trig_map[0][0][31] = 1;
        g_tofd_trig_map[0][0][31] = 1;
        g_tofd_trig_map[0][0][32] = 2;
        g_tofd_trig_map[0][0][32] = 2;
        g_tofd_trig_map[0][0][33] = 2;
        g_tofd_trig_map[0][0][33] = 2;
        g_tofd_trig_map[0][0][34] = 2;
        g_tofd_trig_map[0][0][34] = 2;
        g_tofd_trig_map[0][0][35] = 2;
        g_tofd_trig_map[0][0][35] = 2;
        g_tofd_trig_map[0][0][36] = 2;
        g_tofd_trig_map[0][0][36] = 2;
        g_tofd_trig_map[0][0][37] = 2;
        g_tofd_trig_map[0][0][37] = 2;
        g_tofd_trig_map[0][0][38] = 2;
        g_tofd_trig_map[0][0][38] = 2;
        g_tofd_trig_map[0][0][39] = 2;
        g_tofd_trig_map[0][0][39] = 2;
        g_tofd_trig_map[0][0][40] = 2;
        g_tofd_trig_map[0][0][40] = 2;
        g_tofd_trig_map[0][0][41] = 2;
        g_tofd_trig_map[0][0][41] = 2;
        g_tofd_trig_map[0][0][42] = 2;
        g_tofd_trig_map[0][0][42] = 2;
        g_tofd_trig_map[0][0][43] = 2;
        g_tofd_trig_map[0][0][43] = 2;
        g_tofd_trig_map[0][1][0] = 3;
        g_tofd_trig_map[0][1][0] = 3;
        g_tofd_trig_map[0][1][1] = 3;
        g_tofd_trig_map[0][1][1] = 3;
        g_tofd_trig_map[0][1][2] = 3;
        g_tofd_trig_map[0][1][2] = 3;
        g_tofd_trig_map[0][1][3] = 3;
        g_tofd_trig_map[0][1][3] = 3;
        g_tofd_trig_map[0][1][4] = 3;
        g_tofd_trig_map[0][1][4] = 3;
        g_tofd_trig_map[0][1][5] = 3;
        g_tofd_trig_map[0][1][5] = 3;
        g_tofd_trig_map[0][1][6] = 3;
        g_tofd_trig_map[0][1][6] = 3;
        g_tofd_trig_map[0][1][7] = 3;
        g_tofd_trig_map[0][1][7] = 3;
        g_tofd_trig_map[0][1][8] = 3;
        g_tofd_trig_map[0][1][8] = 3;
        g_tofd_trig_map[0][1][9] = 3;
        g_tofd_trig_map[0][1][9] = 3;
        g_tofd_trig_map[0][1][10] = 3;
        g_tofd_trig_map[0][1][10] = 3;
        g_tofd_trig_map[0][1][11] = 3;
        g_tofd_trig_map[0][1][11] = 3;
        g_tofd_trig_map[0][1][12] = 3;
        g_tofd_trig_map[0][1][12] = 3;
        g_tofd_trig_map[0][1][13] = 3;
        g_tofd_trig_map[0][1][13] = 3;
        g_tofd_trig_map[0][1][14] = 3;
        g_tofd_trig_map[0][1][14] = 3;
        g_tofd_trig_map[0][1][15] = 3;
        g_tofd_trig_map[0][1][15] = 3;
        g_tofd_trig_map[0][1][16] = 4;
        g_tofd_trig_map[0][1][16] = 4;
        g_tofd_trig_map[0][1][17] = 4;
        g_tofd_trig_map[0][1][17] = 4;
        g_tofd_trig_map[0][1][18] = 4;
        g_tofd_trig_map[0][1][18] = 4;
        g_tofd_trig_map[0][1][19] = 4;
        g_tofd_trig_map[0][1][19] = 4;
        g_tofd_trig_map[0][1][20] = 4;
        g_tofd_trig_map[0][1][20] = 4;
        g_tofd_trig_map[0][1][21] = 4;
        g_tofd_trig_map[0][1][21] = 4;
        g_tofd_trig_map[0][1][22] = 4;
        g_tofd_trig_map[0][1][22] = 4;
        g_tofd_trig_map[0][1][23] = 4;
        g_tofd_trig_map[0][1][23] = 4;
        g_tofd_trig_map[0][1][24] = 4;
        g_tofd_trig_map[0][1][24] = 4;
        g_tofd_trig_map[0][1][25] = 4;
        g_tofd_trig_map[0][1][25] = 4;
        g_tofd_trig_map[0][1][26] = 4;
        g_tofd_trig_map[0][1][26] = 4;
        g_tofd_trig_map[0][1][27] = 4;
        g_tofd_trig_map[0][1][27] = 4;
        g_tofd_trig_map[0][1][28] = 4;
        g_tofd_trig_map[0][1][28] = 4;
        g_tofd_trig_map[0][1][29] = 4;
        g_tofd_trig_map[0][1][29] = 4;
        g_tofd_trig_map[0][1][30] = 4;
        g_tofd_trig_map[0][1][30] = 4;
        g_tofd_trig_map[0][1][31] = 4;
        g_tofd_trig_map[0][1][31] = 4;
        g_tofd_trig_map[0][1][32] = 5;
        g_tofd_trig_map[0][1][32] = 5;
        g_tofd_trig_map[0][1][33] = 5;
        g_tofd_trig_map[0][1][33] = 5;
        g_tofd_trig_map[0][1][34] = 5;
        g_tofd_trig_map[0][1][34] = 5;
        g_tofd_trig_map[0][1][35] = 5;
        g_tofd_trig_map[0][1][35] = 5;
        g_tofd_trig_map[0][1][36] = 5;
        g_tofd_trig_map[0][1][36] = 5;
        g_tofd_trig_map[0][1][37] = 5;
        g_tofd_trig_map[0][1][37] = 5;
        g_tofd_trig_map[0][1][38] = 5;
        g_tofd_trig_map[0][1][38] = 5;
        g_tofd_trig_map[0][1][39] = 5;
        g_tofd_trig_map[0][1][39] = 5;
        g_tofd_trig_map[0][1][40] = 5;
        g_tofd_trig_map[0][1][40] = 5;
        g_tofd_trig_map[0][1][41] = 5;
        g_tofd_trig_map[0][1][41] = 5;
        g_tofd_trig_map[0][1][42] = 5;
        g_tofd_trig_map[0][1][42] = 5;
        g_tofd_trig_map[0][1][43] = 5;
        g_tofd_trig_map[0][1][43] = 5;
        g_tofd_trig_map[1][0][0] = 6;
        g_tofd_trig_map[1][0][0] = 6;
        g_tofd_trig_map[1][0][1] = 6;
        g_tofd_trig_map[1][0][1] = 6;
        g_tofd_trig_map[1][0][2] = 6;
        g_tofd_trig_map[1][0][2] = 6;
        g_tofd_trig_map[1][0][3] = 6;
        g_tofd_trig_map[1][0][3] = 6;
        g_tofd_trig_map[1][0][4] = 6;
        g_tofd_trig_map[1][0][4] = 6;
        g_tofd_trig_map[1][0][5] = 6;
        g_tofd_trig_map[1][0][5] = 6;
        g_tofd_trig_map[1][0][6] = 6;
        g_tofd_trig_map[1][0][6] = 6;
        g_tofd_trig_map[1][0][7] = 6;
        g_tofd_trig_map[1][0][7] = 6;
        g_tofd_trig_map[1][0][8] = 6;
        g_tofd_trig_map[1][0][8] = 6;
        g_tofd_trig_map[1][0][9] = 6;
        g_tofd_trig_map[1][0][9] = 6;
        g_tofd_trig_map[1][0][10] = 6;
        g_tofd_trig_map[1][0][10] = 6;
        g_tofd_trig_map[1][0][11] = 6;
        g_tofd_trig_map[1][0][11] = 6;
        g_tofd_trig_map[1][0][12] = 6;
        g_tofd_trig_map[1][0][12] = 6;
        g_tofd_trig_map[1][0][13] = 6;
        g_tofd_trig_map[1][0][13] = 6;
        g_tofd_trig_map[1][0][14] = 6;
        g_tofd_trig_map[1][0][14] = 6;
        g_tofd_trig_map[1][0][15] = 6;
        g_tofd_trig_map[1][0][15] = 6;
        g_tofd_trig_map[1][0][16] = 7;
        g_tofd_trig_map[1][0][16] = 7;
        g_tofd_trig_map[1][0][17] = 7;
        g_tofd_trig_map[1][0][17] = 7;
        g_tofd_trig_map[1][0][18] = 7;
        g_tofd_trig_map[1][0][18] = 7;
        g_tofd_trig_map[1][0][19] = 7;
        g_tofd_trig_map[1][0][19] = 7;
        g_tofd_trig_map[1][0][20] = 7;
        g_tofd_trig_map[1][0][20] = 7;
        g_tofd_trig_map[1][0][21] = 7;
        g_tofd_trig_map[1][0][21] = 7;
        g_tofd_trig_map[1][0][22] = 7;
        g_tofd_trig_map[1][0][22] = 7;
        g_tofd_trig_map[1][0][23] = 7;
        g_tofd_trig_map[1][0][23] = 7;
        g_tofd_trig_map[1][0][24] = 7;
        g_tofd_trig_map[1][0][24] = 7;
        g_tofd_trig_map[1][0][25] = 7;
        g_tofd_trig_map[1][0][25] = 7;
        g_tofd_trig_map[1][0][26] = 7;
        g_tofd_trig_map[1][0][26] = 7;
        g_tofd_trig_map[1][0][27] = 7;
        g_tofd_trig_map[1][0][27] = 7;
        g_tofd_trig_map[1][0][28] = 7;
        g_tofd_trig_map[1][0][28] = 7;
        g_tofd_trig_map[1][0][29] = 7;
        g_tofd_trig_map[1][0][29] = 7;
        g_tofd_trig_map[1][0][30] = 7;
        g_tofd_trig_map[1][0][30] = 7;
        g_tofd_trig_map[1][0][31] = 7;
        g_tofd_trig_map[1][0][31] = 7;
        g_tofd_trig_map[1][0][32] = 8;
        g_tofd_trig_map[1][0][32] = 8;
        g_tofd_trig_map[1][0][33] = 8;
        g_tofd_trig_map[1][0][33] = 8;
        g_tofd_trig_map[1][0][34] = 8;
        g_tofd_trig_map[1][0][34] = 8;
        g_tofd_trig_map[1][0][35] = 8;
        g_tofd_trig_map[1][0][35] = 8;
        g_tofd_trig_map[1][0][36] = 8;
        g_tofd_trig_map[1][0][36] = 8;
        g_tofd_trig_map[1][0][37] = 8;
        g_tofd_trig_map[1][0][37] = 8;
        g_tofd_trig_map[1][0][38] = 8;
        g_tofd_trig_map[1][0][38] = 8;
        g_tofd_trig_map[1][0][39] = 8;
        g_tofd_trig_map[1][0][39] = 8;
        g_tofd_trig_map[1][0][40] = 8;
        g_tofd_trig_map[1][0][40] = 8;
        g_tofd_trig_map[1][0][41] = 8;
        g_tofd_trig_map[1][0][41] = 8;
        g_tofd_trig_map[1][0][42] = 8;
        g_tofd_trig_map[1][0][42] = 8;
        g_tofd_trig_map[1][0][43] = 8;
        g_tofd_trig_map[1][0][43] = 8;
        g_tofd_trig_map[1][1][0] = 9;
        g_tofd_trig_map[1][1][0] = 9;
        g_tofd_trig_map[1][1][1] = 9;
        g_tofd_trig_map[1][1][1] = 9;
        g_tofd_trig_map[1][1][2] = 9;
        g_tofd_trig_map[1][1][2] = 9;
        g_tofd_trig_map[1][1][3] = 9;
        g_tofd_trig_map[1][1][3] = 9;
        g_tofd_trig_map[1][1][4] = 9;
        g_tofd_trig_map[1][1][4] = 9;
        g_tofd_trig_map[1][1][5] = 9;
        g_tofd_trig_map[1][1][5] = 9;
        g_tofd_trig_map[1][1][6] = 9;
        g_tofd_trig_map[1][1][6] = 9;
        g_tofd_trig_map[1][1][7] = 9;
        g_tofd_trig_map[1][1][7] = 9;
        g_tofd_trig_map[1][1][8] = 9;
        g_tofd_trig_map[1][1][8] = 9;
        g_tofd_trig_map[1][1][9] = 9;
        g_tofd_trig_map[1][1][9] = 9;
        g_tofd_trig_map[1][1][10] = 9;
        g_tofd_trig_map[1][1][10] = 9;
        g_tofd_trig_map[1][1][11] = 9;
        g_tofd_trig_map[1][1][11] = 9;
        g_tofd_trig_map[1][1][12] = 9;
        g_tofd_trig_map[1][1][12] = 9;
        g_tofd_trig_map[1][1][13] = 9;
        g_tofd_trig_map[1][1][13] = 9;
        g_tofd_trig_map[1][1][14] = 9;
        g_tofd_trig_map[1][1][14] = 9;
        g_tofd_trig_map[1][1][15] = 9;
        g_tofd_trig_map[1][1][15] = 9;
        g_tofd_trig_map[1][1][16] = 10;
        g_tofd_trig_map[1][1][16] = 10;
        g_tofd_trig_map[1][1][17] = 10;
        g_tofd_trig_map[1][1][17] = 10;
        g_tofd_trig_map[1][1][18] = 10;
        g_tofd_trig_map[1][1][18] = 10;
        g_tofd_trig_map[1][1][19] = 10;
        g_tofd_trig_map[1][1][19] = 10;
        g_tofd_trig_map[1][1][20] = 10;
        g_tofd_trig_map[1][1][20] = 10;
        g_tofd_trig_map[1][1][21] = 10;
        g_tofd_trig_map[1][1][21] = 10;
        g_tofd_trig_map[1][1][22] = 10;
        g_tofd_trig_map[1][1][22] = 10;
        g_tofd_trig_map[1][1][23] = 10;
        g_tofd_trig_map[1][1][23] = 10;
        g_tofd_trig_map[1][1][24] = 10;
        g_tofd_trig_map[1][1][24] = 10;
        g_tofd_trig_map[1][1][25] = 10;
        g_tofd_trig_map[1][1][25] = 10;
        g_tofd_trig_map[1][1][26] = 10;
        g_tofd_trig_map[1][1][26] = 10;
        g_tofd_trig_map[1][1][27] = 10;
        g_tofd_trig_map[1][1][27] = 10;
        g_tofd_trig_map[1][1][28] = 10;
        g_tofd_trig_map[1][1][28] = 10;
        g_tofd_trig_map[1][1][29] = 10;
        g_tofd_trig_map[1][1][29] = 10;
        g_tofd_trig_map[1][1][30] = 10;
        g_tofd_trig_map[1][1][30] = 10;
        g_tofd_trig_map[1][1][31] = 10;
        g_tofd_trig_map[1][1][31] = 10;
        g_tofd_trig_map[1][1][32] = 11;
        g_tofd_trig_map[1][1][32] = 11;
        g_tofd_trig_map[1][1][33] = 11;
        g_tofd_trig_map[1][1][33] = 11;
        g_tofd_trig_map[1][1][34] = 11;
        g_tofd_trig_map[1][1][34] = 11;
        g_tofd_trig_map[1][1][35] = 11;
        g_tofd_trig_map[1][1][35] = 11;
        g_tofd_trig_map[1][1][36] = 11;
        g_tofd_trig_map[1][1][36] = 11;
        g_tofd_trig_map[1][1][37] = 11;
        g_tofd_trig_map[1][1][37] = 11;
        g_tofd_trig_map[1][1][38] = 11;
        g_tofd_trig_map[1][1][38] = 11;
        g_tofd_trig_map[1][1][39] = 11;
        g_tofd_trig_map[1][1][39] = 11;
        g_tofd_trig_map[1][1][40] = 11;
        g_tofd_trig_map[1][1][40] = 11;
        g_tofd_trig_map[1][1][41] = 11;
        g_tofd_trig_map[1][1][41] = 11;
        g_tofd_trig_map[1][1][42] = 11;
        g_tofd_trig_map[1][1][42] = 11;
        g_tofd_trig_map[1][1][43] = 11;
        g_tofd_trig_map[1][1][43] = 11;
        g_tofd_trig_map[2][0][0] = 12;
        g_tofd_trig_map[2][0][0] = 12;
        g_tofd_trig_map[2][0][1] = 12;
        g_tofd_trig_map[2][0][1] = 12;
        g_tofd_trig_map[2][0][2] = 12;
        g_tofd_trig_map[2][0][2] = 12;
        g_tofd_trig_map[2][0][3] = 12;
        g_tofd_trig_map[2][0][3] = 12;
        g_tofd_trig_map[2][0][4] = 12;
        g_tofd_trig_map[2][0][4] = 12;
        g_tofd_trig_map[2][0][5] = 12;
        g_tofd_trig_map[2][0][5] = 12;
        g_tofd_trig_map[2][0][6] = 12;
        g_tofd_trig_map[2][0][6] = 12;
        g_tofd_trig_map[2][0][7] = 12;
        g_tofd_trig_map[2][0][7] = 12;
        g_tofd_trig_map[2][0][8] = 12;
        g_tofd_trig_map[2][0][8] = 12;
        g_tofd_trig_map[2][0][9] = 12;
        g_tofd_trig_map[2][0][9] = 12;
        g_tofd_trig_map[2][0][10] = 12;
        g_tofd_trig_map[2][0][10] = 12;
        g_tofd_trig_map[2][0][11] = 12;
        g_tofd_trig_map[2][0][11] = 12;
        g_tofd_trig_map[2][0][12] = 12;
        g_tofd_trig_map[2][0][12] = 12;
        g_tofd_trig_map[2][0][13] = 12;
        g_tofd_trig_map[2][0][13] = 12;
        g_tofd_trig_map[2][0][14] = 12;
        g_tofd_trig_map[2][0][14] = 12;
        g_tofd_trig_map[2][0][15] = 12;
        g_tofd_trig_map[2][0][15] = 12;
        g_tofd_trig_map[2][0][16] = 13;
        g_tofd_trig_map[2][0][16] = 13;
        g_tofd_trig_map[2][0][17] = 13;
        g_tofd_trig_map[2][0][17] = 13;
        g_tofd_trig_map[2][0][18] = 13;
        g_tofd_trig_map[2][0][18] = 13;
        g_tofd_trig_map[2][0][19] = 13;
        g_tofd_trig_map[2][0][19] = 13;
        g_tofd_trig_map[2][0][20] = 13;
        g_tofd_trig_map[2][0][20] = 13;
        g_tofd_trig_map[2][0][21] = 13;
        g_tofd_trig_map[2][0][21] = 13;
        g_tofd_trig_map[2][0][22] = 13;
        g_tofd_trig_map[2][0][22] = 13;
        g_tofd_trig_map[2][0][23] = 13;
        g_tofd_trig_map[2][0][23] = 13;
        g_tofd_trig_map[2][0][24] = 13;
        g_tofd_trig_map[2][0][24] = 13;
        g_tofd_trig_map[2][0][25] = 13;
        g_tofd_trig_map[2][0][25] = 13;
        g_tofd_trig_map[2][0][26] = 13;
        g_tofd_trig_map[2][0][26] = 13;
        g_tofd_trig_map[2][0][27] = 13;
        g_tofd_trig_map[2][0][27] = 13;
        g_tofd_trig_map[2][0][28] = 13;
        g_tofd_trig_map[2][0][28] = 13;
        g_tofd_trig_map[2][0][29] = 13;
        g_tofd_trig_map[2][0][29] = 13;
        g_tofd_trig_map[2][0][30] = 13;
        g_tofd_trig_map[2][0][30] = 13;
        g_tofd_trig_map[2][0][31] = 13;
        g_tofd_trig_map[2][0][31] = 13;
        g_tofd_trig_map[2][0][32] = 14;
        g_tofd_trig_map[2][0][32] = 14;
        g_tofd_trig_map[2][0][33] = 14;
        g_tofd_trig_map[2][0][33] = 14;
        g_tofd_trig_map[2][0][34] = 14;
        g_tofd_trig_map[2][0][34] = 14;
        g_tofd_trig_map[2][0][35] = 14;
        g_tofd_trig_map[2][0][35] = 14;
        g_tofd_trig_map[2][0][36] = 14;
        g_tofd_trig_map[2][0][36] = 14;
        g_tofd_trig_map[2][0][37] = 14;
        g_tofd_trig_map[2][0][37] = 14;
        g_tofd_trig_map[2][0][38] = 14;
        g_tofd_trig_map[2][0][38] = 14;
        g_tofd_trig_map[2][0][39] = 14;
        g_tofd_trig_map[2][0][39] = 14;
        g_tofd_trig_map[2][0][40] = 14;
        g_tofd_trig_map[2][0][40] = 14;
        g_tofd_trig_map[2][0][41] = 14;
        g_tofd_trig_map[2][0][41] = 14;
        g_tofd_trig_map[2][0][42] = 14;
        g_tofd_trig_map[2][0][42] = 14;
        g_tofd_trig_map[2][0][43] = 14;
        g_tofd_trig_map[2][0][43] = 14;
        g_tofd_trig_map[2][1][0] = 15;
        g_tofd_trig_map[2][1][0] = 15;
        g_tofd_trig_map[2][1][1] = 15;
        g_tofd_trig_map[2][1][1] = 15;
        g_tofd_trig_map[2][1][2] = 15;
        g_tofd_trig_map[2][1][2] = 15;
        g_tofd_trig_map[2][1][3] = 15;
        g_tofd_trig_map[2][1][3] = 15;
        g_tofd_trig_map[2][1][4] = 15;
        g_tofd_trig_map[2][1][4] = 15;
        g_tofd_trig_map[2][1][5] = 15;
        g_tofd_trig_map[2][1][5] = 15;
        g_tofd_trig_map[2][1][6] = 15;
        g_tofd_trig_map[2][1][6] = 15;
        g_tofd_trig_map[2][1][7] = 15;
        g_tofd_trig_map[2][1][7] = 15;
        g_tofd_trig_map[2][1][8] = 15;
        g_tofd_trig_map[2][1][8] = 15;
        g_tofd_trig_map[2][1][9] = 15;
        g_tofd_trig_map[2][1][9] = 15;
        g_tofd_trig_map[2][1][10] = 15;
        g_tofd_trig_map[2][1][10] = 15;
        g_tofd_trig_map[2][1][11] = 15;
        g_tofd_trig_map[2][1][11] = 15;
        g_tofd_trig_map[2][1][12] = 15;
        g_tofd_trig_map[2][1][12] = 15;
        g_tofd_trig_map[2][1][13] = 15;
        g_tofd_trig_map[2][1][13] = 15;
        g_tofd_trig_map[2][1][14] = 15;
        g_tofd_trig_map[2][1][14] = 15;
        g_tofd_trig_map[2][1][15] = 15;
        g_tofd_trig_map[2][1][15] = 15;
        g_tofd_trig_map[2][1][16] = 16;
        g_tofd_trig_map[2][1][16] = 16;
        g_tofd_trig_map[2][1][17] = 16;
        g_tofd_trig_map[2][1][17] = 16;
        g_tofd_trig_map[2][1][18] = 16;
        g_tofd_trig_map[2][1][18] = 16;
        g_tofd_trig_map[2][1][19] = 16;
        g_tofd_trig_map[2][1][19] = 16;
        g_tofd_trig_map[2][1][20] = 16;
        g_tofd_trig_map[2][1][20] = 16;
        g_tofd_trig_map[2][1][21] = 16;
        g_tofd_trig_map[2][1][21] = 16;
        g_tofd_trig_map[2][1][22] = 16;
        g_tofd_trig_map[2][1][22] = 16;
        g_tofd_trig_map[2][1][23] = 16;
        g_tofd_trig_map[2][1][23] = 16;
        g_tofd_trig_map[2][1][24] = 16;
        g_tofd_trig_map[2][1][24] = 16;
        g_tofd_trig_map[2][1][25] = 16;
        g_tofd_trig_map[2][1][25] = 16;
        g_tofd_trig_map[2][1][26] = 16;
        g_tofd_trig_map[2][1][26] = 16;
        g_tofd_trig_map[2][1][27] = 16;
        g_tofd_trig_map[2][1][27] = 16;
        g_tofd_trig_map[2][1][28] = 16;
        g_tofd_trig_map[2][1][28] = 16;
        g_tofd_trig_map[2][1][29] = 16;
        g_tofd_trig_map[2][1][29] = 16;
        g_tofd_trig_map[2][1][30] = 16;
        g_tofd_trig_map[2][1][30] = 16;
        g_tofd_trig_map[2][1][31] = 16;
        g_tofd_trig_map[2][1][31] = 16;
        g_tofd_trig_map[2][1][32] = 17;
        g_tofd_trig_map[2][1][32] = 17;
        g_tofd_trig_map[2][1][33] = 17;
        g_tofd_trig_map[2][1][33] = 17;
        g_tofd_trig_map[2][1][34] = 17;
        g_tofd_trig_map[2][1][34] = 17;
        g_tofd_trig_map[2][1][35] = 17;
        g_tofd_trig_map[2][1][35] = 17;
        g_tofd_trig_map[2][1][36] = 17;
        g_tofd_trig_map[2][1][36] = 17;
        g_tofd_trig_map[2][1][37] = 17;
        g_tofd_trig_map[2][1][37] = 17;
        g_tofd_trig_map[2][1][38] = 17;
        g_tofd_trig_map[2][1][38] = 17;
        g_tofd_trig_map[2][1][39] = 17;
        g_tofd_trig_map[2][1][39] = 17;
        g_tofd_trig_map[2][1][40] = 17;
        g_tofd_trig_map[2][1][40] = 17;
        g_tofd_trig_map[2][1][41] = 17;
        g_tofd_trig_map[2][1][41] = 17;
        g_tofd_trig_map[2][1][42] = 17;
        g_tofd_trig_map[2][1][42] = 17;
        g_tofd_trig_map[2][1][43] = 17;
        g_tofd_trig_map[2][1][43] = 17;
        g_tofd_trig_map[3][0][0] = 18;
        g_tofd_trig_map[3][0][0] = 18;
        g_tofd_trig_map[3][0][1] = 18;
        g_tofd_trig_map[3][0][1] = 18;
        g_tofd_trig_map[3][0][2] = 18;
        g_tofd_trig_map[3][0][2] = 18;
        g_tofd_trig_map[3][0][3] = 18;
        g_tofd_trig_map[3][0][3] = 18;
        g_tofd_trig_map[3][0][4] = 18;
        g_tofd_trig_map[3][0][4] = 18;
        g_tofd_trig_map[3][0][5] = 18;
        g_tofd_trig_map[3][0][5] = 18;
        g_tofd_trig_map[3][0][6] = 18;
        g_tofd_trig_map[3][0][6] = 18;
        g_tofd_trig_map[3][0][7] = 18;
        g_tofd_trig_map[3][0][7] = 18;
        g_tofd_trig_map[3][0][8] = 18;
        g_tofd_trig_map[3][0][8] = 18;
        g_tofd_trig_map[3][0][9] = 18;
        g_tofd_trig_map[3][0][9] = 18;
        g_tofd_trig_map[3][0][10] = 18;
        g_tofd_trig_map[3][0][10] = 18;
        g_tofd_trig_map[3][0][11] = 18;
        g_tofd_trig_map[3][0][11] = 18;
        g_tofd_trig_map[3][0][12] = 18;
        g_tofd_trig_map[3][0][12] = 18;
        g_tofd_trig_map[3][0][13] = 18;
        g_tofd_trig_map[3][0][13] = 18;
        g_tofd_trig_map[3][0][14] = 18;
        g_tofd_trig_map[3][0][14] = 18;
        g_tofd_trig_map[3][0][15] = 18;
        g_tofd_trig_map[3][0][15] = 18;
        g_tofd_trig_map[3][0][16] = 19;
        g_tofd_trig_map[3][0][16] = 19;
        g_tofd_trig_map[3][0][17] = 19;
        g_tofd_trig_map[3][0][17] = 19;
        g_tofd_trig_map[3][0][18] = 19;
        g_tofd_trig_map[3][0][18] = 19;
        g_tofd_trig_map[3][0][19] = 19;
        g_tofd_trig_map[3][0][19] = 19;
        g_tofd_trig_map[3][0][20] = 19;
        g_tofd_trig_map[3][0][20] = 19;
        g_tofd_trig_map[3][0][21] = 19;
        g_tofd_trig_map[3][0][21] = 19;
        g_tofd_trig_map[3][0][22] = 19;
        g_tofd_trig_map[3][0][22] = 19;
        g_tofd_trig_map[3][0][23] = 19;
        g_tofd_trig_map[3][0][23] = 19;
        g_tofd_trig_map[3][0][24] = 19;
        g_tofd_trig_map[3][0][24] = 19;
        g_tofd_trig_map[3][0][25] = 19;
        g_tofd_trig_map[3][0][25] = 19;
        g_tofd_trig_map[3][0][26] = 19;
        g_tofd_trig_map[3][0][26] = 19;
        g_tofd_trig_map[3][0][27] = 19;
        g_tofd_trig_map[3][0][27] = 19;
        g_tofd_trig_map[3][0][28] = 19;
        g_tofd_trig_map[3][0][28] = 19;
        g_tofd_trig_map[3][0][29] = 19;
        g_tofd_trig_map[3][0][29] = 19;
        g_tofd_trig_map[3][0][30] = 19;
        g_tofd_trig_map[3][0][30] = 19;
        g_tofd_trig_map[3][0][31] = 19;
        g_tofd_trig_map[3][0][31] = 19;
        g_tofd_trig_map[3][0][32] = 20;
        g_tofd_trig_map[3][0][32] = 20;
        g_tofd_trig_map[3][0][33] = 20;
        g_tofd_trig_map[3][0][33] = 20;
        g_tofd_trig_map[3][0][34] = 20;
        g_tofd_trig_map[3][0][34] = 20;
        g_tofd_trig_map[3][0][35] = 20;
        g_tofd_trig_map[3][0][35] = 20;
        g_tofd_trig_map[3][0][36] = 20;
        g_tofd_trig_map[3][0][36] = 20;
        g_tofd_trig_map[3][0][37] = 20;
        g_tofd_trig_map[3][0][37] = 20;
        g_tofd_trig_map[3][0][38] = 20;
        g_tofd_trig_map[3][0][38] = 20;
        g_tofd_trig_map[3][0][39] = 20;
        g_tofd_trig_map[3][0][39] = 20;
        g_tofd_trig_map[3][0][40] = 20;
        g_tofd_trig_map[3][0][40] = 20;
        g_tofd_trig_map[3][0][41] = 20;
        g_tofd_trig_map[3][0][41] = 20;
        g_tofd_trig_map[3][0][42] = 20;
        g_tofd_trig_map[3][0][42] = 20;
        g_tofd_trig_map[3][0][43] = 20;
        g_tofd_trig_map[3][0][43] = 20;
        g_tofd_trig_map[3][1][0] = 21;
        g_tofd_trig_map[3][1][0] = 21;
        g_tofd_trig_map[3][1][1] = 21;
        g_tofd_trig_map[3][1][1] = 21;
        g_tofd_trig_map[3][1][2] = 21;
        g_tofd_trig_map[3][1][2] = 21;
        g_tofd_trig_map[3][1][3] = 21;
        g_tofd_trig_map[3][1][3] = 21;
        g_tofd_trig_map[3][1][4] = 21;
        g_tofd_trig_map[3][1][4] = 21;
        g_tofd_trig_map[3][1][5] = 21;
        g_tofd_trig_map[3][1][5] = 21;
        g_tofd_trig_map[3][1][6] = 21;
        g_tofd_trig_map[3][1][6] = 21;
        g_tofd_trig_map[3][1][7] = 21;
        g_tofd_trig_map[3][1][7] = 21;
        g_tofd_trig_map[3][1][8] = 21;
        g_tofd_trig_map[3][1][8] = 21;
        g_tofd_trig_map[3][1][9] = 21;
        g_tofd_trig_map[3][1][9] = 21;
        g_tofd_trig_map[3][1][10] = 21;
        g_tofd_trig_map[3][1][10] = 21;
        g_tofd_trig_map[3][1][11] = 21;
        g_tofd_trig_map[3][1][11] = 21;
        g_tofd_trig_map[3][1][12] = 21;
        g_tofd_trig_map[3][1][12] = 21;
        g_tofd_trig_map[3][1][13] = 21;
        g_tofd_trig_map[3][1][13] = 21;
        g_tofd_trig_map[3][1][14] = 21;
        g_tofd_trig_map[3][1][14] = 21;
        g_tofd_trig_map[3][1][15] = 21;
        g_tofd_trig_map[3][1][15] = 21;
        g_tofd_trig_map[3][1][16] = 22;
        g_tofd_trig_map[3][1][16] = 22;
        g_tofd_trig_map[3][1][17] = 22;
        g_tofd_trig_map[3][1][17] = 22;
        g_tofd_trig_map[3][1][18] = 22;
        g_tofd_trig_map[3][1][18] = 22;
        g_tofd_trig_map[3][1][19] = 22;
        g_tofd_trig_map[3][1][19] = 22;
        g_tofd_trig_map[3][1][20] = 22;
        g_tofd_trig_map[3][1][20] = 22;
        g_tofd_trig_map[3][1][21] = 22;
        g_tofd_trig_map[3][1][21] = 22;
        g_tofd_trig_map[3][1][22] = 22;
        g_tofd_trig_map[3][1][22] = 22;
        g_tofd_trig_map[3][1][23] = 22;
        g_tofd_trig_map[3][1][23] = 22;
        g_tofd_trig_map[3][1][24] = 22;
        g_tofd_trig_map[3][1][24] = 22;
        g_tofd_trig_map[3][1][25] = 22;
        g_tofd_trig_map[3][1][25] = 22;
        g_tofd_trig_map[3][1][26] = 22;
        g_tofd_trig_map[3][1][26] = 22;
        g_tofd_trig_map[3][1][27] = 22;
        g_tofd_trig_map[3][1][27] = 22;
        g_tofd_trig_map[3][1][28] = 22;
        g_tofd_trig_map[3][1][28] = 22;
        g_tofd_trig_map[3][1][29] = 22;
        g_tofd_trig_map[3][1][29] = 22;
        g_tofd_trig_map[3][1][30] = 22;
        g_tofd_trig_map[3][1][30] = 22;
        g_tofd_trig_map[3][1][31] = 22;
        g_tofd_trig_map[3][1][31] = 22;
        g_tofd_trig_map[3][1][32] = 23;
        g_tofd_trig_map[3][1][32] = 23;
        g_tofd_trig_map[3][1][33] = 23;
        g_tofd_trig_map[3][1][33] = 23;
        g_tofd_trig_map[3][1][34] = 23;
        g_tofd_trig_map[3][1][34] = 23;
        g_tofd_trig_map[3][1][35] = 23;
        g_tofd_trig_map[3][1][35] = 23;
        g_tofd_trig_map[3][1][36] = 23;
        g_tofd_trig_map[3][1][36] = 23;
        g_tofd_trig_map[3][1][37] = 23;
        g_tofd_trig_map[3][1][37] = 23;
        g_tofd_trig_map[3][1][38] = 23;
        g_tofd_trig_map[3][1][38] = 23;
        g_tofd_trig_map[3][1][39] = 23;
        g_tofd_trig_map[3][1][39] = 23;
        g_tofd_trig_map[3][1][40] = 23;
        g_tofd_trig_map[3][1][40] = 23;
        g_tofd_trig_map[3][1][41] = 23;
        g_tofd_trig_map[3][1][41] = 23;
        g_tofd_trig_map[3][1][42] = 23;
        g_tofd_trig_map[3][1][42] = 23;
        g_tofd_trig_map[3][1][43] = 23;
        g_tofd_trig_map[3][1][43] = 23;
        */
}

R3BTofDCal2Hit::~R3BTofDCal2Hit()
{
    if (fTofdHisto)
    {
        // if (fhTpat)
        //   delete fhTpat;
        if (fhNoTpat)
            delete fhNoTpat;
        for (Int_t i = 0; i < fNofPlanes; i++)
        {
            if (fhQ[i])
                delete fhQ[i];
            if (fhxy[i])
                delete fhxy[i];
            if (fhQvsEvent[i])
                delete fhQvsEvent[i];
            if (fhTdiff[i])
                delete fhTdiff[i];
            if (fhTsync[i])
                delete fhTsync[i];
            if (fhQ0Qt[i])
                delete fhQ0Qt[i];
            if (fhTvsQ[i])
                delete fhTvsQ[i];
            for (Int_t j = 0; j < N_TOFD_HIT_PADDLE_MAX; j++)
            {
                if (fhQvsPos[i][j])
                    delete fhQvsPos[i][j];
            }
        }
    }
    if (fHitItems)
    {
        delete fHitItems;
        fHitItems = NULL;
    }
}

void R3BTofDCal2Hit::SetParContainers()
{
   // fMapPar = (R3BTofDMappingPar*)FairRuntimeDb::instance()->getContainer("tofdMappingPar");
   // R3BLOG_IF(WARNING, !fMapPar, "Could not get access to tofdMappingPar container");

    fHitPar = (R3BTofDHitPar*)FairRuntimeDb::instance()->getContainer("TofdHitPar");
    if (!fHitPar)
    {
        R3BLOG(FATAL, "Could not get access to TofdHitPar container");
        fNofHitPars = 0;
        return;
    }
}

void R3BTofDCal2Hit::SetParameter()
{
    //fHitPar = (R3BTofDHitPar*)FairRuntimeDb::instance()->getContainer("TofdHitPar");
    if (!fHitPar)
    {
        R3BLOG(FATAL, "Could not get access to TofdHitPar-Container.");
        fNofHitPars = 0;
    }
    fNofHitPars = fHitPar->GetNumModulePar();
    if (fNofHitPars == 0)
    {
        R3BLOG(FATAL, "There are no Hit parameters in container TofdHitPar");
    }
    return;
}

InitStatus R3BTofDCal2Hit::Init()
{
    R3BLOG(INFO, "");
    FairRootManager* mgr = FairRootManager::Instance();
    if (!mgr)
    {
        R3BLOG(FATAL, "FairRootManager not found");
        return kFATAL;
    }

    header = (R3BEventHeader*)mgr->GetObject("EventHeader.");
    R3BLOG_IF(fatal, NULL == header, "EventHeader. not found");

    fCalItems = (TClonesArray*)mgr->GetObject("TofdCal");
    R3BLOG_IF(fatal, NULL == fCalItems, "TofdCal not found");

    fCalTriggerItems = (TClonesArray*)mgr->GetObject("TofdTriggerCal");
    R3BLOG_IF(fatal, NULL == fCalTriggerItems, "TofdTriggerCal not found");

    maxevent = mgr->CheckMaxEventNo();

    // request storage of Hit data in output tree
    mgr->Register("TofdHit", "TofdHit data", fHitItems, !fOnline);

    SetParameter();

    if (fTofdHisto)
    {
        for (Int_t i = 1; i <= fNofPlanes; i++)
            for (Int_t j = 1; j <= N_TOFD_HIT_PADDLE_MAX; j++)
                CreateHistograms(i, j);
    }

    return kSUCCESS;
}

InitStatus R3BTofDCal2Hit::ReInit()
{
    SetParContainers();
    SetParameter();
    return kSUCCESS;
}

void R3BTofDCal2Hit::Exec(Option_t* option)
{
    static uint32_t counter = 0;
    ++counter;

    // test for requested trigger (if possible)
    if ((fTrigger >= 0) && (header) && (header->GetTrigger() != fTrigger))
    {
        wrongtrigger++;
        return;
    }
    // fTpat = 1-16; fTpat_bit = 0-15
    Int_t fTpat_bit1 = fTpat1 - 1;
    Int_t fTpat_bit2 = fTpat2 - 1;
    Int_t tpatbin;
    Int_t tpatsum = 0;
    if (header && fTpat1 >= 0 && fTpat2 >= 0)
    {
        for (int i = 0; i < 16; i++)
        {
            tpatbin = (header->GetTpat() & (1 << i));
            // LOG(DEBUG)<<"tpatbin "tpatbin;
            if (tpatbin != 0 && (i < fTpat_bit1 || i > fTpat_bit2))
            {
                wrongtpat++;
                return;
            }
            if (tpatbin != 0)
            {
                //  fhTpat->Fill(i+1);
                LOG(DEBUG) << "Accepted Tpat: " << i + 1;
            }
            tpatsum += tpatbin;
        }
        if (tpatsum < 1)
        {
            if (NULL == fhNoTpat)
            {
                fhNoTpat = new TH1F("NoTpat", "NoTpat", 200, 0, 200);
                fhNoTpat->GetXaxis()->SetTitle("No Tpat event dist");
            }
            fhNoTpat->Fill(counter - lasttpatevent);
            lasttpatevent = counter;
            notpat++;
            // LOG(fatal)<<"No Tpat info";
            return;
        }
    }

    headertpat++;
    Double_t timeP0 = 0.;
    Double_t randx;

    UInt_t vmultihits[N_PLANE_MAX + 1][N_TOFD_HIT_PADDLE_MAX + 1];
    for (Int_t i = 0; i <= fNofPlanes; i++)
    {
        for (Int_t j = 0; j <= N_TOFD_HIT_PADDLE_MAX; j++)
        {
            vmultihits[i][j] = 0;
        }
    }
    struct hit
    {
        Double_t charge;
        Double_t time;
        Double_t xpos;
        Double_t ypos;
        Int_t plane;
        Int_t bar;
        Double_t time_raw;
    };

    //    std::cout<<"new event!*************************************\n";

    std::vector<hit> event;

    Int_t nHits = fCalItems->GetEntries();
    LOG(DEBUG) << "Leading and trailing edges in this event: " << nHits;
    if (nHits == 0)
        events_wo_tofd_hits++;

    // Organize cals into bars.
    struct Entry
    {
        std::vector<R3BTofdCalData*> top;
        std::vector<R3BTofdCalData*> bot;
    };
    std::map<size_t, Entry> bar_map;
    // puts("Event");
    for (Int_t ihit = 0; ihit < nHits; ihit++)
    {
        auto* hit = (R3BTofdCalData*)fCalItems->At(ihit);
        size_t idx = hit->GetDetectorId() * fPaddlesPerPlane * hit->GetBarId();

        // std::cout << "Hits: " << hit->GetDetectorId() << ' ' << hit->GetBarId() << ' ' << hit->GetSideId() << '  '
        //          << hit->GetTimeLeading_ns() << ' ' << hit->GetTimeTrailing_ns() << ' '
        //          << hit->GetTimeTrailing_ns() - hit->GetTimeLeading_ns() << '\n';

        auto ret = bar_map.insert(std::pair<size_t, Entry>(idx, Entry()));
        auto& vec = 1 == hit->GetSideId() ? ret.first->second.bot : ret.first->second.top;
        vec.push_back(hit);
        events_in_cal_level++;
    }

    // Build trigger map.
    std::vector<R3BTofdCalData const*> trig_map;
    for (int i = 0; i < fCalTriggerItems->GetEntries(); ++i)
    {
        auto trig = (R3BTofdCalData const*)fCalTriggerItems->At(i);
        if (trig_map.size() < trig->GetBarId())
        {
            trig_map.resize(trig->GetBarId());
        }
        trig_map.at(trig->GetBarId() - 1) = trig;
    }

    bool s_was_trig_missing = false;
    // Find coincident PMT hits.
    // std::cout << "Print:\n";
    for (auto it = bar_map.begin(); bar_map.end() != it; ++it)
    {
        //    reset:
        // for (auto it2 = it->second.top.begin(); it->second.top.end() != it2; ++it2) {
        // std::cout << "Top: " << (*it2)->GetDetectorId() << ' ' << (*it2)->GetBarId() << ' ' <<
        // (*it2)->GetTimeLeading_ns() << '\n';
        // }
        // for (auto it2 = it->second.bot.begin(); it->second.bot.end() != it2; ++it2) {
        // std::cout << "Bot: " << (*it2)->GetDetectorId() << ' ' << (*it2)->GetBarId() << ' ' <<
        // (*it2)->GetTimeLeading_ns() << '\n';
        // }
        auto const& top_vec = it->second.top;
        auto const& bot_vec = it->second.bot;
        size_t top_i = 0;
        size_t bot_i = 0;
        for (; top_i < top_vec.size() && bot_i < bot_vec.size();)
        {
            auto top = top_vec.at(top_i);
            auto bot = bot_vec.at(bot_i);
            // auto top_trig_i = g_tofd_trig_map[top->GetDetectorId() - 1][top->GetSideId() - 1][top->GetBarId() - 1];
            // auto bot_trig_i = g_tofd_trig_map[bot->GetDetectorId() - 1][bot->GetSideId() - 1][bot->GetBarId() - 1];

            Int_t top_trig_i = 0;
            Int_t bot_trig_i = 0;

            if (fMapPar)
            {
                top_trig_i = fMapPar->GetTrigMap(top->GetDetectorId(), top->GetBarId(), top->GetSideId());
                bot_trig_i = fMapPar->GetTrigMap(bot->GetDetectorId(), bot->GetBarId(), bot->GetSideId());
            }

            Double_t top_trig_ns = 0, bot_trig_ns = 0;
            if (top_trig_i < trig_map.size() && trig_map.at(top_trig_i) && bot_trig_i < trig_map.size() &&
                trig_map.at(bot_trig_i))
            {
                auto top_trig = trig_map.at(top_trig_i);
                auto bot_trig = trig_map.at(bot_trig_i);
                top_trig_ns = top_trig->GetTimeLeading_ns();
                bot_trig_ns = bot_trig->GetTimeLeading_ns();
                /*
                                std::cout << "Top: " << top->GetDetectorId() << ' ' << top->GetSideId() << ' ' <<
                   top->GetBarId() << ' '
                                << top_trig_i << ' ' << top_trig->GetTimeLeading_ns() << std::endl;
                                std::cout << "Bot: " <<
                                bot->GetDetectorId() << ' ' << bot->GetSideId() << ' ' << bot->GetBarId() << ' ' <<
                   bot_trig_i << ' '
                                << bot_trig->GetTimeLeading_ns() << std::endl;
                */
                ++n1;
            }
            else
            {
                if (!s_was_trig_missing)
                {
                    LOG(ERROR) << "R3BTofdCal2HitS494Par::Exec() : Missing trigger information!";
                    LOG(ERROR) << "Top: " << top->GetDetectorId() << ' ' << top->GetSideId() << ' ' << top->GetBarId();
                    LOG(ERROR) << "Bot: " << bot->GetDetectorId() << ' ' << bot->GetSideId() << ' ' << bot->GetBarId();
                    s_was_trig_missing = true;
                }
                ++n2;
            }

            // Shift the cyclic difference window by half a window-length and move it back,
            // this way the trigger time will be at 0.
            auto top_ns =
                fmod(top->GetTimeLeading_ns() - top_trig_ns + c_range_ns + c_range_ns / 2, c_range_ns) - c_range_ns / 2;
            auto bot_ns =
                fmod(bot->GetTimeLeading_ns() - bot_trig_ns + c_range_ns + c_range_ns / 2, c_range_ns) - c_range_ns / 2;

            /*
                        if(top_ns>2000 || bot_ns>2000){
                            std::cout << top->GetTimeLeading_ns() << ' ' << top_trig_ns << ' ' << top_ns << std::endl;
                            std::cout << bot->GetTimeLeading_ns() << ' ' << bot_trig_ns << ' ' << bot_ns << std::endl;
                        }
            */
            auto dt = top_ns - bot_ns;
            // Handle wrap-around.
            auto dt_mod = fmod(dt + c_range_ns, c_range_ns);
            if (dt < 0)
            {
                // We're only interested in the short time-differences, so we
                // want to move the upper part of the coarse counter range close
                // to the lower range, i.e. we cut the middle of the range and
                // glue zero and the largest values together.
                dt_mod -= c_range_ns;
            }
            // std::cout << top_i << ' ' << bot_i << ": " << top_ns << ' ' << bot_ns << " = " << dt << ' ' <<
            // std::abs(dt_mod) << '\n';
            if (std::abs(dt_mod) < c_bar_coincidence_ns)
            {
                inbarcoincidence++;
                // Hit!
                Int_t iPlane = top->GetDetectorId(); // 1..n
                Int_t iBar = top->GetBarId();        // 1..n
                if (iPlane > fNofPlanes)             // this also errors for iDetector==0
                {
                    R3BLOG(ERROR, "More detectors than expected! Det: " << iPlane << " allowed are 1.." << fNofPlanes);
                    continue;
                }
                if (iBar > fPaddlesPerPlane) // same here
                {
                    R3BLOG(ERROR, "More bars then expected! Det: " << iBar << " allowed are 1.." << fPaddlesPerPlane);
                    continue;
                }

                auto top_tot = fmod(top->GetTimeTrailing_ns() - top->GetTimeLeading_ns() + c_range_ns, c_range_ns);
                auto bot_tot = fmod(bot->GetTimeTrailing_ns() - bot->GetTimeLeading_ns() + c_range_ns, c_range_ns);

                auto THit_raw = (bot->GetTimeLeading_ns() + top->GetTimeLeading_ns()) / 2.; // needed for TOF for ROLUs

                // std::cout<<"ToT: "<<top_tot << " "<<bot_tot<<"\n";

                // register multi hits
                vmultihits[iPlane][iBar] += 1;

                R3BTofDHitModulePar* par = fHitPar->GetModuleParAt(iPlane, iBar);
                if (!par)
                {
                    R3BLOG(ERROR, "Hit par not found, Plane: " << top->GetDetectorId() << ", Bar: " << top->GetBarId());
                    continue;
                }

                // walk corrections
                if (par->GetPar1Walk() == 0. || par->GetPar2Walk() == 0. || par->GetPar3Walk() == 0. ||
                    par->GetPar4Walk() == 0. || par->GetPar5Walk() == 0.)
                {
                    R3BLOG(DEBUG, "TofD walk correction not found");
                }
                else
                {
                    auto bot_ns_walk = bot_ns - walk(bot_tot,
                                                     par->GetPar1Walk(),
                                                     par->GetPar2Walk(),
                                                     par->GetPar3Walk(),
                                                     par->GetPar4Walk(),
                                                     par->GetPar5Walk());
                    auto top_ns_walk = top_ns - walk(top_tot,
                                                     par->GetPar1Walk(),
                                                     par->GetPar2Walk(),
                                                     par->GetPar3Walk(),
                                                     par->GetPar4Walk(),
                                                     par->GetPar5Walk());
                }

                // calculate tdiff
                auto tdiff = ((bot_ns + par->GetOffset1()) - (top_ns + par->GetOffset2()));

                // calculate time of hit
                Double_t THit = (bot_ns + top_ns) / 2. - par->GetSync();
                if (std::isnan(THit))
                {
                    R3BLOG(FATAL, "TofD THit not found");
                }
                if (timeP0 == 0.)
                    timeP0 = THit;

                // calculate y-position
                auto pos = ((bot_ns + par->GetOffset1()) - (top_ns + par->GetOffset2())) * par->GetVeff();

                // calculate y-position from ToT
                auto posToT =
                    par->GetLambda() * log((top_tot * par->GetToTOffset2()) / (bot_tot * par->GetToTOffset1()));

                if (fTofdTotPos)
                {
                    pos = posToT;
                }

                Float_t paddle_width = 2.70000;
                Float_t paddle_thickness = 0.50000;
                Float_t air_gap_paddles = 0.04;
                Float_t air_gap_layer = 5.;
                // define number of layers and paddles with sizes of the detector
                Int_t number_layers = 2;   // number of layers
                Int_t number_paddles = 44; // number of paddles per layer
                Float_t detector_width =
                    number_paddles * paddle_width + (number_paddles - 1) * air_gap_paddles + paddle_width;
                Float_t detector_thickness = (number_layers - 1) * air_gap_layer + number_layers * paddle_thickness;
                Double_t xp = -1000.;
                // calculate x-position
                if (iPlane == 1 || iPlane == 3)
                {
                    xp = -detector_width / 2 + (paddle_width + air_gap_paddles) / 2 +
                         (iBar - 1) * (paddle_width + air_gap_paddles) - 0.04;
                }
                if (iPlane == 2 || iPlane == 4)
                {
                    xp = -detector_width / 2 + (paddle_width + air_gap_paddles) +
                         (iBar - 1) * (paddle_width + air_gap_paddles) - 0.04;
                }

                Double_t para[4];
                Double_t qb = 0.;
                if (fTofdQ > 0)
                {
                    if (fTofdTotPos)
                    {
                        // via pol3
                        para[0] = par->GetPola();
                        para[1] = par->GetPolb();
                        para[2] = par->GetPolc();
                        para[3] = par->GetPold();
                        qb = TMath::Sqrt(top_tot * bot_tot) /
                             (para[0] + para[1] * pos + para[2] * pow(pos, 2) + para[3] * pow(pos, 3));
                        qb = qb * fTofdQ;
                    }
                    else
                    {
                        // via double exponential:
                        para[0] = par->GetPar1a();
                        para[1] = par->GetPar1b();
                        para[2] = par->GetPar1c();
                        para[3] = par->GetPar1d();
                        auto q1 = bot_tot /
                                  (para[0] * (exp(-para[1] * (pos + 100.)) + exp(-para[2] * (pos + 100.))) + para[3]);
                        para[0] = par->GetPar2a();
                        para[1] = par->GetPar2b();
                        para[2] = par->GetPar2c();
                        para[3] = par->GetPar2d();
                        auto q2 = top_tot /
                                  (para[0] * (exp(-para[1] * (pos + 100.)) + exp(-para[2] * (pos + 100.))) + para[3]);
                        q1 = q1 * fTofdQ;
                        q2 = q2 * fTofdQ;
                        qb = (q1 + q2) / 2.;
                    }
                }
                else
                {
                    qb = TMath::Sqrt(top_tot * bot_tot);
                }

                Double_t parz[3];
                parz[0] = par->GetPar1za();
                parz[1] = par->GetPar1zb();
                parz[2] = par->GetPar1zc();

                if (parz[0] > 0 && parz[2] > 0)
                    LOG(DEBUG) << "Charges in this event " << parz[0] * TMath::Power(qb, parz[2]) + parz[1] << " plane "
                               << iPlane << " ibar " << iBar;
                else
                    LOG(DEBUG) << "Charges in this event " << qb << " plane " << iPlane << " ibar " << iBar;
                LOG(DEBUG) << "Times in this event " << THit << " plane " << iPlane << " ibar " << iBar;
                if (iPlane == 1 || iPlane == 3)
                    LOG(DEBUG) << "x in this event "
                               << -detector_width / 2 + (paddle_width + air_gap_paddles) / 2 +
                                      (iBar - 1) * (paddle_width + air_gap_paddles) - 0.04
                               << " plane " << iPlane << " ibar " << iBar;
                if (iPlane == 2 || iPlane == 4)
                    LOG(DEBUG) << "x in this event "
                               << -detector_width / 2 + (paddle_width + air_gap_paddles) +
                                      (iBar - 1) * (paddle_width + air_gap_paddles) - 0.04
                               << " plane " << iPlane << " ibar " << iBar;
                LOG(DEBUG) << "y in this event " << pos << " plane " << iPlane << " ibar " << iBar << "\n";

                if (parz[0] > 0 && parz[2] > 0)
                {
                    event.push_back(
                        { parz[0] * TMath::Power(qb, parz[2]) + parz[1], THit, xp, pos, iPlane, iBar, THit_raw });
                }
                else
                {
                    parz[0] = 1.;
                    parz[1] = 0.;
                    parz[2] = 1.;

                    event.push_back({ qb, THit, xp, pos, iPlane, iBar, THit_raw });
                }

                if (fTofdHisto)
                {
                    // fill control histograms
                    fhTsync[iPlane - 1]->Fill(iBar, THit);
                    fhTdiff[iPlane - 1]->Fill(iBar, tdiff);
                    fhQvsPos[iPlane - 1][iBar - 1]->Fill(pos, parz[0] * TMath::Power(qb, parz[2]) + parz[1]);
                    // fhQvsTHit[iPlane - 1][iBar - 1]->Fill(qb, THit);
                    // fhTvsTHit[iPlane - 1][iBar - 1]->Fill(dt_mod, THit);
                }

                for (Int_t e = 0; e < event.size(); e++)
                {
                    LOG(DEBUG) << event[e].charge << " " << event[e].time << " " << event[e].xpos << " "
                               << event[e].ypos << " " << event[e].plane << " " << event[e].bar;
                }

                ++top_i;
                ++bot_i;
            }
            else if (dt < 0 && dt > -c_range_ns / 2)
            {
                ++top_i;
                LOG(DEBUG) << "Not in bar coincidence increase top counter";
            }
            else
            {
                ++bot_i;
                LOG(DEBUG) << "Not in bar coincidence increase bot counter";
            }
        }
    }

    // Now all hits in this event are analyzed

    LOG(DEBUG) << "Hits in this event: " << event.size();

    Bool_t tArrU[event.size() + 1];
    for (int i = 0; i < (event.size() + 1); i++)
        tArrU[i] = kFALSE;

    for (Int_t i = 1; i <= fNofPlanes; i++)
    {
        for (Int_t j = 0; j < fPaddlesPerPlane + 1; j++)
        {
            if (vmultihits[i][j] > 1)
            {
                bars_with_multihit++;
                multihit += vmultihits[i][j] - 1;
                // if(vmultihits[i][j]>3)cout<<vmultihits[i][j] - 1<<"\n";
            }
        }
    }

    std::sort(event.begin(), event.end(), [](hit const& a, hit const& b) { return a.time < b.time; });
    // Now we have all hits in this event time sorted

    if (fTofdHisto)
    {
        LOG(DEBUG) << "Charge Time xpos ypos plane bar";
        for (Int_t hit = 0; hit < event.size(); hit++)
        {
            LOG(DEBUG) << event[hit].charge << " " << event[hit].time << " " << event[hit].xpos << " "
                       << event[hit].ypos << " " << event[hit].plane << " " << event[hit].bar;
            // if (event[hit].plane == 2 && (event[hit].bar < 21 || event[hit].bar > 24)) fhTvsQ[event[hit].plane -
            // 1]->Fill(event[hit].time-event[0].time,event[hit].charge);
            if (event[hit].plane == 2 && (event[hit].bar == 18))
                fhTvsQ[event[hit].plane - 1]->Fill(event[hit].time - event[0].time, event[hit].charge);
        }
    }

    // Now we can analyze the hits in this event

    // select events with feasible times
    Double_t hit_coinc = 20.; // coincidence window for hits in one event in ns. physics says max 250 ps
    Double_t time0;
    for (Int_t ihit = 0; ihit < event.size();)
    { // loop over all hits in this event
        LOG(DEBUG) << "Set new coincidence window: " << event[ihit].plane << " " << event[ihit].bar << " "
                   << event[ihit].time << " " << event[ihit].charge;
        time0 = event[ihit].time;              // time of first hit in coincidence window
        Double_t charge0 = event[ihit].charge; // charge of first hit in coincidence window
        Int_t plane0 = event[ihit].plane;      // plane of first hit in coincidence window
        std::vector<Double_t> goodcharge;
        std::vector<Double_t> goodplane;
        std::vector<Double_t> goodbar;
        struct goodhit
        {
            Double_t goodq;
            Double_t goodp;
            Double_t goodb;
        };
        struct by_charge
        {
            bool operator()(goodhit const& a, goodhit const& b) const noexcept { return a.goodq < b.goodq; }
        };
        std::vector<goodhit> goodevents;

        while (event[ihit].time < time0 + hit_coinc)
        { // check if in coincidence window
            if (fTofdHisto)
            {
                if (event[ihit].plane == plane0 && charge0 != event[ihit].charge)
                {
                    fhQ0Qt[event[ihit].plane - 1]->Fill(charge0, event[ihit].charge);
                }
            }

            if ((event[ihit].charge > 5.5 && event[ihit].charge < 6.5) ||
                (event[ihit].charge > 1.5 && event[ihit].charge < 2.5))
            {
                goodcharge.push_back(event[ihit].charge);
                goodplane.push_back(event[ihit].plane);
                goodbar.push_back(event[ihit].bar);
            }

            LOG(DEBUG) << "Hit in coincidence window: " << event[ihit].plane << " " << event[ihit].bar << " "
                       << event[ihit].time << " " << event[ihit].charge;

            ihit++;
            if (ihit >= event.size())
                break;
        }
        if (goodcharge.size() > 3)
        {
            if (goodcharge.size() == 4)
            {
                if (std::accumulate(goodcharge.begin(), goodcharge.end(), 0.0) > 14. &&
                    std::accumulate(goodcharge.begin(), goodcharge.end(), 0.0) < 18.)
                {
                    if (std::accumulate(goodplane.begin(), goodplane.end(), 0) == 6.)
                    {
                        LOG(DEBUG) << "Found good pair 2 times in all planes";
                        for (Int_t g = 0; g < goodcharge.size(); g++)
                        {
                            LOG(DEBUG) << goodcharge.at(g);
                            LOG(DEBUG) << goodplane.at(g);
                            LOG(DEBUG) << goodbar.at(g);
                            goodevents.push_back({ goodcharge.at(g), goodplane.at(g), goodbar.at(g) });
                        }
                        goodpair++;
                        goodpair4++;
                    }
                    else
                    {
                        goodpair3++;
                    }
                }
            }
            else if (std::accumulate(goodcharge.begin(), goodcharge.end(), 0.0) > 14.)
                goodpair6++;
        }
        if (goodcharge.size() == 3)
        {
            if (std::accumulate(goodcharge.begin(), goodcharge.end(), 0.0) > 8.5 &&
                std::accumulate(goodcharge.begin(), goodcharge.end(), 0.0) < 15.5)
            {
                if (std::accumulate(goodplane.begin(), goodplane.end(), 0) == 4 ||
                    std::accumulate(goodplane.begin(), goodplane.end(), 0) == 5)
                {
                    LOG(DEBUG) << "Found good pair at least once in all planes";
                    for (Int_t g = 0; g < goodcharge.size(); g++)
                    {
                        LOG(DEBUG) << goodcharge.at(g);
                    }
                    goodpair++;
                    goodpair5++;
                }
                else
                {
                    goodpair7++;
                }
            }
        }
        if (goodcharge.size() == 2)
        {
            if (std::accumulate(goodcharge.begin(), goodcharge.end(), 0.0) > 7. &&
                std::accumulate(goodcharge.begin(), goodcharge.end(), 0.0) < 9.)
            {
                if (std::accumulate(goodplane.begin(), goodplane.end(), 0) == 2. ||
                    std::accumulate(goodplane.begin(), goodplane.end(), 0) == 4.)
                {
                    LOG(DEBUG) << "Found good pair in one plane";
                    for (Int_t g = 0; g < goodcharge.size(); g++)
                    {
                        LOG(DEBUG) << goodcharge.at(g);
                    }
                    goodpair++;
                    goodpair1++;
                }
                if (std::accumulate(goodplane.begin(), goodplane.end(), 0) == 3.)
                {
                    LOG(DEBUG) << "Found good pair in different planes";
                    for (Int_t g = 0; g < goodcharge.size(); g++)
                    {
                        LOG(DEBUG) << goodcharge.at(g);
                    }
                    goodpair++;
                    goodpair2++;
                }
            }
        }
        std::sort(
            goodevents.begin(), goodevents.end(), [](goodhit const& a, goodhit const& b) { return a.goodq < b.goodq; });
        for (Int_t g = 0; g < goodevents.size(); g++)
        {
            LOG(DEBUG) << goodevents[g].goodq;
            LOG(DEBUG) << goodevents[g].goodp;
            LOG(DEBUG) << goodevents[g].goodb;
        }
    }

    if (fTofdHisto)
    {
        size_t ihit = 0;
        for (; ihit < event.size();)
        {                                                                          // loop over all hits
            fhQ[event[ihit].plane - 1]->Fill(event[ihit].bar, event[ihit].charge); // charge per plane
            fhQvsEvent[event[ihit].plane - 1]->Fill(fnEvents, event[ihit].charge); // charge vs event #
            fhxy[event[ihit].plane - 1]->Fill(event[ihit].bar, event[ihit].ypos);  // xy of plane
            ihit++;
        }
    }

    // store events
    for (Int_t hit = 0; hit < event.size(); hit++)
    { // loop over hits
        if (tArrU[hit] == false)
        {
            eventstore++;
            tArrU[hit] = true;
            // store single hits
            singlehit++;
            new ((*fHitItems)[fHitItems->GetEntriesFast()]) R3BTofdHitData(event[hit].time,
                                                                           event[hit].xpos,
                                                                           event[hit].ypos,
                                                                           event[hit].charge,
                                                                           -1.,
                                                                           event[hit].charge,
                                                                           event[hit].plane,
                                                                           event[hit].bar,
                                                                           event[hit].time_raw);
        }
    }

    LOG(DEBUG) << "Used up hits in this event:";
    for (Int_t a = 0; a < event.size(); a++)
    {
        LOG(DEBUG) << "Event " << a << " " << tArrU[a] << " ";
        if (tArrU[a] != true)
            LOG(FATAL) << "Not all events analyzed!";
    }

    LOG(DEBUG) << "------------------------------------------------------\n";
    fnEvents++;
}

void R3BTofDCal2Hit::CreateHistograms(Int_t iPlane, Int_t iBar)
{
    Double_t max_charge = 80.;
    // create histograms if not already existing
    if (NULL == fhNoTpat)
    {
        fhNoTpat = new TH1F("NoTpat", "NoTpat", 200, 0, 200);
        fhNoTpat->GetXaxis()->SetTitle("No Tpat event dist");
    }

    if (NULL == fhTsync[iPlane - 1])
    {
        char strName[255];
        sprintf(strName, "Time_Sync_Plane_%d", iPlane);
        fhTsync[iPlane - 1] = new TH2F(strName, "", 50, 0, 50, 10000, -2000., 2000.);
        fhTsync[iPlane - 1]->GetXaxis()->SetTitle("Bar #");
        fhTsync[iPlane - 1]->GetYaxis()->SetTitle("THit in ns");
    }

    if (NULL == fhTdiff[iPlane - 1])
    {
        char strName1[255];
        char strName2[255];
        sprintf(strName1, "Time_Diff_Plane_%d", iPlane);
        sprintf(strName2, "Time Diff Plane %d", iPlane);
        fhTdiff[iPlane - 1] = new TH2F(strName1, strName2, 50, 0, 50, 400, -8., 8.);
        fhTdiff[iPlane - 1]->GetXaxis()->SetTitle("Bar #");
        fhTdiff[iPlane - 1]->GetYaxis()->SetTitle("Time difference (PM1 - PM2) in ns");
    }

    if (NULL == fhTvsQ[iPlane - 1])
    {
        char strName[255];
        char strName2[255];
        sprintf(strName, "TvsQ_Plane_%d", iPlane);
        sprintf(strName2, "Time Hit vs Q per event Plane %d", iPlane);
        fhTvsQ[iPlane - 1] = new TH2F(strName, "", 10000, 0, 1000, 1000, 0., max_charge);
        fhTvsQ[iPlane - 1]->GetXaxis()->SetTitle("Time between hits in ns");
        fhTvsQ[iPlane - 1]->GetYaxis()->SetTitle("Charge");
    }

    if (NULL == fhQvsPos[iPlane - 1][iBar - 1])
    {
        char strName[255];
        sprintf(strName, "Q_vs_Pos_Plane_%d_Bar_%d", iPlane, iBar);
        fhQvsPos[iPlane - 1][iBar - 1] = new TH2F(strName, "", 400, -100, 100, max_charge * 10, 0., max_charge);
        fhQvsPos[iPlane - 1][iBar - 1]->GetYaxis()->SetTitle("Charge");
        fhQvsPos[iPlane - 1][iBar - 1]->GetXaxis()->SetTitle("Position in cm");
    }
    /*
    if (NULL == fhQvsTHit[iPlane - 1][iBar - 1])
    {
        char strName[255];
        sprintf(strName, "Q_vs_THit_Plane_%d_Bar_%d", iPlane, iBar);
        fhQvsTHit[iPlane - 1][iBar - 1] = new TH2F(strName, "", 1000, 0., max_charge, 1000, -10, 40);
        fhQvsTHit[iPlane - 1][iBar - 1]->GetYaxis()->SetTitle("THit in ns");
        fhQvsTHit[iPlane - 1][iBar - 1]->GetXaxis()->SetTitle("Charge");
    }
    */
    /*
    if (NULL == fhTvsTHit[iPlane - 1][iBar - 1])
    {
        char strName[255];
        sprintf(strName, "T_vs_ToF_Plane_%d_Bar_%d", iPlane, iBar);
        fhTvsTHit[iPlane - 1][iBar - 1] = new TH2F(strName, "", 625, -25, 25, 1000, -10, 40);
        fhTvsTHit[iPlane - 1][iBar - 1]->GetYaxis()->SetTitle("THit in ns");
        fhTvsTHit[iPlane - 1][iBar - 1]->GetXaxis()->SetTitle("T1-T2 in ns");
    }
    */
    if (NULL == fhQ[iPlane - 1])
    {
        char strName1[255];
        sprintf(strName1, "Q_Plane_%d", iPlane);
        char strName2[255];
        sprintf(strName2, "Q Plane %d ", iPlane);
        fhQ[iPlane - 1] = new TH2F(strName1, strName2, 44, 0, 44, max_charge * 10, 0., max_charge);
        fhQ[iPlane - 1]->GetYaxis()->SetTitle("Charge");
        fhQ[iPlane - 1]->GetXaxis()->SetTitle("Bar #");
    }
    /*
    if (NULL == fhxy[iPlane - 1])
    {
        char strName1[255];
        sprintf(strName1, "xy_Plane_%d", iPlane);
        char strName2[255];
        sprintf(strName2, "xy of Plane %d ", iPlane);
        fhxy[iPlane - 1] = new TH2F(strName1, strName2, 320, -80, 80, 400, -100., 100.);
        fhxy[iPlane - 1]->GetYaxis()->SetTitle("y-position in cm");
        fhxy[iPlane - 1]->GetXaxis()->SetTitle("x-position in cm");
    }
    */
    if (NULL == fhxy[iPlane - 1])
    {
        char strName1[255];
        sprintf(strName1, "xy_Plane_%d", iPlane);
        char strName2[255];
        sprintf(strName2, "xy of Plane %d ", iPlane);
        fhxy[iPlane - 1] = new TH2F(strName1, strName2, 46, 0, 46, 400, -100., 100.);
        fhxy[iPlane - 1]->GetYaxis()->SetTitle("y-position in cm");
        fhxy[iPlane - 1]->GetXaxis()->SetTitle("Bar #");
    }

    if (NULL == fhQvsEvent[iPlane - 1])
    {
        char strName1[255];
        sprintf(strName1, "QvsEvent_Plane_%d", iPlane);
        char strName2[255];
        sprintf(strName2, "Charge vs Event # Plane %d ", iPlane);
        fhQvsEvent[iPlane - 1] = new TH2F(strName1, strName2, 2e5, 0, 2e9, max_charge * 10, 0., max_charge);
        fhQvsEvent[iPlane - 1]->GetYaxis()->SetTitle("Charge");
        fhQvsEvent[iPlane - 1]->GetXaxis()->SetTitle("Event #");
    }
    if (NULL == fhQ0Qt[iPlane - 1])
    {
        char strName1[255];
        sprintf(strName1, "QvsQt0_Plane_%d", iPlane);
        char strName2[255];
        sprintf(strName2, "Q vs Q_time0 Plane %d ", iPlane);
        fhQ0Qt[iPlane - 1] =
            new TH2F(strName1, strName2, max_charge * 10, 0., max_charge, max_charge * 10, 0., max_charge);
        fhQ0Qt[iPlane - 1]->GetYaxis()->SetTitle("Charge particle i");
        fhQ0Qt[iPlane - 1]->GetXaxis()->SetTitle("Charge first particle");
    }
}
void R3BTofDCal2Hit::FinishEvent()
{
    if (fHitItems)
    {
        fHitItems->Clear();
        fNofHitItems = 0;
    }
}

void R3BTofDCal2Hit::FinishTask()
{
    if (fTofdHisto)
    {
        //   if (fhTpat)
        //     fhTpat->Write();
        if (fhNoTpat)
            fhNoTpat->Write();
        for (Int_t i = 0; i < fNofPlanes; i++)
        {
            if (fhQ[i])
                fhQ[i]->Write();
            if (fhxy[i])
                fhxy[i]->Write();
            if (fhQvsEvent[i])
                fhQvsEvent[i]->Write();
            if (fhTdiff[i])
                fhTdiff[i]->Write();
            if (fhTsync[i])
                fhTsync[i]->Write();
            if (fhQ0Qt[i])
                fhQ0Qt[i]->Write();
            if (fhTvsQ[i])
                fhTvsQ[i]->Write();
            for (Int_t j = 0; j < N_TOFD_HIT_PADDLE_MAX; j++)
            {

                // control histogram time particles
                if (fhQvsPos[i][j])
                    fhQvsPos[i][j]->Write();
                /*
                if (fhQvsTHit[i][j])
                    fhQvsTHit[i][j]->Write();
                */
                /*
                if (fhTvsTHit[i][j])
                    fhTvsTHit[i][j]->Write();
                */
            }
        }
    }

    std::stringstream sprint;

    sprint << "\n\nSome statistics:\n";
    sprint << "Total number of events in tree    " << maxevent << "\n";
    sprint << "Max Event analyzed                " << fnEvents + wrongtrigger + wrongtpat + notpat << "\n";
    sprint << "Events skipped due to trigger     " << wrongtrigger << "\n";
    sprint << "Events skipped due to tpat        " << wrongtpat << "\n";
    sprint << "Events with no tpat               " << notpat << "\n";
    sprint << "Events with correct header&tpat   " << headertpat << "\n";
    sprint << "Events without ToFd hits          " << events_wo_tofd_hits << "\n";
    sprint << "Events in cal level               " << events_in_cal_level << " leading and trailing edges\n";
    sprint << "Hits in bar coincidence           " << inbarcoincidence;
    sprint << " leading and trailing edges glued together\n";
    sprint << "Could not match                   " << events_in_cal_level - inbarcoincidence << " edges\n";
    sprint << "Bars with multihits               " << bars_with_multihit << "\n";
    sprint << "# Multihits in planes and bars    " << multihit << "\n";
    sprint << "Events stored                     " << eventstore << " <-> " << inbarcoincidence;
    sprint << " (Events in bar coincidence)\n";
    sprint << "Events in single planes           " << singlehit;
    sprint << "\n";
    //<< "Good events in total            " << eventstore << " <-> " << singlehit << " = singlehit \n";
    sprint << "Really good events                " << goodpair4 << " 2 particles 2 times in 2 planes \n";
    sprint << "Good events                       " << goodpair1 << " 2 particles in 1 plane \n";
    sprint << "Good events                       " << goodpair2 << " 2 particles in different planes \n";
    sprint << "Good events                       " << goodpair5 << " 2 particles at least once in 2 planes\n";
    sprint << "Good events                       " << goodpair7 << " 3 in 2 planes\n";
    sprint << "Good events                       " << goodpair3 << " 2 particles in 2 planes odd\n";
    sprint << "Good events                       " << goodpair6 << " more than 2 particles in 2 planes\n";
    sprint << "Good events in total              " << goodpair << "\n";

    sprint << "n1=" << n1 << " n2=" << n2;

    R3BLOG(INFO, sprint.str());
}

Double_t R3BTofDCal2Hit::walk(Double_t Q,
                              Double_t par1,
                              Double_t par2,
                              Double_t par3,
                              Double_t par4,
                              Double_t par5) // new method
{
    Double_t y = 0;
    y = -30.2 + par1 * TMath::Power(Q, par2) + par3 / Q + par4 * Q + par5 * Q * Q;
    return y;
}

ClassImp(R3BTofDCal2Hit);
