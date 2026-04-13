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
#include "TMath.h"
#include "TObjArray.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "R3BPWCal2HitPar.h"
#include "R3BPWCalData.h"
#include "R3BPWHitPar.h"

#include <algorithm>
#include <iostream>
#include <stdlib.h>
#include <vector>

R3BPWCal2HitPar::R3BPWCal2HitPar()
    : R3BPWCal2HitPar("R3B PW Time Calibration Parameters Finder ", 1)
{
}

R3BPWCal2HitPar::R3BPWCal2HitPar(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
{
    for (Int_t i = 0; i < N_BAR - 1; i++)
    {
        fhTime[i] = nullptr;
    }
}

R3BPWCal2HitPar::~R3BPWCal2HitPar()
{
    LOG(info) << "R3BPWCal2HitPar: Delete instance";
    if (fCalDataCA)
        delete fCalDataCA;
}

void R3BPWCal2HitPar::SetParContainers()
{
    // Parameter Container
    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    if (!rtdb)
    {
        LOG(error) << "FairRuntimeDb not opened!";
    }
}

InitStatus R3BPWCal2HitPar::Init()
{
    LOG(info) << "R3BPWCal2HitPar::Init()";

    FairRootManager* rootManager = FairRootManager::Instance();
    if (!rootManager)
    {
        LOG(error) << "R3BPWCal2HitPar::Init() FairRootManager not found";
        return kFATAL;
    }

    fCalDataCA = dynamic_cast<TClonesArray*>(rootManager->GetObject("R3BPWCalData"));
    if (!fCalDataCA)
    {
        LOG(error) << "R3BPWCal2HitPar::Init() R3BPWCalData not found";
        return kFATAL;
    }

    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    if (!rtdb)
    {
        LOG(error) << "R3BPWCal2HitPar::Init() FairRuntimeDb not found";
        return kFATAL;
    }

    fHitPar = dynamic_cast<R3BPWHitPar*>(rtdb->getContainer("PWHitPar"));
    if (!fHitPar)

    {
        LOG(error) << "R3BPWCal2HitPar::Init() Couldn't get handle on PWHitPar container";
        return kFATAL;
    }

    return kSUCCESS;
}

InitStatus R3BPWCal2HitPar::ReInit()
{
    SetParContainers();
    return kSUCCESS;
}

void R3BPWCal2HitPar::Exec(Option_t* opt)
{

    // loop over si data
    Int_t nHits = fCalDataCA->GetEntries();
    std::vector<double> times(nHits, 0);
    std::vector<int> bar(nHits, 0);

    UInt_t iDetector = 0;
    double time_left = 0;
    double time_right = 0;
    UInt_t inum = 0;

    for (Int_t i = 0; i < nHits; i++)
    {
        auto map1 = dynamic_cast<R3BPWCalData*>(fCalDataCA->At(i));
        iDetector = map1->GetDetId();
        inum = map1->GetChannelId();

        if (iDetector == 0)
        {
            time_right = map1->GetTime0();
            time_left = map1->GetTime1();
            times.push_back((time_left + time_right) / 2.);
            bar.push_back(inum);
        }
    }

    int ind1 = 0;
    int ind2 = 0;
    for (int i = 0; i < N_BAR - 1; i++)
    {

        auto it1 = std::find(bar.begin(), bar.end(), i + 1);
        if (it1 != bar.end())
            ind1 = std::distance(bar.begin(), it1);
        auto it2 = std::find(bar.begin(), bar.end(), i + 2);
        if (it2 != bar.end())
            ind2 = std::distance(bar.begin(), it2);
        if (ind1 > 0 && ind2 > 0)
        {
            if (NULL == fhTime[i])
            {
                TString strName = Form("%s_timecaldata_%d", fHitPar->GetName(), i);
                fhTime[i] = new TH1F(strName, "", 5000, -500, 500);
            }
            fhTime[i]->Fill(times[ind1] - times[ind2]);
        }
    }

    return;
}

void R3BPWCal2HitPar::FinishTask()
{

    CalculateParsBar();

    fHitPar->setChanged();
    fHitPar->printParams();
    fHitPar->Write();
}

void R3BPWCal2HitPar::CalculateParsBar()
{

    double offset[16] = { 0 };

    for (int t = 0; t < N_BAR - 1; t++)
    {

        if (NULL == fhTime[t])
        {
            continue;
        }

        fhTime[t]->Fit("gaus",
                       "",
                       "",
                       fhTime[t]->GetMean() - 2 * fhTime[t]->GetRMS(),
                       fhTime[t]->GetMean() + 2 * fhTime[t]->GetRMS());
        TF1* g = dynamic_cast<TF1*>(fhTime[t]->GetListOfFunctions()->FindObject("gaus"));
        double mu = g->GetParameter(1);
        offset[t + 1] = offset[t] + mu;
    }
    for (int t = 0; t < N_BAR; t++)
        fHitPar->SetCalParams(offset[t], t);
}

ClassImp(R3BPWCal2HitPar)
