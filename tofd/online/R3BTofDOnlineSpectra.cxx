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
// -----               R3BTofDOnlineSpectra               -----
// -----    Created 18/03/22 by J.L. Rodriguez-Sanchez    -----
// -----           Fill TofD online histograms            -----
// ------------------------------------------------------------

// ROOT headers
#include "TCanvas.h"
#include "TClonesArray.h"
#include "TFolder.h"
#include "TGaxis.h"
#include "TH1F.h"
#include "TH2F.h"
#include "THttpServer.h"
#include "TMath.h"
#include "TVector3.h"

// FAIR headers
#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunOnline.h"
#include "FairRuntimeDb.h"

// R3B headers
#include "R3BEventHeader.h"
#include "R3BLogger.h"
#include "R3BLosCalData.h"
#include "R3BTCalEngine.h"
#include "R3BTofDOnlineSpectra.h"
#include "R3BTofdCalData.h"
#include "R3BTofdHitData.h"
#include "R3BTofdMappedData.h"

#define IS_NAN(x) TMath::IsNaN(x)
using namespace std;

namespace
{
    double c_range_ns = 2048 * 5;
    double c_bar_coincidence_ns = 15; // nanoseconds.
    uint64_t n1, n2;
} // namespace

// R3BTofDOnlineSpectra::Default Constructor --------------------------
R3BTofDOnlineSpectra::R3BTofDOnlineSpectra()
    : R3BTofDOnlineSpectra("TofdOnlineSpectra", 1)
{
}

