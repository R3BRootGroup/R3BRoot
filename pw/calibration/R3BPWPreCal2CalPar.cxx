/******************************************************************************
 *   Copyright (C) 2026 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2026 Members of R3B Collaboration                     *
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
#include "TF1.h"
#include "TObjArray.h"
#include "TVector3.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "R3BPWPreCal2CalPar.h"
#include "R3BPWTotCalPar.h"

#include "R3BPWPreCalData.h"
#include "TCanvas.h"

#include <iostream>
#include <stdlib.h>

R3BPWPreCal2CalPar::R3BPWPreCal2CalPar()
    : R3BPWPreCal2CalPar("R3B PW Calibration Parameters Finder ", 1)
{
}

R3BPWPreCal2CalPar::R3BPWPreCal2CalPar(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
{
    for (Int_t i = 0; i < N_NUM; i++)
    {
        fhTot[i] = NULL;
    }
}

R3BPWPreCal2CalPar::~R3BPWPreCal2CalPar() { LOG(info) << "R3BPWPreCal2CalPar: Delete instance"; }

InitStatus R3BPWPreCal2CalPar::Init()
{
    LOG(info) << "R3BPWPreCal2CalPar::Init()";

    FairRootManager* rootManager = FairRootManager::Instance();
    if (!rootManager)
    {
        LOG(error) << "R3BPWPreCal2CalPar::Init() FairRootManager not found";
        return kFATAL;
    }

    fPreCalDataCA = dynamic_cast<TClonesArray*>(rootManager->GetObject("R3BPWPreCalData"));
    if (!fPreCalDataCA)
    {
        LOG(error) << "R3BPWPreCal2CalPar::Init() fPreCalDataCA not found";
        return kFATAL;
    }
    FairRuntimeDb* rtdbPar = FairRuntimeDb::instance();
    if (!rtdbPar)
    {
        LOG(error) << "R3BPWPreCal2CalPar::Init() FairRuntimeDb not found";
        return kFATAL;
    }

    fTotCalPar = dynamic_cast<R3BPWTotCalPar*>(rtdbPar->getContainer("PWTotCalPar"));
    if (!fTotCalPar)
    {
        LOG(error) << "R3BPWPreCal2CalPar::Init() Couldn't get handle on PWTotCalPar container";
        return kFATAL;
    }

    // Set container with mapping parameter
    return kSUCCESS;
}

InitStatus R3BPWPreCal2CalPar::ReInit() { return kSUCCESS; }

void R3BPWPreCal2CalPar::Exec(Option_t* opt)
{
    // loop over si data
    Int_t nHits = fPreCalDataCA->GetEntries();
    UInt_t iDetector = 0;
    for (Int_t i = 0; i < nHits; i++)
    {
        auto map1 = dynamic_cast<R3BPWPreCalData*>(fPreCalDataCA->At(i));
        iDetector = map1->GetDetId();
        UInt_t inum = map1->GetChannelId() - 1;
        if (iDetector == 0)
        {
            if (NULL == fhTot[inum])
            {
                TString strName = Form("%s_totcaldata_%d", fTotCalPar->GetName(), inum);
                fhTot[inum] = new TH1F(strName, "", 600, 1, 61);
            }
            fhTot[inum]->Fill(map1->GetTot());
        }
    }
}

void R3BPWPreCal2CalPar::FinishTask()
{
    for (int t = 0; t < N_NUM; t++)
    {
        if (NULL == fhTot[t])
        {
            continue;
        }
        int max = fhTot[t]->GetMaximumBin();
        int s = fhTot[t]->GetBinCenter(max);
        double it = double(30. / s);
        fTotCalPar->SetCalParams(it, t);
    }
    fTotCalPar->setChanged();
    fTotCalPar->printParams();
    fTotCalPar->Write();
}

ClassImp(R3BPWPreCal2CalPar)
