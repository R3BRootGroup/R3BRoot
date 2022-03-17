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
#include "TF1.h"

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
    , fRpcCalib(true)
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

    //loop over strip data
    Int_t nHits = fCalStripDataCA->GetEntries();
    UInt_t iDetector = 0;
    double charge_left = -1000;
    double charge_right = -1000;
    double time_left=0;
    double time_right = 0;
    double ichn_right= 0;
    double ichn_left= 0;

    bool valid = false;


    for (Int_t i = 0; i < nHits; i++)
    {
        auto map1 = (R3BRpcStripCalData*)(fCalStripDataCA->At(i));
        valid =true; 

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
        
        UInt_t inum = iDetector * 41 + ichn_right -1;
        if (NULL == fhPos[inum])
        {
            char strName[255];
            sprintf(strName, "%s_poscaldata_%d", fHitPar->GetName(),inum);
            
            if(!fRpcCalib){
                fhPos[inum] = new TH1F(strName, "", 750,0.,1500.);
            }
            else{
                fhPos[inum] = new TH1F(strName, "", 800,-20.,20.);
            }

        }
        if (NULL == fhTime[inum])
        {
            char strName[255];
            sprintf(strName, "%s_timecaldata_%d", fHitPar->GetName(),inum);
            fhTime[inum] = new TH1F(strName, "", 800,-200,-150);
        }

    }

    if(ichn_left == ichn_right && valid ){
        UInt_t inum = iDetector * 41 + ichn_right -1;

        
        if(!fRpcCalib){
            fhPos[inum]->Fill(((((time_left-time_right)/2. - (fHitPar->GetCalParams1()->GetAt(inum)-400)*40./800.)*800./40.)
                  /(fHitPar->GetCalParams2()->GetAt(inum) - fHitPar->GetCalParams1()->GetAt(inum)))*1500);
        }
        else{
            fhPos[inum]->Fill((time_left - time_right)/2.);
        }
        fhTime[inum]->Fill((time_left + time_right)/2.); 
    }

    //loop over Pmt data
    nHits = fCalPmtDataCA->GetEntries();
    iDetector = 1;

    for (Int_t i = 0; i < nHits; i++)
    {
        auto map2 = (R3BRpcPmtCalData*)(fCalPmtDataCA->At(i));
        UInt_t inum = iDetector * 41 + map2->GetChannelId()-1;
        if (NULL == fhPos[inum])
        {
            char strName[255];
            sprintf(strName, "%s_poscaldata_%d", fHitPar->GetName(),inum);
            fhPos[inum] = new TH1F(strName, "", 5000,-2500.,2500.);
        }
        fhPos[inum]->Fill((map2->GetTimeBottom() - map2->GetTimeTop())*CSCINT/2.);
    }

    return;
}

void R3BRpcCal2HitPar::Reset() {}

void R3BRpcCal2HitPar::FinishEvent() {}

void R3BRpcCal2HitPar::FinishTask() {

    if(fRpcCalib){CalculateParsStrip();}
    else{

        CalculateParsPmt();
    }

    fHitPar->setChanged();
    fHitPar->printParams();
    fHitPar->Write();
}

void R3BRpcCal2HitPar::CalculateParsPmt(){

    double counter=0;
    double average=0;
    std::vector<double> v;

    for (int t = 0; t < N_STRIP_NB; t++){

        if (NULL == fhPos[t]){continue;}

        int bin_max = fhPos[t]->GetMaximumBin();
        if(t>=41){
            float threshold = 
            (fhPos[t]->GetBinContent(bin_max - 1) + fhPos[t]->GetBinContent(bin_max) + 
            fhPos[t]->GetBinContent(bin_max + 1) )/10.;

            for(int i = 1; i <= bin_max; i++){
                if (fhPos[t]->GetBinContent(i) >= threshold){
                    fHitPar->SetCalParams1(i,t);
                    break;
                }                
            }
        }
        else{

            if (NULL == fhPos[t]){continue;}

            fhPos[t]->GetXaxis()->SetRange(bin_max-50,bin_max+50);

            fhPos[t]->Fit("gaus");

            TF1 *g = (TF1*)fhPos[t]->GetListOfFunctions()->FindObject("gaus");
            
            v.push_back(g->GetParameter(1));
            average+=g->GetParameter(1);
            counter++;
  
        }
    }

    for (int i = 0; i < v.size(); i++)
    {
        std::cout << v[i] << " " << v[i]-average/counter << " " << average/counter<<  std::endl;
        fHitPar->SetCalParams4(v[i]-average/counter,i);
    }
}

void R3BRpcCal2HitPar::CalculateParsStrip(){

    for (int t = 0; t < N_STRIP_NB; t++){

        if (NULL == fhPos[t]){continue;}

        fHitPar->SetCalParams4(0,t);

        int bin_max = fhPos[t]->GetMaximumBin();
        
        float threshold = 
        (fhPos[t]->GetBinContent(bin_max - 1) + fhPos[t]->GetBinContent(bin_max) + 
        fhPos[t]->GetBinContent(bin_max + 1) )/90.;

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

    for (int t = 0; t < N_STRIP_NB; t++){

        if (NULL == fhTime[t]){continue;}

        int bin_max = fhTime[t]->GetMaximumBin();

        fhTime[t]->GetXaxis()->SetRange(bin_max-50,bin_max+50);

        fhTime[t]->Fit("gaus");

        TF1 *g = (TF1*)fhTime[t]->GetListOfFunctions()->FindObject("gaus");
        fHitPar->SetCalParams3(g->GetParameter(1),t);

    }
}

ClassImp(R3BRpcCal2HitPar)
