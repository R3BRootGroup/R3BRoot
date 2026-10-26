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

#include "R3BHFIBCal2HitPar.h"
#include "R3BHFIBTotCalPar.h"

#include "R3BHFIBCalData.h"
#include "TCanvas.h"

#include <iostream>
#include <stdlib.h>

R3BHFIBCal2HitPar::R3BHFIBCal2HitPar()
    : R3BHFIBCal2HitPar("R3B HFIB Calibration Parameters Finder ", 1)
{
}

R3BHFIBCal2HitPar::R3BHFIBCal2HitPar(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
{
    for (Int_t i = 0; i < N_NUM; i++)
    {
        fhTot[i] = NULL;
    }
}

R3BHFIBCal2HitPar::~R3BHFIBCal2HitPar() { LOG(info) << "R3BHFIBCal2HitPar: Delete instance"; }

InitStatus R3BHFIBCal2HitPar::Init()
{
    LOG(info) << "R3BHFIBCal2HitPar::Init()";

    FairRootManager* rootManager = FairRootManager::Instance();
    if (!rootManager)
    {
        LOG(error) << "R3BHFIBCal2HitPar::Init() FairRootManager not found";
        return kFATAL;
    }

    fCalDataCA = dynamic_cast<TClonesArray*>(rootManager->GetObject("R3BHFIBCalData"));
    if (!fCalDataCA)
    {
        LOG(error) << "R3BHFIBCal2HitPar::Init() fCalDataCA not found";
        return kFATAL;
    }
    FairRuntimeDb* rtdbPar = FairRuntimeDb::instance();
    if (!rtdbPar)
    {
        LOG(error) << "R3BHFIBCal2HitPar::Init() FairRuntimeDb not found";
        return kFATAL;
    }

    fTotCalPar = dynamic_cast<R3BHFIBTotCalPar*>(rtdbPar->getContainer("HFIBTotCalPar"));
    if (!fTotCalPar)
    {
        LOG(error) << "R3BHFIBCal2HitPar::Init() Couldn't get handle on HFIBTotCalPar container";
        return kFATAL;
    }

    // Set container with mapping parameter
    return kSUCCESS;
}

InitStatus R3BHFIBCal2HitPar::ReInit() { return kSUCCESS; }

void R3BHFIBCal2HitPar::Exec(Option_t* opt)
{
    // loop over farich data
    Int_t nHits = fCalDataCA->GetEntries();
    UInt_t iDetector = 0;
    UInt_t iTdc = 0;

    for (Int_t i = 0; i < nHits; i++)
    {
        auto cal1 = dynamic_cast<R3BHFIBCalData*>(fCalDataCA->At(i));
        iDetector = cal1->GetDetId();
        iTdc = cal1->GetTDC();
        UInt_t inum = (iDetector - 1) + (iTdc - 1) + (cal1->GetChannelId() - 1);
        if (iDetector > 0)
        {
            if (NULL == fhTot[inum])
            {
                TString strName = Form("%s_totcaldata_%d", fTotCalPar->GetName(), inum);
                fhTot[inum] = new TH1F(strName, "", 500, 0, 100);
            }
            fhTot[inum]->Fill(cal1->GetTot());
        }
    }
}

void R3BHFIBCal2HitPar::FinishTask()
{
    for (int t = 0; t < N_NUM; t++)
    {
        if (NULL == fhTot[t])
        {
            continue;
        }
        fhTot[t]->Fit("gaus");
        TF1* f = fhTot[t]->GetFunction("gaus");
        double mean = f->GetParameter(1);
        double shift = mean - 10.;
        fTotCalPar->SetCalParams(shift, t);
    }
    fTotCalPar->setChanged();
    fTotCalPar->printParams();
    fTotCalPar->Write();
}

ClassImp(R3BHFIBCal2HitPar)
