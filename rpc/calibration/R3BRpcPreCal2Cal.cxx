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
#include "R3BRpcStripCalData.h"

#include "R3BRpcPreCal2Cal.h"

#include "R3BRpcStripPreCalData.h"
#include "R3BRpcPmtPreCalData.h"


// R3BRpcPreCal2Cal: Constructor
R3BRpcPreCal2Cal::R3BRpcPreCal2Cal()
    : FairTask("R3B RPC Calibrator")
    , fNumChannels(0)
    , fStripPreCalDataCA(NULL)
    , fParCont(NULL)
    , fPmtPreCalDataCA(NULL)
    , fRpcStripCalDataCA(NULL)
    , fRpcPmtCalDataCA(NULL)
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
        LOG(ERROR) << "R3BRpcPreCal2Cal:: FairRuntimeDb not opened";
    }



    fStripPreCalDataCA = (TClonesArray*)rootManager->GetObject("R3BRpcStripPreCalData");
    if (!fStripPreCalDataCA)
    {
        LOG(ERROR) << "R3BRpcPreCal2CalPar::Init() fStripPreCalDataCA not found";
        return kFATAL;
    }

    fPmtPreCalDataCA = (TClonesArray*)rootManager->GetObject("R3BRpcPmtPreCalData");
    if (!fPmtPreCalDataCA)
    {
        LOG(ERROR) << "R3BRpcPreCal2CalPar::Init() fPmtPreCalDataCA not found";
        return kFATAL;
    }

    fTotCalPar = (R3BRpcTotCalPar*)rtdb->getContainer("RpcTotCalPar");
    if (!fTotCalPar)
    {
        LOG(ERROR) << "R3BRpcPreCal2CalPar::Init() Couldn't get handle on RpcTotCalPar container";
        return kFATAL;
    }

    //fill the TArray with Tot parameters!!!
    fParCont = fTotCalPar->GetCalParams();

    // OUTPUT DATA
    // Calibrated data
    fRpcStripCalDataCA = new TClonesArray("R3BRpcStripCalData", 50);

    fRpcPmtCalDataCA = new TClonesArray("R3BRpcPmtCalData", 50);

    rootManager->Register("R3BRpcStripCalData", "RPC Strip Cal", fRpcStripCalDataCA, !fOnline);
    rootManager->Register("R3BRpcPmtCalData", "RPC Pmt Cal", fRpcPmtCalDataCA, !fOnline);

    return kSUCCESS;
}

InitStatus R3BRpcPreCal2Cal::ReInit()
{

    return kSUCCESS;
}

void R3BRpcPreCal2Cal::Exec(Option_t* option)
{
    // Reset entries in output arrays, local arrays
    Reset();
    //loop over strip data
    double time_right;
    double time_left;
    double tot_right;
    double tot_left;
    Int_t nHits = fStripPreCalDataCA->GetEntries();
    UInt_t iDetector = 0;
    for (Int_t i = 0; i < nHits; i++)
    {
        auto map1 = (R3BRpcStripPreCalData*)(fStripPreCalDataCA->At(i));

        UInt_t inum = (iDetector * 41 + map1->GetChannelId())*2 + map1->GetSide() -2 ;
        for (Int_t ii = i+1; ii < nHits; ii++)
        {

       
                auto nxt_chn = (R3BRpcStripPreCalData*)fStripPreCalDataCA->At(ii);
            if(map1->GetChannelId() == nxt_chn->GetChannelId()
             && map1->GetSide() != nxt_chn->GetSide() )
            {
                UInt_t nxt_inum = (iDetector * 41 + nxt_chn->GetChannelId())*2 + nxt_chn->GetSide() -2 ;
       
                if (map1->GetSide() == 1)
                {       
                   tot_right = map1->GetTot() - fParCont->GetAt(inum);
                   tot_left = nxt_chn->GetTot() - fParCont->GetAt(nxt_inum);
           
                   time_right = map1->GetTime();
                   time_left = nxt_chn->GetTime();
        
                }

                else
                {
                   tot_left = map1->GetTot() - fParCont->GetAt(inum);
                   tot_right = nxt_chn->GetTot() - fParCont->GetAt(nxt_inum);

                   time_left = map1->GetTime();
                   time_right = nxt_chn->GetTime();
                }
       
                // It fills the R3BRpcStripCalData
                TClonesArray& clref = *fRpcStripCalDataCA;
                Int_t size = clref.GetEntriesFast();
                //R3BRpcStripCalData(UShort_t channelId, double TimeRight, double TimeLeft, double TotRight, double TotLeft);
                new (clref[size]) R3BRpcStripCalData(nxt_chn->GetChannelId(), time_right, time_left, tot_right, tot_left);

                break;
            }
        }
    }
    //loop over pmt data
    double time_top;
    double time_bottom;
    double tot_top;
    double tot_bottom;
    nHits = fPmtPreCalDataCA->GetEntries();
    iDetector = 1;
    for (Int_t i = 0; i < nHits; i++)
    {
        auto map2 = (R3BRpcPmtPreCalData*)(fPmtPreCalDataCA->At(i));

        UInt_t inum =  (iDetector*41 + map2->GetChannelId())*2 + map2->GetSide() -2; 

        for (Int_t ii = i+1; ii < nHits; ii++)
        {
            auto nxt_chn = (R3BRpcPmtPreCalData*)fPmtPreCalDataCA->At(ii);
            if(map2->GetChannelId() == nxt_chn->GetChannelId()
             && map2->GetSide() != nxt_chn->GetSide() )
            {
                UInt_t nxt_inum = (iDetector * 41 + nxt_chn->GetChannelId())*2 + nxt_chn->GetSide() -2 ;

                if (map2->GetSide() == 0)
                {
                    tot_top = map2->GetTot() - fParCont->GetAt(inum);
                    tot_bottom = nxt_chn->GetTot() - fParCont->GetAt(nxt_inum);

                    time_top = map2->GetTime();
                    time_bottom = nxt_chn->GetTime();
                }

                else
                {
                    tot_bottom = map2->GetTot() - fParCont->GetAt(inum);
                    tot_top = nxt_chn->GetTot() - fParCont->GetAt(nxt_inum);

                    time_bottom = map2->GetTime();
                    time_top = nxt_chn->GetTime();
                }
       
                // It fills the R3BRpcPmtCalData
                TClonesArray& clref = *fRpcPmtCalDataCA;
                Int_t size = clref.GetEntriesFast();
                //R3BRpcPmtCalData(UShort_t channelId, double TimeTop, double TimeBottom, double TotTop, double TotBottom);
                new (clref[size]) R3BRpcPmtCalData(nxt_chn->GetChannelId(), time_top, time_bottom, tot_top , tot_bottom);

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
    if (fRpcStripCalDataCA){
        fRpcStripCalDataCA->Clear();
    }
    if (fRpcPmtCalDataCA){
        fRpcPmtCalDataCA->Clear();
    }
}


ClassImp(R3BRpcPreCal2Cal)
                                         