// R3BTofDOnlineSpectra::Standard Constructor --------------------------
R3BTofDOnlineSpectra::R3BTofDOnlineSpectra(const TString& name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fTrigger(-1)
    , fTpat1(-1)
    , fTpat2(-1)
    , fCalTriggerItems(NULL)
    , fNofPlanes(N_PLANE_MAX_TOFD_S494)
    , fPaddlesPerPlane(N_PADDLE_MAX_TOFD_S494)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
    , fNEvents(0)
    , fMappedItems(NULL)
    , fCalItems(NULL)
    , fHitItems(NULL)
    , fLosTriggerCalDataItems(NULL)
    , fLosCalDataItems(NULL)
{
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
}

// Virtual R3BTofDOnlineSpectra::Destructor
R3BTofDOnlineSpectra::~R3BTofDOnlineSpectra()
{
    R3BLOG(DEBUG1, "Destructor");
    if (fMappedItems)
        delete fMappedItems;
    if (fCalItems)
        delete fCalItems;
    if (fHitItems)
        delete fHitItems;
}

InitStatus R3BTofDOnlineSpectra::Init()
{
    R3BLOG(INFO, "");
    FairRootManager* mgr = FairRootManager::Instance();
    R3BLOG_IF(FATAL, NULL == mgr, "FairRootManager not found");

    header = (R3BEventHeader*)mgr->GetObject("EventHeader.");
    if (!header)
    {
        R3BLOG(WARNING, "EventHeader. not found");
        header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");
    }
    else
        R3BLOG(INFO, "EventHeader. found");

    FairRunOnline* run = FairRunOnline::Instance();
    run->GetHttpServer()->Register("", this);

    fCalTriggerItems = (TClonesArray*)mgr->GetObject("TofdTriggerCal");
    R3BLOG_IF(WARNING, NULL == fCalTriggerItems, "TofdTriggerCal not found");

    fMappedItems = (TClonesArray*)mgr->GetObject("TofdMapped");
    R3BLOG_IF(FATAL, NULL == fMappedItems, "TofdMapped not found");

    fCalItems = (TClonesArray*)mgr->GetObject("TofdCal");
    R3BLOG_IF(WARNING, NULL == fCalItems, "TofdCal not found");

    fHitItems = (TClonesArray*)mgr->GetObject("TofdHit");
    R3BLOG_IF(WARNING, NULL == fHitItems, "TofdHit not found");

    fLosTriggerCalDataItems = (TClonesArray*)mgr->GetObject("LosTriggerCal");
    R3BLOG_IF(WARNING, NULL == fLosTriggerCalDataItems, "TofdHit not found");

    fLosCalDataItems = (TClonesArray*)mgr->GetObject("LosCal");
    R3BLOG_IF(WARNING, NULL == fLosCalDataItems, "TofdHit not found");

    // MAIN FOLDER-Twim-Foot
    TFolder* maintofd = new TFolder("TofD", "TofD info");

    //------------------------------------------------------------------------
    // create histograms of all detectors
    //------------------------------------------------------------------------
    // TofD detector

    if (fMappedItems)
    {
        TCanvas* cTofd_planes = new TCanvas("TofD_planes_Cal", "TOFD planes CAL data", 10, 10, 1100, 1000);
        cTofd_planes->Divide(6, 4);

        for (Int_t j = 0; j < 4; j++)
        {
            char strName1[255];
            sprintf(strName1, "tofd_channels_plane_%d", j + 1);
            char strName2[255];
            sprintf(strName2, "Tofd channels plane %d", j + 1);
            fh_tofd_channels[j] = new TH1F(strName1, strName2, 90, -45., 45.);
            fh_tofd_channels[j]->GetXaxis()->SetTitle("Channel");
            fh_tofd_channels[j]->GetYaxis()->SetTitle("Counts");
            fh_tofd_channels[j]->GetYaxis()->SetTitleOffset(1.);
            fh_tofd_channels[j]->GetXaxis()->CenterTitle(true);
            fh_tofd_channels[j]->GetYaxis()->CenterTitle(true);
            fh_tofd_channels[j]->GetXaxis()->SetLabelSize(0.045);
            fh_tofd_channels[j]->GetXaxis()->SetTitleSize(0.045);
            fh_tofd_channels[j]->GetYaxis()->SetLabelSize(0.045);
            fh_tofd_channels[j]->GetYaxis()->SetTitleSize(0.045);

            char strName3[255];
            sprintf(strName3, "tofd_ToT_plane_%d", j + 1);
            char strName4[255];
            sprintf(strName4, "Tofd ToT plane %d", j + 1);
            fh_tofd_TotPm[j] = new TH2F(strName3, strName4, 90, -45, 45, 900, 0., 300.);
            fh_tofd_TotPm[j]->GetXaxis()->SetTitle("Bar number");
            fh_tofd_TotPm[j]->GetYaxis()->SetTitle("ToT / ns");
            fh_tofd_TotPm[j]->GetYaxis()->SetTitleOffset(1.1);
            fh_tofd_TotPm[j]->GetXaxis()->CenterTitle(true);
            fh_tofd_TotPm[j]->GetYaxis()->CenterTitle(true);
            fh_tofd_TotPm[j]->GetXaxis()->SetLabelSize(0.045);
            fh_tofd_TotPm[j]->GetXaxis()->SetTitleSize(0.045);
            fh_tofd_TotPm[j]->GetYaxis()->SetLabelSize(0.045);
            fh_tofd_TotPm[j]->GetYaxis()->SetTitleSize(0.045);

            char strName5[255];
            sprintf(strName5, "tofd_ToT_coinc_plane_%d", j + 1);
            char strName6[255];
            sprintf(strName6, "Tofd ToT coinc plane %d", j + 1);
            fh_tofd_TotPm_coinc[j] = new TH2F(strName5, strName6, 90, -45, 45, 900, 0., 300.);
            fh_tofd_TotPm_coinc[j]->GetXaxis()->SetTitle("Bar number");
            fh_tofd_TotPm_coinc[j]->GetYaxis()->SetTitle("ToT / ns");
            fh_tofd_TotPm_coinc[j]->GetYaxis()->SetTitleOffset(1.1);
            fh_tofd_TotPm_coinc[j]->GetXaxis()->CenterTitle(true);
            fh_tofd_TotPm_coinc[j]->GetYaxis()->CenterTitle(true);
            fh_tofd_TotPm_coinc[j]->GetXaxis()->SetLabelSize(0.045);
            fh_tofd_TotPm_coinc[j]->GetXaxis()->SetTitleSize(0.045);
            fh_tofd_TotPm_coinc[j]->GetYaxis()->SetLabelSize(0.045);
            fh_tofd_TotPm_coinc[j]->GetYaxis()->SetTitleSize(0.045);

            char strName7[255];
            sprintf(strName7, "tofd_multihit_plane_%d", j + 1);
            char strName8[255];
            sprintf(strName8, "Tofd multihit plane %d", j + 1);
            fh_tofd_multihit[j] = new TH2F(strName7, strName8, 90, -45., 45., 30, 0, 30);
            fh_tofd_multihit[j]->GetXaxis()->SetTitle("Bar number");
            fh_tofd_multihit[j]->GetYaxis()->SetTitle("Multihit");
            fh_tofd_multihit[j]->GetYaxis()->SetTitleOffset(1.);
            fh_tofd_multihit[j]->GetXaxis()->CenterTitle(true);
            fh_tofd_multihit[j]->GetYaxis()->CenterTitle(true);
            fh_tofd_multihit[j]->GetXaxis()->SetLabelSize(0.045);
            fh_tofd_multihit[j]->GetXaxis()->SetTitleSize(0.045);
            fh_tofd_multihit[j]->GetYaxis()->SetLabelSize(0.045);
            fh_tofd_multihit[j]->GetYaxis()->SetTitleSize(0.045);

            char strName9[255];
            sprintf(strName9, "tofd_multihit_coinc_plane_%d", j + 1);
            char strName10[255];
            sprintf(strName10, "Tofd multihit coinc plane %d", j + 1);
            fh_tofd_multihit_coinc[j] = new TH2F(strName9, strName10, 45, 0., 45., 30, 0, 30);
            fh_tofd_multihit_coinc[j]->GetXaxis()->SetTitle("Bar number");
            fh_tofd_multihit_coinc[j]->GetYaxis()->SetTitle("Multihit");
            fh_tofd_multihit_coinc[j]->GetYaxis()->SetTitleOffset(1.);
            fh_tofd_multihit_coinc[j]->GetXaxis()->CenterTitle(true);
            fh_tofd_multihit_coinc[j]->GetYaxis()->CenterTitle(true);
            fh_tofd_multihit_coinc[j]->GetXaxis()->SetLabelSize(0.045);
            fh_tofd_multihit_coinc[j]->GetXaxis()->SetTitleSize(0.045);
            fh_tofd_multihit_coinc[j]->GetYaxis()->SetLabelSize(0.045);
            fh_tofd_multihit_coinc[j]->GetYaxis()->SetTitleSize(0.045);

            if (j < 3)
            {
                int jk = j + 1;
                char strName11[255];
                sprintf(strName11, "tofd_ToF_plane_%d_%d", jk, jk + 1);
                char strName12[255];
                sprintf(strName12, "Tofd ToF plane %d and %d ", jk, jk + 1);
                fh_tofd_dt[j] = new TH2F(strName11, strName12, 50, 0, 50, 4000, -2000., 2000);
                fh_tofd_dt[j]->GetXaxis()->SetTitle("Bar number");
                fh_tofd_dt[j]->GetYaxis()->SetTitle("dt / ns");
                fh_tofd_dt[j]->GetYaxis()->SetTitleOffset(1.);
                fh_tofd_dt[j]->GetXaxis()->CenterTitle(true);
                fh_tofd_dt[j]->GetYaxis()->CenterTitle(true);
                fh_tofd_dt[j]->GetXaxis()->SetLabelSize(0.045);
                fh_tofd_dt[j]->GetXaxis()->SetTitleSize(0.045);
                fh_tofd_dt[j]->GetYaxis()->SetLabelSize(0.045);
                fh_tofd_dt[j]->GetYaxis()->SetTitleSize(0.045);
            }

            char strName13[255];
            sprintf(strName13, "tofd_numHits_top_vs_bottom_%d", j + 1);
            char strName14[255];
            sprintf(strName14, "Tofd numHitsMapped top vs bottom %d", j + 1);
            fh_num_side[j] = new TH2F(strName13, strName14, 45, 0., 45., 45, 0, 45);
            fh_num_side[j]->GetXaxis()->SetTitle("Num hits up");
            fh_num_side[j]->GetYaxis()->SetTitle("Num hits bottom");
            fh_num_side[j]->GetYaxis()->SetTitleOffset(1.);
            fh_num_side[j]->GetXaxis()->CenterTitle(true);
            fh_num_side[j]->GetYaxis()->CenterTitle(true);
            fh_num_side[j]->GetXaxis()->SetLabelSize(0.045);
            fh_num_side[j]->GetXaxis()->SetTitleSize(0.045);
            fh_num_side[j]->GetYaxis()->SetLabelSize(0.045);
            fh_num_side[j]->GetYaxis()->SetTitleSize(0.045);
        }

        cTofd_planes->cd(1);
        fh_tofd_channels[0]->Draw();
        cTofd_planes->cd(2);
        gPad->SetLogz();
        fh_tofd_TotPm[0]->Draw("colz");
        cTofd_planes->cd(3);
        gPad->SetLogz();
        fh_tofd_multihit[0]->SetAxisRange(1, 30, "Y");
        fh_tofd_multihit[0]->Draw("colz");
        cTofd_planes->cd(4);
        gPad->SetLogz();
        fh_tofd_TotPm_coinc[0]->Draw("colz");
        cTofd_planes->cd(5);
        gPad->SetLogz();
        fh_tofd_multihit_coinc[0]->SetAxisRange(1, 30, "Y");
        fh_tofd_multihit_coinc[0]->Draw("colz");

        cTofd_planes->cd(7);
        fh_tofd_channels[1]->Draw();
        cTofd_planes->cd(8);
        gPad->SetLogz();
        fh_tofd_TotPm[1]->Draw("colz");
        cTofd_planes->cd(9);
        gPad->SetLogz();
        fh_tofd_multihit[1]->SetAxisRange(1, 30, "Y");
        fh_tofd_multihit[1]->Draw("colz");
        cTofd_planes->cd(10);
        gPad->SetLogz();
        fh_tofd_TotPm_coinc[1]->Draw("colz");
        cTofd_planes->cd(11);
        gPad->SetLogz();
        fh_tofd_multihit_coinc[1]->SetAxisRange(1, 30, "Y");
        fh_tofd_multihit_coinc[1]->Draw("colz");
        cTofd_planes->cd(12);
        gPad->SetLogz();
        fh_tofd_dt[0]->Draw("colz");

        cTofd_planes->cd(13);
        fh_tofd_channels[2]->Draw();
        cTofd_planes->cd(14);
        gPad->SetLogz();
        fh_tofd_TotPm[2]->Draw("colz");
        cTofd_planes->cd(15);
        gPad->SetLogz();
        fh_tofd_multihit[2]->SetAxisRange(1, 30, "Y");
        fh_tofd_multihit[2]->Draw("colz");
        cTofd_planes->cd(16);
        gPad->SetLogz();
        fh_tofd_TotPm_coinc[2]->Draw("colz");
        cTofd_planes->cd(17);
        gPad->SetLogz();
        fh_tofd_multihit_coinc[2]->SetAxisRange(1, 30, "Y");
        fh_tofd_multihit_coinc[2]->Draw("colz");
        cTofd_planes->cd(18);
        gPad->SetLogz();
        fh_tofd_dt[1]->Draw("colz");

        cTofd_planes->cd(19);
        fh_tofd_channels[3]->Draw();
        cTofd_planes->cd(20);
        gPad->SetLogz();
        fh_tofd_TotPm[3]->Draw("colz");
        cTofd_planes->cd(21);
        gPad->SetLogz();
        fh_tofd_multihit[3]->SetAxisRange(1, 30, "Y");
        fh_tofd_multihit[3]->Draw("colz");
        cTofd_planes->cd(22);
        gPad->SetLogz();
        fh_tofd_TotPm_coinc[3]->Draw("colz");
        cTofd_planes->cd(23);
        gPad->SetLogz();
        fh_tofd_multihit_coinc[3]->SetAxisRange(1, 30, "Y");
        fh_tofd_multihit_coinc[3]->Draw("colz");
        cTofd_planes->cd(24);
        gPad->SetLogz();
        fh_tofd_dt[2]->Draw("colz");

        // Adding this canvas to the main folder
        maintofd->Add(cTofd_planes);
    }

    if (fHitItems)
    {
        TCanvas* cTofd_planes_hit = new TCanvas("TofD_planes_Hit", "TOFD planes HIT data", 20, 20, 1120, 1020);
        cTofd_planes_hit->Divide(5, 4);

        for (Int_t j = 0; j < 4; j++)
        {
            char strName3[255];
            sprintf(strName3, "tofd_hit_Q_plane_%d", j + 1);
            char strName4[255];
            sprintf(strName4, "Tofd hit Charge plane %d", j + 1);
            fh_tofd_Tot_hit[j] = new TH2F(strName3, strName4, 45, 0, 45, 1000, 0., 40.);
            fh_tofd_Tot_hit[j]->GetXaxis()->SetTitle("BarId");
            fh_tofd_Tot_hit[j]->GetYaxis()->SetTitle("Charge");
            fh_tofd_Tot_hit[j]->GetYaxis()->SetTitleOffset(1.);
            fh_tofd_Tot_hit[j]->GetXaxis()->CenterTitle(true);
            fh_tofd_Tot_hit[j]->GetYaxis()->CenterTitle(true);
            fh_tofd_Tot_hit[j]->GetXaxis()->SetLabelSize(0.045);
            fh_tofd_Tot_hit[j]->GetXaxis()->SetTitleSize(0.045);
            fh_tofd_Tot_hit[j]->GetYaxis()->SetLabelSize(0.045);
            fh_tofd_Tot_hit[j]->GetYaxis()->SetTitleSize(0.045);

            char strName7[255];
            sprintf(strName7, "tofd_hit_multihit_plane_%d", j + 1);
            char strName8[255];
            sprintf(strName8, "Tofd hit multihit plane %d", j + 1);
            fh_tofd_multihit_hit[j] = new TH1F(strName7, strName8, 100, 0, 100);
            fh_tofd_multihit_hit[j]->GetYaxis()->SetTitle("Counts");
            fh_tofd_multihit_hit[j]->GetXaxis()->SetTitle("Multihit");
            fh_tofd_multihit_hit[j]->GetYaxis()->SetTitleOffset(1.);
            fh_tofd_multihit_hit[j]->GetXaxis()->CenterTitle(true);
            fh_tofd_multihit_hit[j]->GetYaxis()->CenterTitle(true);
            fh_tofd_multihit_hit[j]->GetXaxis()->SetLabelSize(0.045);
            fh_tofd_multihit_hit[j]->GetXaxis()->SetTitleSize(0.045);
            fh_tofd_multihit_hit[j]->GetYaxis()->SetLabelSize(0.045);
            fh_tofd_multihit_hit[j]->GetYaxis()->SetTitleSize(0.045);

            char strName21[255];
            sprintf(strName21, "tofd_bars_plane_%d", j + 1);
            char strName22[255];
            sprintf(strName22, "Tofd bars plane %d", j + 1);
            fh_tofd_bars[j] = new TH1F(strName21, strName22, 45, 0., 45.);
            fh_tofd_bars[j]->GetXaxis()->SetTitle("BarId");
            fh_tofd_bars[j]->GetYaxis()->SetTitle("Counts");
            fh_tofd_bars[j]->GetYaxis()->SetTitleOffset(1.);
            fh_tofd_bars[j]->GetXaxis()->CenterTitle(true);
            fh_tofd_bars[j]->GetYaxis()->CenterTitle(true);
            fh_tofd_bars[j]->GetXaxis()->SetLabelSize(0.045);
            fh_tofd_bars[j]->GetXaxis()->SetTitleSize(0.045);
            fh_tofd_bars[j]->GetYaxis()->SetLabelSize(0.045);
            fh_tofd_bars[j]->GetYaxis()->SetTitleSize(0.045);

            char strName23[255];
            sprintf(strName23, "tofd_hit_time_plane_%d", j + 1);
            char strName24[255];
            sprintf(strName24, "Tofd hit time plane %d", j + 1);
            fh_tofd_time_hit[j] = new TH2F(strName23, strName24, 45, 0, 45, 4000, -2000., 2000.);
            fh_tofd_time_hit[j]->GetXaxis()->SetTitle("BarId");
            fh_tofd_time_hit[j]->GetYaxis()->SetTitle("time / ns");
            fh_tofd_time_hit[j]->GetYaxis()->SetTitleOffset(1.1);
            fh_tofd_time_hit[j]->GetXaxis()->CenterTitle(true);
            fh_tofd_time_hit[j]->GetYaxis()->CenterTitle(true);
            fh_tofd_time_hit[j]->GetXaxis()->SetLabelSize(0.045);
            fh_tofd_time_hit[j]->GetXaxis()->SetTitleSize(0.045);
            fh_tofd_time_hit[j]->GetYaxis()->SetLabelSize(0.045);
            fh_tofd_time_hit[j]->GetYaxis()->SetTitleSize(0.045);

            if (j < 3)
            {
                int jk = j + 1;
                char strName11[255];
                sprintf(strName11, "tofd_hit_ToF_dt_plane_%d_%d", jk, jk + 1);
                char strName12[255];
                sprintf(strName12, "Tofd hit ToF dt plane %d and %d ", jk, jk + 1);
                fh_tofd_dt_hit[j] = new TH2F(strName11, strName12, 45, 0, 45, 4000, -20., 20);
                char strName9[255];
                sprintf(strName9, "BarId plane %d ", j + 1);
                fh_tofd_dt_hit[j]->GetXaxis()->SetTitle(strName9);
                fh_tofd_dt_hit[j]->GetYaxis()->SetTitle("dt / ns");
                fh_tofd_dt_hit[j]->GetYaxis()->SetTitleOffset(1.);
                fh_tofd_dt_hit[j]->GetXaxis()->CenterTitle(true);
                fh_tofd_dt_hit[j]->GetYaxis()->CenterTitle(true);
                fh_tofd_dt_hit[j]->GetXaxis()->SetLabelSize(0.045);
                fh_tofd_dt_hit[j]->GetXaxis()->SetTitleSize(0.045);
                fh_tofd_dt_hit[j]->GetYaxis()->SetLabelSize(0.045);
                fh_tofd_dt_hit[j]->GetYaxis()->SetTitleSize(0.045);
            }
        }

        cTofd_planes_hit->cd(1);
        gPad->SetLogy();
        fh_tofd_multihit_hit[0]->Draw();
        cTofd_planes_hit->cd(3);
        gPad->SetLogz();
        fh_tofd_Tot_hit[0]->Draw("colz");
        cTofd_planes_hit->cd(2);
        gPad->SetLogy();
        fh_tofd_bars[0]->Draw();
        cTofd_planes_hit->cd(4);
        gPad->SetLogz();
        fh_tofd_time_hit[0]->Draw("colz");

        cTofd_planes_hit->cd(6);
        gPad->SetLogy();
        fh_tofd_multihit_hit[1]->Draw();
        cTofd_planes_hit->cd(8);
        gPad->SetLogz();
        fh_tofd_Tot_hit[1]->Draw("colz");
        cTofd_planes_hit->cd(7);
        gPad->SetLogy();
        fh_tofd_bars[1]->Draw();
        cTofd_planes_hit->cd(9);
        gPad->SetLogz();
        fh_tofd_time_hit[1]->Draw("colz");
        cTofd_planes_hit->cd(10);
        gPad->SetLogz();
        fh_tofd_dt_hit[0]->Draw("colz");

        cTofd_planes_hit->cd(11);
        gPad->SetLogy();
        fh_tofd_multihit_hit[2]->Draw();
        cTofd_planes_hit->cd(13);
        gPad->SetLogz();
        fh_tofd_Tot_hit[2]->Draw("colz");
        cTofd_planes_hit->cd(12);
        gPad->SetLogy();
        fh_tofd_bars[2]->Draw();
        cTofd_planes_hit->cd(14);
        gPad->SetLogz();
        fh_tofd_time_hit[2]->Draw("colz");
        cTofd_planes_hit->cd(15);
        gPad->SetLogz();
        fh_tofd_dt_hit[1]->Draw("colz");

        cTofd_planes_hit->cd(16);
        gPad->SetLogy();
        fh_tofd_multihit_hit[3]->Draw();
        cTofd_planes_hit->cd(18);
        gPad->SetLogz();
        fh_tofd_Tot_hit[3]->Draw("colz");
        cTofd_planes_hit->cd(17);
        gPad->SetLogy();
        fh_tofd_bars[3]->Draw();
        cTofd_planes_hit->cd(19);
        gPad->SetLogz();
        fh_tofd_time_hit[3]->Draw("colz");
        cTofd_planes_hit->cd(20);
        gPad->SetLogz();
        fh_tofd_dt_hit[2]->Draw("colz");

        // Adding this canvas to the main folder
        maintofd->Add(cTofd_planes_hit);

        auto cToFd_los_h2 = new TCanvas("ToFD time - Los time", "ToFD time - Los time", 20, 20, 1120, 1020);
        cToFd_los_h2->Divide(2, 2);
        for (Int_t i = 0; i < N_PLANE_MAX_TOFD_S494; i++)
        {
            char strNameLos_c[255];
            sprintf(strNameLos_c, "tofd-los_timediff_plane_%d", i + 1);
            fh_tofd_time_los_h2[i] = new TH2F(strNameLos_c, strNameLos_c, 45, 0, 45, 5000, -50000, 10000);
            fh_tofd_time_los_h2[i]->GetXaxis()->SetTitle("Bar");
            fh_tofd_time_los_h2[i]->GetYaxis()->SetTitle("ToF");
            cToFd_los_h2->cd(i + 1);
            fh_tofd_time_los_h2[i]->Draw("colz");

            auto cToFd_los = new TCanvas(strNameLos_c, strNameLos_c, 20, 20, 1120, 1020);
            cToFd_los->Divide(5, 9);
            for (Int_t j = 0; j < N_PADDLE_MAX_TOFD_S494; j++)
            {
                char strNameLos[255];
                sprintf(strNameLos, "tofd-los_timediff_bar_%d_plane_%d", j + 1, i + 1);
                char strNameLos2[255];
                sprintf(strNameLos2, "Tofd_time - Los_time bar %d plane %d", j + 1, i + 1);
                fh_tofd_time_los[i][j] = new TH1F(strNameLos, strNameLos2, 50, 0, 1000);
                fh_tofd_time_los[i][j]->GetXaxis()->SetTitle("Time");
                fh_tofd_time_los[i][j]->GetYaxis()->SetTitle("counts");
                cToFd_los->cd(j + 1);
                fh_tofd_time_los[i][j]->Draw("");
            }
            // Adding this canvas to the main folder
            maintofd->Add(cToFd_los);
        }
        maintofd->Add(cToFd_los_h2);
    }

    run->AddObject(maintofd);
    // Register command to reset histograms
    run->GetHttpServer()->RegisterCommand("Reset_TofD_HIST", Form("/Objects/%s/->Reset_Histo()", GetName()));

    return kSUCCESS;
}

void R3BTofDOnlineSpectra::Reset_Histo()
{
    R3BLOG(INFO, "");
    for (int i = 0; i < N_PLANE_MAX_TOFD_S494; i++)
    {
        fh_tofd_channels[i]->Reset();
        fh_tofd_multihit[i]->Reset();
        fh_tofd_TotPm[i]->Reset();
        fh_tofd_multihit_coinc[i]->Reset();
        fh_tofd_TotPm_coinc[i]->Reset();
    }
    fh_tofd_dt[0]->Reset();
    fh_tofd_dt[1]->Reset();
    fh_tofd_dt[2]->Reset();

    if (fHitItems)
    {
        for (int i = 0; i < N_PLANE_MAX_TOFD_S494; i++)
        {
            fh_tofd_Tot_hit[i]->Reset();
            fh_tofd_time_hit[i]->Reset();
            fh_tofd_multihit_hit[i]->Reset();
            fh_tofd_bars[i]->Reset();
            fh_tofd_time_los_h2[i]->Reset();
            for (Int_t j = 0; j < N_PADDLE_MAX_TOFD_S494; j++)
                fh_tofd_time_los[i][j]->Reset();
        }
        for (int i = 0; i < N_PLANE_MAX_TOFD_S494 - 1; i++)
        {
            fh_tofd_dt_hit[i]->Reset();
        }
    }
    return;
}

void R3BTofDOnlineSpectra::Exec(Option_t* option)
{
    //   check for requested trigger (Todo: should be done globablly / somewhere else)
    if ((fTrigger >= 0) && (header) && (header->GetTrigger() != fTrigger))
        return;

    // fTpat = 1-16; fTpat_bit = 0-15
    Int_t fTpat_bit1 = fTpat1 - 1;
    Int_t fTpat_bit2 = fTpat2 - 1;
    Int_t tpatbin;
    if (fTpat1 >= 0 && fTpat2 >= 0)
    {
        for (int i = 0; i < 16; i++)
        {
            tpatbin = (header->GetTpat() & (1 << i));
            if (tpatbin != 0 && (i < fTpat_bit1 || i > fTpat_bit2))
            {
                return;
            }
        }
    }

    Int_t NumPaddles[4] = { 0 };
    if (fMappedItems)
    {
        // auto det = fMappedItems.at(DET_TOFD);
        Int_t nMapped = fMappedItems->GetEntriesFast();
        Int_t iPlaneMem = 1, iBarMem = 0;
        Int_t nsum_top[N_PLANE_MAX_TOFD_S494] = { 0 };
        Int_t nsum_bot[N_PLANE_MAX_TOFD_S494] = { 0 };

        // cout << "map "<< nMapped <<endl;

        for (Int_t imapped = 0; imapped < nMapped; imapped++)
        {
            auto mapped = (R3BTofdMappedData*)fMappedItems->At(imapped);
            if (!mapped)
                continue; // should not happen

            Int_t const iPlane = mapped->GetDetectorId(); // 1..n
            Int_t const iBar = mapped->GetBarId();        // 1..n
            Int_t const iSide = mapped->GetSideId();      // 1..n
            Int_t const iEdge = mapped->GetEdgeId();

            if (iSide == 1 && iEdge == 1 && iPlane < N_PLANE_MAX_TOFD_S494)
                nsum_bot[iPlane - 1] += 1;
            if (iSide == 2 && iEdge == 1 && iPlane < N_PLANE_MAX_TOFD_S494)
                nsum_top[iPlane - 1] += 1;

            if (iPlaneMem != iPlane)
                iBarMem = 0;
            if (iPlane == 1 && iBarMem != iBar && iSide == 1 && iEdge == 1)
                NumPaddles[0] += 1;
            if (iPlane == 2 && iBarMem != iBar && iSide == 1 && iEdge == 1)
                NumPaddles[1] += 1;
            if (iPlane == 3 && iBarMem != iBar && iSide == 1 && iEdge == 1)
                NumPaddles[2] += 1;
            if (iPlane == 3 && iBarMem != iBar && iSide == 1 && iEdge == 1)
                NumPaddles[3] += 1;

            // cout<<imapped<<", "<<iPlane<<"; "<<iPlaneMem<<"; "<<iBar<<"; "<<iBarMem<<", "
            // <<NumPaddles[iPlane-1]<<endl;

            iPlaneMem = iPlane;
            iBarMem = iBar;

            if (iPlane <= fNofPlanes)
            {
                if (iSide == 1) // bottom
                    fh_tofd_channels[iPlane - 1]->Fill(-iBar - 1);
                if (iSide == 2) // top
                    fh_tofd_channels[iPlane - 1]->Fill(iBar);
            }
        }
        for (Int_t i = 0; i < N_PLANE_MAX_TOFD_S494; i++)
        {
            fh_num_side[i]->Fill(nsum_bot[i], nsum_top[i]);
        }
    }

    // Reading LOS cal data
    /*    Float_t losTime=0.0;
        Float_t losTriggerTime=0.0;

        if(fLosCalDataItems && fLosCalDataItems->GetEntriesFast()>0){R3BLosCalData* losHit =
       (R3BLosCalData*)fLosCalDataItems->At(0); Int_t losChannel = losHit->GetDetector();
       // std::cout<<"LOS Time : "<<losHit->GetTimeT_ns(losChannel)<<std::endl;
        losTime = losHit->GetTimeV_ns(losChannel);}

        if(fLosTriggerCalDataItems && fLosTriggerCalDataItems->GetEntriesFast()>0){R3BLosCalData* losTriggerHit =
       (R3BLosCalData*)fLosTriggerCalDataItems->At(0); Int_t losChannelTrigger = losTriggerHit->GetDetector();
        //std::cout<<"LOS Time (Trigger) :
       "<<losTriggerHit->Ge(losHit->GetTime()-losCalTriggerHits->GetTimeL_ns(channelLos)tTimeL_ns(0)<<"
       "<<losChannelTrigger<<std::endl; losTriggerTime = losTriggerHit->GetTimeL_ns(0);}
    */

    if (fCalItems)
    {

        UInt_t vmultihits[N_PLANE_MAX_TOFD_S494 + 1][N_PADDLE_MAX_TOFD_S494];
        UInt_t vmultihits_top[N_PLANE_MAX_TOFD_S494 + 1][N_PADDLE_MAX_TOFD_S494];
        UInt_t vmultihits_bot[N_PLANE_MAX_TOFD_S494 + 1][N_PADDLE_MAX_TOFD_S494];
        Double_t time_bar[N_PLANE_MAX_TOFD_S494 + 1][N_PADDLE_MAX_TOFD_S494][100];
        for (Int_t i = 0; i < fNofPlanes + 1; i++)
        {
            for (Int_t j = 0; j < N_PADDLE_MAX_TOFD_S494; j++)
            {
                vmultihits[i][j] = 0;
                vmultihits_top[i][j] = 0;
                vmultihits_bot[i][j] = 0;
                for (Int_t l = 0; l < 100; l++)
                {
                    time_bar[i][j][l] = 0. / 0.;
                }
            }
        }

        //    std::cout<<"new event!*************************************\n";
        Int_t nHits = fCalItems->GetEntriesFast();

        Int_t nHitsEvent = 0;
        // Organize cals into bars.
        struct Entry
        {
            std::vector<R3BTofdCalData*> top;
            std::vector<R3BTofdCalData*> bot;
        };

        std::map<size_t, Entry> bar_map;
        //   puts("Event");
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            auto* hit = (R3BTofdCalData*)fCalItems->At(ihit);
            size_t idx = hit->GetDetectorId() * fPaddlesPerPlane * hit->GetBarId();

            auto ret = bar_map.insert(std::pair<size_t, Entry>(idx, Entry()));

            // hit->GetSideId() gives 1 for bottom and 2 for top
            auto& vec = 1 == hit->GetSideId() ? ret.first->second.bot : ret.first->second.top;
            vec.push_back(hit);
        }

        // Build trigger map.
        std::vector<R3BTofdCalData*> trig_map;
        for (int i = 0; i < fCalTriggerItems->GetEntries(); ++i)
        {
            auto trig = (R3BTofdCalData*)fCalTriggerItems->At(i);
            if (trig_map.size() < trig->GetBarId())
            {
                trig_map.resize(trig->GetBarId());
            }
            trig_map.at(trig->GetBarId() - 1) = trig;
        }

        // Without coincidences
        bool s_was_trig_missing = false;

        for (auto it = bar_map.begin(); bar_map.end() != it; ++it)
        {
            auto const& top_vec = it->second.top;
            size_t top_i = 0;
            auto const& bot_vec = it->second.bot;
            size_t bot_i = 0;
            for (; top_i < top_vec.size();)
            {
                auto top = top_vec.at(top_i);
                auto top_trig_i = g_tofd_trig_map[top->GetDetectorId() - 1][top->GetSideId() - 1][top->GetBarId() - 1];

                Double_t top_trig_ns = 0;
                if (top_trig_i < trig_map.size() && trig_map.at(top_trig_i))
                {
                    auto top_trig = trig_map.at(top_trig_i);
                    top_trig_ns = top_trig->GetTimeLeading_ns();
                }
                else
                {
                    if (!s_was_trig_missing)
                    {
                        LOG(ERROR) << "R3BOnlineSpectraToFD::Exec() : Missing trigger information!";
                        s_was_trig_missing = true;
                    }
                }

                Int_t iPlane = top->GetDetectorId(); // 1..n
                Int_t iBar = top->GetBarId();        // 1..n
                if (iPlane > fNofPlanes)             // this also errors for iDetector==0
                {
                    LOG(ERROR) << "R3BTOnlineSpectraToFD::Exec() : more detectors than expected! Det: " << iPlane
                               << " allowed are 1.." << fNofPlanes;
                    continue;
                }
                if (iBar > fPaddlesPerPlane) // same here
                {
                    LOG(ERROR) << "R3BTOnlineSpectraToFD::Exec() : more bars then expected! Det: " << iBar
                               << " allowed are 1.." << fPaddlesPerPlane;
                    continue;
                }

                auto top_tot = fmod(top->GetTimeTrailing_ns() - top->GetTimeLeading_ns() + c_range_ns, c_range_ns);
                fh_tofd_TotPm[iPlane - 1]->Fill(iBar, top_tot);
                vmultihits_top[iPlane - 1][iBar - 1] += 1;

                ++top_i;
            }

            for (; bot_i < bot_vec.size();)
            {
                auto bot = bot_vec.at(bot_i);
                auto bot_trig_i = g_tofd_trig_map[bot->GetDetectorId() - 1][bot->GetSideId() - 1][bot->GetBarId() - 1];

                Double_t bot_trig_ns = 0;
                if (bot_trig_i < trig_map.size() && trig_map.at(bot_trig_i))
                {
                    auto bot_trig = trig_map.at(bot_trig_i);
                    bot_trig_ns = bot_trig->GetTimeLeading_ns();
                }
                else
                {
                    if (!s_was_trig_missing)
                    {
                        LOG(ERROR) << "R3BOnlineSpectraToFD::Exec() : Missing trigger information!";
                        s_was_trig_missing = true;
                    }
                }

                // Shift the cyclic difference window by half a window-length and move it back,
                // this way the trigger time will be at 0.
                auto bot_ns = fmod(bot->GetTimeLeading_ns() - bot_trig_ns + c_range_ns + c_range_ns / 2, c_range_ns) -
                              c_range_ns / 2;

                Int_t iPlane = bot->GetDetectorId(); // 1..n
                Int_t iBar = bot->GetBarId();        // 1..n
                if (iPlane > fNofPlanes)             // this also errors for iDetector==0
                {
                    LOG(ERROR) << "R3BTOnlineSpectraToFD::Exec() : more detectors than expected! Det: " << iPlane
                               << " allowed are 1.." << fNofPlanes;
                    continue;
                }
                if (iBar > fPaddlesPerPlane) // same here
                {
                    LOG(ERROR) << "R3BTOnlineSpectraToFD::Exec() : more bars then expected! Det: " << iBar
                               << " allowed are 1.." << fPaddlesPerPlane;
                    continue;
                }

                auto bot_tot = fmod(bot->GetTimeTrailing_ns() - bot->GetTimeLeading_ns() + c_range_ns, c_range_ns);

                fh_tofd_TotPm[iPlane - 1]->Fill(-iBar - 1, bot_tot);

                // register multi hits
                vmultihits_bot[iPlane - 1][iBar - 1] += 1;

                ++bot_i;
            }
        }

        for (Int_t ipl = 0; ipl < N_PLANE_MAX_TOFD_S494; ipl++)
        {
            for (Int_t ibr = 1; ibr < N_PADDLE_MAX_TOFD_S494 + 1; ibr++)
            {
                fh_tofd_multihit[ipl]->Fill(-ibr - 1, vmultihits_bot[ipl][ibr - 1]);
                fh_tofd_multihit[ipl]->Fill(ibr, vmultihits_top[ipl][ibr - 1]);
            }
        }

        // With coincidences:
        bool s_was_trig_missingc = false;
        for (auto it = bar_map.begin(); bar_map.end() != it; ++it)
        {
            auto const& topc_vec = it->second.top;
            auto const& botc_vec = it->second.bot;
            size_t topc_i = 0;
            size_t botc_i = 0;
            for (; topc_i < topc_vec.size() && botc_i < botc_vec.size();)
            {
                auto topc = topc_vec.at(topc_i);
                auto botc = botc_vec.at(botc_i);
                auto topc_trig_i =
                    g_tofd_trig_map[topc->GetDetectorId() - 1][topc->GetSideId() - 1][topc->GetBarId() - 1];
                auto botc_trig_i =
                    g_tofd_trig_map[botc->GetDetectorId() - 1][botc->GetSideId() - 1][botc->GetBarId() - 1];

                Double_t topc_trig_ns = 0, botc_trig_ns = 0;
                if (topc_trig_i < trig_map.size() && trig_map.at(topc_trig_i) && botc_trig_i < trig_map.size() &&
                    trig_map.at(botc_trig_i))
                {
                    auto topc_trig = trig_map.at(topc_trig_i);
                    auto botc_trig = trig_map.at(botc_trig_i);
                    topc_trig_ns = topc_trig->GetTimeLeading_ns();
                    botc_trig_ns = botc_trig->GetTimeLeading_ns();
                    ++n1;
                }
                else
                {
                    if (!s_was_trig_missingc)
                    {
                        LOG(ERROR) << "R3BOnlineSpectraToFD::Exec() : Missing trigger information!";
                        s_was_trig_missingc = true;
                    }
                    ++n2;
                }

                // Shift the cyclic difference window by half a window-length and move it back,
                // this way the trigger time will be at 0.
                auto topc_ns =
                    fmod(topc->GetTimeLeading_ns() - topc_trig_ns + c_range_ns + c_range_ns / 2, c_range_ns) -
                    c_range_ns / 2;
                auto botc_ns =
                    fmod(botc->GetTimeLeading_ns() - botc_trig_ns + c_range_ns + c_range_ns / 2, c_range_ns) -
                    c_range_ns / 2;

                auto dt = topc_ns - botc_ns;
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

                if (std::abs(dt_mod) < c_bar_coincidence_ns)
                {
                    // Hit!
                    // std::cout << "Hit!\n";
                    Int_t iPlane = topc->GetDetectorId(); // 1..n
                    Int_t iBar = topc->GetBarId();        // 1..n
                    if (iPlane > fNofPlanes)              // this also errors for iDetector==0
                    {
                        LOG(ERROR) << "R3BTOnlineSpectraToFD::Exec() : more detectors than expected! Det: " << iPlane
                                   << " allowed are 1.." << fNofPlanes;
                        continue;
                    }
                    if (iBar > fPaddlesPerPlane) // same here
                    {
                        LOG(ERROR) << "R3BTOnlineSpectraToFD::Exec() : more bars then expected! Det: " << iBar
                                   << " allowed are 1.." << fPaddlesPerPlane;
                        continue;
                    }

                    auto topc_tot =
                        fmod(topc->GetTimeTrailing_ns() - topc->GetTimeLeading_ns() + c_range_ns + c_range_ns / 2,
                             c_range_ns) -
                        c_range_ns / 2;
                    auto botc_tot =
                        fmod(botc->GetTimeTrailing_ns() - botc->GetTimeLeading_ns() + c_range_ns + c_range_ns / 2,
                             c_range_ns) -
                        c_range_ns / 2;

                    fh_tofd_TotPm_coinc[iPlane - 1]->Fill(-iBar - 1, botc_tot);
                    fh_tofd_TotPm_coinc[iPlane - 1]->Fill(iBar, topc_tot);

                    // std::cout<<"ToT: "<<top_tot << " "<<bot_tot<<"\n";

                    // register multi hits
                    Int_t imlt = vmultihits[iPlane - 1][iBar - 1];
                    time_bar[iPlane - 1][iBar - 1][imlt] = (topc_ns + botc_ns) / 2.;
                    vmultihits[iPlane - 1][iBar - 1] += 1;

                    ++topc_i;
                    ++botc_i;
                }
                else if (dt < 0 && dt > -c_range_ns / 2)
                {
                    ++topc_i;
                }
                else
                {
                    ++botc_i;
                }
            }
        }

        for (Int_t ipl = 0; ipl < N_PLANE_MAX_TOFD_S494; ipl++)
        {
            for (Int_t ibr = 1; ibr < N_PADDLE_MAX_TOFD_S494 + 1; ibr++)
            {
                fh_tofd_multihit_coinc[ipl]->Fill(ibr, vmultihits[ipl][ibr - 1]);
                if (ipl > 0)
                {
                    for (Int_t imult1 = 0; imult1 < vmultihits[ipl][ibr - 1]; imult1++)
                    {
                        for (Int_t imult2 = 0; imult2 < vmultihits[ipl - 1][ibr - 1]; imult2++)
                        {
                            Double_t tof_plane = 0. / 0.;
                            tof_plane = fmod(time_bar[ipl][ibr - 1][imult1] - time_bar[ipl - 1][ibr - 1][imult2] +
                                                 c_range_ns + c_range_ns / 2,
                                             c_range_ns) -
                                        c_range_ns / 2;
                            fh_tofd_dt[ipl - 1]->Fill(ibr, tof_plane);
                        }
                    }
                }
            }
        }

    } // endi if fCalItems

    if (fHitItems)
    {
        Int_t nHits = fHitItems->GetEntriesFast();

        if (nHits > 100)
            return;

        Double_t x[N_PLANE_MAX_TOFD_S494][10], y[N_PLANE_MAX_TOFD_S494][10], t[N_PLANE_MAX_TOFD_S494][10],
            q[N_PLANE_MAX_TOFD_S494][10], bar[N_PLANE_MAX_TOFD_S494][10];
        for (Int_t i = 0; i < N_PLANE_MAX_TOFD_S494; i++)
        {
            for (Int_t k = 0; k < 10; k++)
            {
                x[i][k] = -1000.;
                y[i][k] = -1000.;
                q[i][k] = -1000.;
                t[i][k] = 0. / 0.;
                bar[i][k] = 0;
            }
        }

        Int_t nMulti[N_PLANE_MAX_TOFD_S494], iCounts[N_PLANE_MAX_TOFD_S494];
        for (Int_t i = 0; i < N_PLANE_MAX_TOFD_S494; i++)
        {
            nMulti[i] = 0;
            iCounts[i] = 0;
        }

        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            auto hitTofd = (R3BTofdHitData*)fHitItems->At(ihit);

            if (IS_NAN(hitTofd->GetTime()))
                continue;

            Int_t iPlane = hitTofd->GetDetId();
            Double_t randx = (std::rand() / (float)RAND_MAX) - 0.5;
            Int_t ictemp = iCounts[iPlane - 1];
            x[iPlane - 1][ictemp] = hitTofd->GetX() + 2.7 * randx;
            y[iPlane - 1][ictemp] = hitTofd->GetY();
            t[iPlane - 1][ictemp] = hitTofd->GetTime();
            q[iPlane - 1][ictemp] = hitTofd->GetEloss();
            bar[iPlane - 1][ictemp] = hitTofd->GetBarId();
            fh_tofd_Tot_hit[iPlane - 1]->Fill(bar[iPlane - 1][ictemp], q[iPlane - 1][ictemp]);
            fh_tofd_time_hit[iPlane - 1]->Fill(bar[iPlane - 1][ictemp], t[iPlane - 1][ictemp]);
            fh_tofd_bars[iPlane - 1]->Fill(hitTofd->GetBarId());
            iCounts[iPlane - 1] += 1;
            nMulti[iPlane - 1] += 1;
        }

        for (Int_t i = 0; i < N_PLANE_MAX_TOFD_S494; i++)
        {
            fh_tofd_multihit_hit[i]->Fill(nMulti[i]);
            if (i > 0)
            {
                for (Int_t im1 = 0; im1 < iCounts[i]; im1++)
                {
                    for (Int_t im2 = 0; im2 < iCounts[i - 1]; im2++)
                    {
                        Double_t tdif =
                            fmod(t[i][im1] - t[i - 1][im2] + c_range_ns + c_range_ns / 2, c_range_ns) - c_range_ns / 2;
                        fh_tofd_dt_hit[i - 1]->Fill(bar[i][im1], tdif);
                    }
                }
            }
        }
    }

    fNEvents += 1;
}

void R3BTofDOnlineSpectra::FinishEvent()
{
    R3BLOG(DEBUG1, "Cleaning data structures");
    if (fMappedItems)
    {
        fMappedItems->Clear();
    }
    if (fCalItems)
    {
        fCalItems->Clear();
    }
    if (fHitItems)
    {
        fHitItems->Clear();
    }
    if (fLosCalDataItems)
    {
        fLosCalDataItems->Clear();
    }
    if (fLosTriggerCalDataItems)
    {
        fLosTriggerCalDataItems->Clear();
    }
}

ClassImp(R3BTofDOnlineSpectra);
