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
#include "TObjArray.h"
#include "TVector3.h"
#include "TF1.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "R3BRpcPreCal2CalPar.h"
#include "R3BRpcTotCalPar.h"

#include "R3BRpcStripPreCalData.h"
#include "R3BRpcPmtPreCalData.h"
#include "TCanvas.h"

#include <iostream>
#include <stdlib.h>

R3BRpcPreCal2CalPar::R3BRpcPreCal2CalPar()
    : R3BRpcPreCal2CalPar("R3B RPC Calibration Parameters Finder ", 1)
{
}

R3BRpcPreCal2CalPar::R3BRpcPreCal2CalPar(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fTotCalPar(NULL)
    , fPreCalStripDataCA(NULL)
    , fPreCalPmtDataCA(NULL)
    , fNumChannels(64)
    , fDebugMode(0)
{
    for (Int_t i = 0; i < N_NUM; i++)
    {
        fhTot[i] = NULL;
    }
}

R3BRpcPreCal2CalPar::~R3BRpcPreCal2CalPar()
{
    LOG(INFO) << "R3BRpcPreCal2CalPar: Delete instance";

}

void R3BRpcPreCal2CalPar::SetParContainers()
{
    // Parameter Container
    FairRuntimeDb* rtdbPar = FairRuntimeDb::instance();
    if (!rtdbPar)
    {
        LOG(ERROR) << "FairRuntimeDb not opened!";
    }
}

void R3BRpcPreCal2CalPar::SetParameter() {}

InitStatus R3BRpcPreCal2CalPar::Init()
{
    LOG(INFO) << "R3BRpcPreCal2CalPar::Init()";

    FairRootManager* rootManager = FairRootManager::Instance();
    if (!rootManager)
    {
        LOG(ERROR) << "R3BRpcPreCal2CalPar::Init() FairRootManager not found";
        return kFATAL;
    }

    fPreCalStripDataCA = (TClonesArray*)rootManager->GetObject("R3BRpcStripPreCalData");
    if (!fPreCalStripDataCA)
    {
        LOG(ERROR) << "R3BRpcPreCal2CalPar::Init() fPreCalStripDataCA not found";
        return kFATAL;
    }

    fPreCalPmtDataCA = (TClonesArray*)rootManager->GetObject("R3BRpcPmtPreCalData");
    if (!fPreCalPmtDataCA)
    {
        LOG(ERROR) << "R3BRpcPreCal2CalPar::Init() fPreCalPmtDataCA not found";
        return kFATAL;
    }

    FairRuntimeDb* rtdbPar = FairRuntimeDb::instance();
    if (!rtdbPar)
    {
        LOG(ERROR) << "R3BRpcPreCal2CalPar::Init() FairRuntimeDb not found";
        return kFATAL;
    }

    fTotCalPar = (R3BRpcTotCalPar*)rtdbPar->getContainer("RpcTotCalPar");
    if (!fTotCalPar)
    {
        LOG(ERROR) << "R3BRpcPreCal2CalPar::Init() Couldn't get handle on RpcTotCalPar container";
        return kFATAL;
    }

    // Set container with mapping parameters
    SetParameter();
    return kSUCCESS;
}

InitStatus R3BRpcPreCal2CalPar::ReInit()
{
    SetParContainers();
    SetParameter();
    return kSUCCESS;
}

void R3BRpcPreCal2CalPar::Exec(Option_t* opt)
{
    //loop over strip data
    Int_t nHits = fPreCalStripDataCA->GetEntries();
    UInt_t iDetector = 0;
    for (Int_t i = 0; i < nHits; i++)
    {
        auto map1 = (R3BRpcStripPreCalData*)(fPreCalStripDataCA->At(i));

        UInt_t inum = (iDetector * 41 + map1->GetChannelId())*2 + map1->GetSide() -2 ;

        if (NULL == fhTot[inum])
        {
            char strName[255];
            sprintf(strName, "%s_totcaldata_%d", fTotCalPar->GetName(),inum);
            fhTot[inum] = new TH1F(strName, "", 2500, 0, 120);
        }
        fhTot[inum]->Fill(map1->GetTot());
    }

    //loop over pmt data
    nHits = fPreCalPmtDataCA->GetEntries();
    iDetector = 1;
    for (Int_t i = 0; i < nHits; i++)
    {
        auto map2 = (R3BRpcPmtPreCalData*)(fPreCalPmtDataCA->At(i));

        UInt_t inum =  (iDetector*41 + map2->GetChannelId())*2 + map2->GetSide() -2; 

        if (NULL == fhTot[inum])
        {
            char strName[255];
            sprintf(strName, "%s_totcaldata_%d", fTotCalPar->GetName(),inum);
            fhTot[inum] = new TH1F(strName, "", 2500, 0, 120);
        }
        fhTot[inum]->Fill(map2->GetTot());
    }
}

void R3BRpcPreCal2CalPar::Reset() {}

void R3BRpcPreCal2CalPar::FinishEvent() {}

Double_t fitf(Double_t *x, Double_t *par)
{
Double_t fitval = par[0] + par[1]*x[0];
return fitval;
}


void R3BRpcPreCal2CalPar::FinishTask() {

    for (int t = 0; t < N_NUM; t++)
    {

        if (NULL == fhTot[t]){continue;}

        float bin_max = (fhTot[t]->GetMaximumBin());

        float max_value = 0.3*fhTot[t]->GetBinContent(bin_max);

        float bin_min=0;

        std::cout << bin_max << " " << max_value << " " << bin_min << " " << t << " " << fhTot[t]->GetBinContent(bin_max) << "\n" ; 
        for(int i = 1; i <= bin_max; i++)
        {

            //std::cout << i << " " << fhTot[t]->GetBinContent(i) << " " << max_value<< "\n" ;
            if (fhTot[t]->GetBinContent(i) >= max_value){
                bin_min=(i-1)*120/2500;
                break;
            }
            
        }
        bin_max = bin_max*120/2500;

        TF1 *linear = new TF1("fitf",fitf,bin_min,bin_max,2);

        // Sets initial values and parameter names
        //linear->SetParameters(90,700);
        linear->SetParNames("intercept","slope");

        // Fit histogram in range defined by function

        fhTot[t]->Fit(linear,"r");
        std::cout << (-1.0*(linear->GetParameter(0)))/linear->GetParameter(1) << "\n";
        fTotCalPar->SetCalParams((-1.0*(linear->GetParameter(0)))/linear->GetParameter(1),t);

    }
    fTotCalPar->setChanged();
    fTotCalPar->printParams();
    fTotCalPar->Write();
}

ClassImp(R3BRpcPreCal2CalPar)
