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

#include "R3BRpcCal2Hit.h"
#include "TClonesArray.h"
#include "TMath.h"
#include "TObjArray.h"
#include "TRandom.h"
#include "TVector3.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TGeoManager.h"
#include "TGeoMatrix.h"

#include "R3BRpcStripCalData.h"
#include "R3BRpcPmtCalData.h"
#include <list>
#include <vector>

R3BRpcCal2Hit::R3BRpcCal2Hit()
    : FairTask("R3B RPC Cal to Hit")
    , fCalDataCA(NULL)
    , fParCont1(NULL)
    , fParCont2(NULL)
    , fRpcHitStripDataCA(NULL)
    , fRpcHitPmtDataCA(NULL)
    , fRpcCalStripDataCA(NULL)
    , fRpcCalPmtDataCA(NULL)
    , fOnline(kFALSE)
{
}

R3BRpcCal2Hit::~R3BRpcCal2Hit()
{
    LOG(INFO) << "R3BRpcCal2Hit: Delete instance";
    if (fRpcHitStripDataCA)
        delete fRpcHitStripDataCA;
    if (fRpcHitPmtDataCA)
        delete fRpcHitPmtDataCA;

}

void R3BRpcCal2Hit::SetParContainers()
{

}

InitStatus R3BRpcCal2Hit::Init()
{

    // Parameter Container
    // Reading RPCHitPar from FairRuntimeDb
    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    if (!rtdb)
    {
        LOG(ERROR) << "R3BRpcCal2Hit:: FairRuntimeDb not opened";
    }

    FairRootManager* rootManager = FairRootManager::Instance();
    if (!rootManager)
    {
        LOG(FATAL) << "R3BRpcCal2Hit::FairRootManager not found";
        return kFATAL;
    }


    fHitPar = (R3BRpcHitPar*)rtdb->getContainer("RpcHitPar");
    if (!fHitPar)
    {
        LOG(ERROR) << "R3BRpcCal2Hit::Init() Couldn't get handle on RPCHitPar container";
    }
    else
    {
        LOG(INFO) << "R3BRpcCal2Hit:: RPCHitPar container open";
    }

    fRpcCalStripDataCA = (TClonesArray*)rootManager->GetObject("R3BRpcStripCalData");
    if (!fRpcCalStripDataCA)
    {
        LOG(ERROR) << "R3BRpcCal2HitPar::Init() R3BRpcStripCalData not found";
        return kFATAL;
    }

    fRpcCalPmtDataCA = (TClonesArray*)rootManager->GetObject("R3BRpcPmtCalData");
    if (!fRpcCalPmtDataCA)
    {
        LOG(ERROR) << "R3BRpcCal2HitPar::Init() R3BRpcPmtCalData not found";
        return kFATAL;
    }

    // Register output array
    fRpcHitStripDataCA = new TClonesArray("R3BRpcStripHitData");
    fRpcHitPmtDataCA = new TClonesArray("R3BRpcPmtHitData");
    rootManager->Register("RpcStripHitData", "RPC Strip Hit", fRpcHitStripDataCA, !fOnline);
    rootManager->Register("RpcPmtHitData", "RPC Pmt Hit", fRpcHitPmtDataCA, !fOnline);


    //fill the TArray with Tot parameters!!!
    fParCont1 = fHitPar->GetCalParams1();
    fParCont2 = fHitPar->GetCalParams2();

    return kSUCCESS;
}

InitStatus R3BRpcCal2Hit::ReInit()
{
    SetParContainers();
    return kSUCCESS;
}

void R3BRpcCal2Hit::Exec(Option_t* opt)
{

    //loop over strip data
    Int_t nHits = fRpcCalStripDataCA->GetEntries();
    UInt_t iDetector = 0;
    double charge_left = -1000;
    double charge_right = -1000;
    double time_left=0;
    double time_right = 0;
    double ichn_right= 0;
    double ichn_left= 0;

    for (Int_t i = 0; i < nHits; i++)
    {
    
        auto map1 = (R3BRpcStripCalData*)(fRpcCalStripDataCA->At(i));

        if(map1->GetTotRight() >=  charge_right){
            charge_right=map1->GetTotRight();
            time_right=map1->GetTimeRight();
            ichn_right = map1->GetChannelId();
        }

        if(map1->GetTotLeft() >= charge_left){
            charge_left=map1->GetTotLeft();
            time_left= map1->GetTimeLeft();
            ichn_left = map1->GetChannelId();
        }

    }

    return;

    if(ichn_left == ichn_right){

        double position = (time_left - time_right)/2.;

        double charge =  (charge_left + charge_right)/2.;

        double time = (time_left + time_right)/2.;

        TClonesArray& clref = *fRpcHitStripDataCA;
        Int_t size = clref.GetEntriesFast();
        //R3BRpcStripHitData(UInt_t channel, double time, double pos, double charge);
        new (clref[size]) R3BRpcStripHitData(ichn_right, time, position,charge);

    }

    /*//loop over Pmt data
    nHits = fRpcCalPmtDataCA->GetEntries();
    iDetector = 1;
    for (Int_t i = 0; i < nHits; i++)
    {
        auto map2 = (R3BRpcPmtCalData*)(fRpcCalPmtDataCA->At(i));

        if(max_charge <= (map2->GetTotBottom() + map2->GetTotTop())/2.){continue;}

        UInt_t inum = iDetector * 41 + map2->GetChannelId() -1;

    }*/
}

void R3BRpcCal2Hit::Reset()
{
    LOG(DEBUG) << "Clearing RPCHItStructure Structure";
    if (fRpcHitStripDataCA){
        fRpcHitStripDataCA->Clear();
    }
    if (fRpcHitPmtDataCA){
        fRpcHitPmtDataCA->Clear();
    }
}


ClassImp(R3BRpcCal2Hit);
