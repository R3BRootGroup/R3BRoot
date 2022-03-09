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
#include "TObjArray.h"
#include "TVector3.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "R3BRpcCal2HitPar.h"
#include "R3BRpcStripCalData.h"
#include "R3BRpcPmtCalData.h"

#include "R3BRpcHitPar.h"

#include <iostream>
#include <stdlib.h>

R3BRpcCal2HitPar::R3BRpcCal2HitPar()
    : R3BRpcCal2HitPar("R3B Rpc Tot Calibration Parameters Finder ", 1)
{
}

R3BRpcCal2HitPar::R3BRpcCal2HitPar(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fHitPar(NULL)
    , fCalStripDataCA(NULL)
    , fCalPmtDataCA(NULL)
    , fNumChannels(64)
    , fDebugMode(false)
{
    for (Int_t i = 0; i < N_STRIP_NB; i++)
    {
        fhPos[i] = NULL;
        fhTime[i] = NULL;

    }

}

R3BRpcCal2HitPar::~R3BRpcCal2HitPar()
{
    LOG(INFO) << "R3BRpcCal2HitPar: Delete instance";
    if (fCalStripDataCA)
        delete fCalStripDataCA;
    if (fCalPmtDataCA)
        delete fCalPmtDataCA;
}

void R3BRpcCal2HitPar::SetParContainers()
{
    // Parameter Container
    // Reading califaMappingPar from FairRuntimeDb
    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    if (!rtdb)
    {
        LOG(ERROR) << "FairRuntimeDb not opened!";
    }
}

void R3BRpcCal2HitPar::SetParameter() {}

InitStatus R3BRpcCal2HitPar::Init()
{
    LOG(INFO) << "R3BRpcCal2HitPar::Init()";

    FairRootManager* rootManager = FairRootManager::Instance();
    if (!rootManager)
    {
        LOG(ERROR) << "R3BRpcCal2HitPar::Init() FairRootManager not found";
        return kFATAL;
    }

    fCalStripDataCA = (TClonesArray*)rootManager->GetObject("R3BRpcStripCalData");
    if (!fCalStripDataCA)
    {
        LOG(ERROR) << "R3BRpcCal2HitPar::Init() R3BRpcStripCalData not found";
        return kFATAL;
    }

    fCalPmtDataCA = (TClonesArray*)rootManager->GetObject("R3BRpcPmtCalData");
    if (!fCalPmtDataCA)
    {
        LOG(ERROR) << "R3BRpcCal2HitPar::Init() R3BRpcPmtCalData not found";
        return kFATAL;
    }

    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    if (!rtdb)
    {
        LOG(ERROR) << "R3BRpcCal2HitPar::Init() FairRuntimeDb not found";
        return kFATAL;
    }

    fHitPar = (R3BRpcHitPar*)rtdb->getContainer("RpcHitPar");
    if (!fHitPar)

    {
        LOG(ERROR) << "R3BRpcCal2HitPar::Init() Couldn't get handle on RPCHitPar container";
        return kFATAL;
    }

    // Set container with mapping parameters
    SetParameter();

    return kSUCCESS;
}

InitStatus R3BRpcCal2HitPar::ReInit()
{
    SetParContainers();
    SetParameter();
    return kSUCCESS;
}

void R3BRpcCal2HitPar::Exec(Option_t* opt)
{
    //std::cout << "something big and stupid !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1" << "\n";
    //loop over strip data
    Int_t nHits = fCalStripDataCA->GetEntries();
    UInt_t iDetector = 0;
    double charge_left = -1000;
    double charge_right = -1000;
    double time_left=0;
    double time_right = 0;
    double ichn_right= 0;
    double ichn_left= 0;

    for (Int_t i = 0; i < nHits; i++)
    {
        //std::cout << "something big and stupid !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1.1" << "\n";
        auto map1 = (R3BRpcStripCalData*)(fCalStripDataCA->At(i));

        if(map1->GetTotRight() >=  charge_right){
//std::cout << "something big and stupid !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1.2" << "\n";
            charge_right=map1->GetTotRight();
            time_right=map1->GetTimeRight();
            ichn_right = map1->GetChannelId();
        }

        if(map1->GetTotLeft() >= charge_left){
//std::cout << "something big and stupid !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1.3" << "\n";
            charge_left=map1->GetTotLeft();
            time_left= map1->GetTimeLeft();
            //std::cout << map1->GetChannelId() << std::endl;
            ichn_left = map1->GetChannelId();
        }

    }
//std::cout << "something big and stupid !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!2" << "\n";
    if(ichn_left == ichn_right){
        UInt_t inum = iDetector * 41 + ichn_right -1;
//std::cout << iDetector << " " << ichn_right << " " << inum << "\n";
        if (NULL == fhPos[inum])
        {
            char strName[255];
            sprintf(strName, "%s_poscaldata_%d", fHitPar->GetName(),inum);
            fhPos[inum] = new TH1F(strName, "", 800,-20.,20.);
        }

        if (NULL == fhTime[inum])
        {
            char strName[255];
            sprintf(strName, "%s_timecaldata_%d", fHitPar->GetName(),inum);
            fhTime[inum] = new TH1F(strName, "", 800,-200,-150);
        }

        //std::cout << inum  << "\n";
        fhPos[inum]->Fill((time_left - time_right)/2.);
        fhTime[inum]->Fill((time_left + time_right)/2.); 
    }

    //loop over Pmt data
    nHits = fCalPmtDataCA->GetEntries();
    iDetector = 1;
    double charge_top = 0;
    double charge_bottom = 0;
    double time_top = 0;
    double time_bottom = 0;
    double ichn_bottom = 0;
    double ichn_top = 0;
    for (Int_t i = 0; i < nHits; i++)
    {
        auto map2 = (R3BRpcPmtCalData*)(fCalPmtDataCA->At(i));

        if(map2->GetTotTop() >=  charge_top){

            charge_top=map2->GetTotTop();
            time_top=map2->GetTimeTop();
            ichn_top = map2->GetChannelId();
        }

        if(map2->GetTotBottom() >= charge_bottom){

            charge_bottom=map2->GetTotBottom();
            time_bottom=map2->GetTimeBottom();
            ichn_bottom = map2->GetChannelId();
        }

    }

    if(ichn_bottom == ichn_top){
        UInt_t inum = iDetector * 41 + ichn_top -1;

        if (NULL == fhPos[inum])
        {
            char strName[255];
            sprintf(strName, "%s_poscaldata_%d", fHitPar->GetName(),inum);
            fhPos[inum] = new TH1F(strName, "", 800,-20.,20.);
        }

        if (NULL == fhTime[inum])
        {
            char strName[255];
            sprintf(strName, "%s_timecaldata_%d", fHitPar->GetName(),inum);
            fhTime[inum] = new TH1F(strName, "", 800,-200,-150);
        }

        fhPos[inum]->Fill((time_bottom - time_top)/2.);
        fhTime[inum]->Fill((time_bottom + time_top)/2.); 
    }
    return;
}

void R3BRpcCal2HitPar::Reset() {}

void R3BRpcCal2HitPar::FinishEvent() {}

void R3BRpcCal2HitPar::FinishTask() {

    for (int t = 0; t < N_STRIP_NB; t++){

        if (NULL == fhPos[t]){continue;}

        int bin_max = fhPos[t]->GetMaximumBin();

        float threshold = 
        (fhPos[t]->GetBinContent(bin_max - 1) + fhPos[t]->GetBinContent(bin_max) + 
        fhPos[t]->GetBinContent(bin_max + 1) )/30.;

        for(int i = 1; i <= bin_max; i++){
            if (fhPos[t]->GetBinContent(i) >= threshold){
                fHitPar->SetCalParams1(i,t);
                break;
            }                
        }
        for(int i = 800; i > bin_max; i--){
            if (fhPos[t]->GetBinContent(i) >= threshold){
                fHitPar->SetCalParams2(i,t);
                break;
            }
        }
    }

    /*for (int t = 0; t < N_STRIP_NB; t++){

        if (NULL == fhTime[t]){continue;}

        int bin_max = fhTime[t]->GetMaximumBin();

        float threshold = 
        (fhTime[t]->GetBinContent(bin_max - 1) + fhTime[t]->GetBinContent(bin_max) + 
        fhTime[t]->GetBinContent(bin_max + 1) )/30.;

        for(int i = 1; i <= bin_max; i++){
            if (fhTime[t]->GetBinContent(i) >= threshold){
                fHitPar->SetCalParams1(i,t);
                break;
            }                
        }
        for(int i = 800; i > bin_max; i--){
            if (fhTime[t]->GetBinContent(i) >= threshold){
                fHitPar->SetCalParams2(i,t);
                break;
            }
        }
    }*/
    fHitPar->setChanged();
    fHitPar->printParams();
    fHitPar->Write();
}

ClassImp(R3BRpcCal2HitPar)
