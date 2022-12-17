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

#include "TClonesArray.h"
#include "TMath.h"
#include "TRandom.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include <iomanip>

#include "R3BRpc.h"
#include "R3BRpcCalData.h"

#include "R3BRpcPreCal2Cal.h"

#include "R3BRpcPreCalData.h"

// R3BRpcPreCal2Cal: Constructor
R3BRpcPreCal2Cal::R3BRpcPreCal2Cal()
    : FairTask("R3B RPC Calibrator")
    , fNumChannels(0)
    , fPreCalDataCA(NULL)
    , fParCont(NULL)
    , fRpcCalDataCA(NULL)
    , fTotCalPar(NULL)
    , fOnline(kFALSE)

{
}

R3BRpcPreCal2Cal::~R3BRpcPreCal2Cal()
{
    LOG(INFO) << "R3BRpcPreCal2Cal: Delete instance";

    delete fTotCalPar;
}

InitStatus R3BRpcPreCal2Cal::Init()
{
    LOG(INFO) << "R3BRpcPreCal2Cal::Init()";

    // INPUT DATA
    FairRootManager* rootManager = FairRootManager::Instance();
    if (!rootManager)
    {
        LOG(FATAL) << "R3BRpcPreCal2Cal::FairRootManager not found";
        return kFATAL;
    }

    // Parameter Container
    // Reading RPCCalPar from FairRuntimeDb
    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    if (!rtdb)
    {
        LOG(error) << "R3BRpcPreCal2Cal:: FairRuntimeDb not opened";
    }

    fPreCalDataCA = (TClonesArray*)rootManager->GetObject("R3BRpcPreCalData");
    if (!fPreCalDataCA)
    {
        LOG(error) << "R3BRpcPreCal2CalPar::Init() fPreCalDataCA not found";
        return kFATAL;
    }

    fTotCalPar = (R3BRpcTotCalPar*)rtdb->getContainer("RpcTotCalPar");
    if (!fTotCalPar)
    {
        LOG(error) << "R3BRpcPreCal2CalPar::Init() Couldn't get handle on RpcTotCalPar container";
        return kFATAL;
    }

    // fill the TArray with Tot parameters!!!
    fParCont = fTotCalPar->GetCalParams();

    // OUTPUT DATA
    // Calibrated data
    fRpcCalDataCA = new TClonesArray("R3BRpcCalData", 50);
    rootManager->Register("R3BRpcCalData", "RPC Strip Cal", fRpcCalDataCA, !fOnline);

    return kSUCCESS;
}

InitStatus R3BRpcPreCal2Cal::ReInit() { return kSUCCESS; }

void R3BRpcPreCal2Cal::Exec(Option_t* option)
{
    // Reset entries in output arrays, local arrays
    Reset();

    // loop over strip data
    double time_R_B;
    double time_L_T;
    double tot_R_B;
    double tot_L_T;
    Int_t nHits = fPreCalDataCA->GetEntries();
    for (Int_t i = 0; i < nHits; i++)
    {
        auto map1 = (R3BRpcPreCalData*)(fPreCalDataCA->At(i));

        UInt_t iDetector = map1->GetDetId();
        if (iDetector == 2)
        {
            continue;
        }
        UInt_t inum = (iDetector * 41 + map1->GetChannelId()) * 2 + map1->GetSide() - 2;
        for (Int_t ii = i + 1; ii < nHits; ii++)
        {
            auto nxt_chn = (R3BRpcPreCalData*)fPreCalDataCA->At(ii);
            if (map1->GetChannelId() == nxt_chn->GetChannelId() && map1->GetSide() != nxt_chn->GetSide())
            {
                UInt_t nxt_inum = (iDetector * 41 + nxt_chn->GetChannelId()) * 2 + nxt_chn->GetSide() - 2;

                if (map1->GetSide() == 1)
                {
                    tot_R_B = map1->GetTot() - fParCont->GetAt(inum);
                    tot_L_T = nxt_chn->GetTot() - fParCont->GetAt(nxt_inum);

                    time_R_B = map1->GetTime();
                    time_L_T = nxt_chn->GetTime();
                }

                else
                {
                    tot_L_T = map1->GetTot() - fParCont->GetAt(inum);
                    tot_R_B = nxt_chn->GetTot() - fParCont->GetAt(nxt_inum);

                    time_L_T = map1->GetTime();
                    time_R_B = nxt_chn->GetTime();
                }

                // It fills the R3BRpcCalData
                TClonesArray& clref = *fRpcCalDataCA;
                Int_t size = clref.GetEntriesFast();
                // R3BRpcCalData(UShort_t channelId, double TimeRight, double TimeLeft, double TotRight, double
                // TotLeft);
                new (clref[size])
                    R3BRpcCalData(iDetector, nxt_chn->GetChannelId(), time_R_B, time_L_T, tot_R_B, tot_L_T);

                break;
            }
        }
    }
    return;
}

void R3BRpcPreCal2Cal::Finish() {}

void R3BRpcPreCal2Cal::Reset()
{
    LOG(DEBUG) << "Clearing RPCTotCalData Structure";
    if (fRpcCalDataCA)
    {
        fRpcCalDataCA->Clear();
    }
}

ClassImp(R3BRpcPreCal2Cal)
