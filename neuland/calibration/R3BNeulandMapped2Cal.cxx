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

#include "R3BNeulandMapped2Cal.h"
#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "R3BEventHeader.h"
#include "R3BNeulandCalData.h"
#include "R3BPaddleTamexMappedData.h"
#include "R3BTCalEngine.h"
#include "R3BTCalPar.h"
#include "TClonesArray.h"
#include "TH2F.h"
#include "TMath.h"

R3BNeulandMapped2Cal::R3BNeulandMapped2Cal()
    : FairTask("NeulandMapped2Cal", 1)
    , fNEvents(0)
    , fPulserMode(kFALSE)
    , fWalkEnabled(kTRUE)
    , fMapped(NULL)
    , fPmt(new TClonesArray("R3BNeulandCalData"))
    , fNPmt(0)
    , fTcalPar(NULL)
    , fTrigger(-1)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
{
}

R3BNeulandMapped2Cal::R3BNeulandMapped2Cal(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fNEvents(0)
    , fPulserMode(kFALSE)
    , fWalkEnabled(kTRUE)
    , fMapped(NULL)
    , fPmt(new TClonesArray("R3BNeulandCalData"))
    , fNPmt(0)
    , fTcalPar(NULL)
    , fTrigger(-1)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
{
}

R3BNeulandMapped2Cal::~R3BNeulandMapped2Cal()
{
    if (fPmt)
    {
        delete fPmt;
        fPmt = NULL;
        fNPmt = 0;
    }
}

InitStatus R3BNeulandMapped2Cal::Init()
{
    fNofTcalPars = fTcalPar->GetNumModulePar();

    if (fNofTcalPars == 0)
    {
        LOG(ERROR) << "There are no TCal parameters in container LandTCalPar";
        return kFATAL;
    }

    LOG(INFO) << "R3BNeulandMapped2Cal::Init : read " << fNofTcalPars << " calibrated modules";

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
    {
        LOG(FATAL) << "FairRootManager not found";
    }

    header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");
    if (NULL == header)
    {
        LOG(FATAL) << "Branch R3BEventHeader not found";
    }

    fMapped = (TClonesArray*)mgr->GetObject("NeulandMappedData");
    if (NULL == fMapped)
    {
        LOG(FATAL) << "Branch NeulandMapped not found";
    }

    mgr->Register("NeulandCalData", "Neuland", fPmt, kTRUE);

    htcal1 = new TH2F("htcal1", "htcal1", 800, 0.5, 800.5, 500, -1., 6.);
    htcal2 = new TH2F("htcal2", "htcal2", 800, 0.5, 800.5, 500, -1., 6.);
    htcal3 = new TH2F("htcal3", "htcal3", 800, 0.5, 800.5, 500, -1., 6.);
    htcal4 = new TH2F("htcal4", "htcal4", 800, 0.5, 800.5, 500, -1., 6.);

    return kSUCCESS;
}

void R3BNeulandMapped2Cal::SetParContainers()
{
    fTcalPar = (R3BTCalPar*)FairRuntimeDb::instance()->getContainer("LandTCalPar");

    if (!fTcalPar)
    {
        LOG(ERROR) << "Could not get access to LandTCalPar-Container.";
        fNofTcalPars = 0;
        return;
    }
}

InitStatus R3BNeulandMapped2Cal::ReInit()
{
    SetParContainers();
    return kSUCCESS;
}

void R3BNeulandMapped2Cal::Exec(Option_t* option)
{
    if (fTrigger >= 0)
    {
        if (header->GetTrigger() != fTrigger)
        {
            return;
        }
    }

    Int_t nHits = fMapped->GetEntriesFast();
    if (fPulserMode)
    {
        if (nHits < fNofPMTs)
        {
            return;
        }
    }
    else
    {
        if (nHits > (fNofPMTs / 2))
        {
            return;
        }
    }

    if (nHits >= fNhitmin) // ig  0
    {
        MakeCal();
    }
}

void R3BNeulandMapped2Cal::MakeCal()
{
    Int_t nHits = fMapped->GetEntriesFast();

    R3BTCalModulePar* par;

    Int_t tdc;
    Double_t timeLE;
    Double_t timeTE;

    for (Int_t ihit = 0; ihit < nHits; ihit++)
    {
        R3BPaddleTamexMappedData* hit = (R3BPaddleTamexMappedData*)fMapped->At(ihit);
        if (NULL == hit)
        {
            continue;
        }

        Double_t qdc = -1.;

        Int_t iPlane = hit->GetPlaneId();
        Int_t iBar = hit->GetBarId();
        Int_t iSide = -1 == hit->fCoarseTime1LE ? 2 : 1;

        if (hit->Is17())
        {
            // 17-th channel
            continue;
        }

        if ((iPlane < 1) || (iPlane > fNofPlanes))
        {
            LOG(INFO) << "R3BNeulandMapped2TCal::Exec : Plane number out of range: " << iPlane;
            continue;
        }
        if ((iBar < 1) || (iBar > fNofBarsPerPlane))
        {
            LOG(INFO) << "R3BNeulandMapped2TCal::Exec : Bar number out of range: " << iBar;
            continue;
        }

        int edge = 2 * iSide - 1;

        // Convert TDC to [ns] leading
        if (!(par = fTcalPar->GetModuleParAt(iPlane, iBar, edge)))
        {
            LOG(DEBUG) << "R3BNeulandTcal::Exec : Tcal par not found, barId: " << iBar << ", side: " << iSide;
            continue;
        }

        tdc = 1 == iSide ? hit->fFineTime1LE : hit->fFineTime2LE;
        timeLE = par->GetTimeVFTX(tdc);

        // Convert TDC to [ns] trailing
        if (!(par = fTcalPar->GetModuleParAt(iPlane, iBar, edge + 1)))
        {
            LOG(DEBUG) << "R3BNeulandTcal::Exec : Tcal par not found, barId: " << iBar << ", side: " << iSide;
            continue;
        }

        tdc = 1 == iSide ? hit->fFineTime1TE : hit->fFineTime2TE;
        timeTE = par->GetTimeVFTX(tdc);

        if (timeLE < 0. || timeLE > fClockFreq || timeTE < 0. || timeTE > fClockFreq)
        {
            LOG(ERROR) << "R3BNeulandMapped2Tcal::Exec : error in time calibration: ch= " << iPlane << iBar << iSide
                       << ", tdc= " << tdc << ", time leading edge = " << timeLE << ", time trailing edge = " << timeTE;
            continue;
        }

        if (1 == iSide)
        {
            htcal1->Fill((iPlane - 1) * 50 + iBar, timeLE);
            htcal2->Fill((iPlane - 1) * 50 + iBar, timeTE);
        }
        if (2 == iSide)
        {
            htcal3->Fill((iPlane - 1) * 50 + iBar, timeLE);
            htcal4->Fill((iPlane - 1) * 50 + iBar, timeTE);
        }

        auto coarse = 1 == iSide ? hit->fCoarseTime1LE : hit->fCoarseTime2LE;
        timeLE = fClockFreq - timeLE + coarse * fClockFreq;
        coarse = 1 == iSide ? hit->fCoarseTime1TE : hit->fCoarseTime2TE;
        timeTE = fClockFreq - timeTE + coarse * fClockFreq;

        if (timeTE - timeLE < 0)
        {
            qdc = 2048 * fClockFreq + timeTE - timeLE;
        }
        else
        {
            qdc = timeTE - timeLE;
        }

        if (fWalkEnabled)
            timeLE = timeLE + WalkCorrection(qdc);

        new ((*fPmt)[fNPmt]) R3BNeulandCalData((iPlane - 1) * 50 + iBar, iSide, timeLE, qdc);
        fNPmt += 1;

        /* if (timeTE-timeLE < 0)
          {
            new ((*fPmt)[fNPmt]) R3BNeulandCalData((iPlane-1)*50+iBar, iSide, timeLE,
                               2048*fClockFreq + timeTE-timeLE);
            fNPmt += 1;
          }
        else
          {
            new ((*fPmt)[fNPmt]) R3BNeulandCalData((iPlane-1)*50+iBar, iSide, timeLE,
                               timeTE-timeLE);
            fNPmt += 1;
            } */
    }
}

void R3BNeulandMapped2Cal::FinishEvent()
{
    if (fVerbose && 0 == (fNEvents % 1000))
    {
        LOG(INFO) << "R3BNeulandMapped2Cal::Exec : event=" << fNEvents << " nPMTs=" << fNPmt;
    }

    if (fPmt)
    {
        fPmt->Clear();
        fNPmt = 0;
    }

    fNEvents += 1;
}

void R3BNeulandMapped2Cal::FinishTask()
{

    htcal1->Write();
    htcal2->Write();
    htcal3->Write();
    htcal4->Write();
}

Double_t R3BNeulandMapped2Cal::WalkCorrection(Double_t x)
{
    Double_t y = 0;

    if (x < 0.)
        return y;

    Double_t walkval[34] = { 69.5,  // 18
                             68.0,  // 22
                             67.6,  // 26
                             65.6,  // 30
                             65.0,  // 34
                             63.8,  // 38
                             62.9,  // 42
                             62.5,  // 46
                             62.1,  // 50
                             61.8,  // 54
                             61.5,  // 58
                             61.0,  // 62
                             60.85, // 66
                             60.7,  // 70
                             60.55, // 74
                             60.4,  // 78
                             60.3,  // 82  60.25
                             60.27, // 86  60.4
                             60.05, // 90
                             60.05, // 94 60.1
                             60.0,  // 98
                             59.8,  // 102
                             59.7,  // 106
                             59.6,  // 110
                             59.6,  // 114
                             59.55, // 118
                             59.4,  // 122
                             59.4,  // 126
                             59.3,  // 130
                             59.25, // 134
                             59.05, // 138
                             59.0,  // 142  58.95
                             58.95, 58.91 };

    if (x < 16.)
        y = 70.5 - x / 4.;

    for (Int_t i = 0; i < 34; i++)
    {

        if (x >= 16. + 4. * i && x < 20 + 4. * i)
            y = walkval[i];
    }

    if (x >= 152. && x < 160.)
        y = 58.9 + 0.08 / 8. * (160. - x);
    if (x >= 160.)
        y = 58.55 + 0.3 / 30. * (190. - x);
    // if (x>=160.&&x<190.) y = 58.7 + 0.4/30.*(190.-x);
    // if (x>=190.) y = 58.5;

    return 58.6 - y;
}

ClassImp(R3BNeulandMapped2Cal)
