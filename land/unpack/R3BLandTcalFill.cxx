// ----------------------------------------------------------------
// -----                     R3BLandTcalFill                  -----
// -----             Created 04-04-2014 by D.Kresan           -----
// ----------------------------------------------------------------

#include "R3BLandTcalFill.h"
#include "R3BLandRawHitMapped.h"
#include "R3BEventHeader.h"

#include "R3BLandCalPar.h"
#include "R3BLandTCalPar.h"

#include "FairRootManager.h"
#include "FairRuntimeDb.h"
#include "FairRunIdGenerator.h"
#include "FairRtdbRun.h"
#include "FairLogger.h"

#include "TClonesArray.h"
#include "TH1F.h"
#include "TF1.h"

#include <iostream>
#include <stdlib.h>

using namespace std;

R3BLandTcalFill::R3BLandTcalFill()
    : fUpdateRate(1000000)
    , fMinStats(100000)
    , fTrigger(1)
    , fNofPMTs(0)
    , fNof17(0)
    , fhData(NULL)
    , fhTime(NULL)
    , f1(new TF1("f1", "[0]", 1500., 2500.))
    , fNEvents(0)
    , fStoreDB(kFALSE)
    , fCal_Par(NULL)
{
}

R3BLandTcalFill::R3BLandTcalFill(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fUpdateRate(1000000)
    , fMinStats(100000)
    , fTrigger(1)
    , fNofPMTs(0)
    , fNof17(0)
    , fhData(NULL)
    , fhTime(NULL)
    , f1(new TF1("f1", "[0]", 1500., 2500.))
    , fNEvents(0)
    , fStoreDB(kFALSE)
    , fCal_Par(NULL)
{
}

R3BLandTcalFill::~R3BLandTcalFill()
{
    if (fhData)
    {
        for (Int_t i = 0; i < (2*fNofPMTs); i++)
        {
            delete fhData[i];
        }
        delete fhData;
        fhData = NULL;
    }
    if (fhTime)
    {
        for (Int_t i = 0; i < (2*fNofPMTs); i++)
        {
            delete fhTime[i];
        }
        delete fhTime;
        fhTime = NULL;
    }

    if (fCal_Par)
        delete fCal_Par;
}

InitStatus R3BLandTcalFill::Init()
{
    FairRootManager* rm = FairRootManager::Instance();
    if (!rm)
    {
        return kFATAL;
    }
    header = (R3BEventHeader*) rm->GetObject("R3BEventHeader");
    if(! header)
    {
        return kFATAL;
    }
    fHits = (TClonesArray*)rm->GetObject("LandRawHitMapped");
    if (!fHits)
    {
        return kFATAL;
    }
    if (fStoreDB)
    {
        fCal_Par = (R3BLandCalPar*)FairRuntimeDb::instance()->getContainer("LandCalPar");
        fCal_Par->setChanged();
    }

    CreateContainers();

    return kSUCCESS;
}

void R3BLandTcalFill::Exec(Option_t* option)
{
    if(header->GetTrigger() != fTrigger)
    {
        return;
    }
    
    Int_t nHits = fHits->GetEntries();
    if(nHits > (fNofPMTs/2))
    {
        return;
    }
    
    R3BLandRawHitMapped* hit;
    Int_t iBar;
    Int_t iSide;
    Int_t channel;

    // Loop over mapped hits
    for (Int_t i = 0; i < nHits; i++)
    {
        hit = (R3BLandRawHitMapped*)fHits->At(i);
        if (!hit)
        {
            continue;
        }
        
        // Check bar ID
        iBar = hit->GetBarId();
        if ((iBar - 1) >= (fNofPMTs / 2))
        {
            LOG(ERROR) << "R3BLandTcalFill::Exec() : wrong bar ID: " << iBar << FairLogger::endl;
            continue;
        }

        if (hit->Is17())
        {
            // 17-th channel
            channel = fNofPMTs + hit->GetGtb()*20 + hit->GetTacAddr();
        }
        else
        {
            // PMT signal
            iSide = hit->GetSide();
            channel = (Double_t)fNofPMTs / 2 * (iSide - 1) + iBar - 1;
        }

        // Check validity of module
        if (channel < 0 || channel >= (2*fNofPMTs))
        {
            LOG(INFO) << "Bar:" << iBar << "  Side:" << iSide << "  " << channel << "   " << (2*fNofPMTs + fNof17) << "   " << hit->GetGtb() << FairLogger::endl;
            FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "Illegal detector ID...");
        }

        // Fill TAC histogram
        fhData[channel]->Fill(hit->GetTacData());
    }

    // Increment events
    fNEvents += 1;
}

void R3BLandTcalFill::FinishEvent()
{
}

void R3BLandTcalFill::FinishTask()
{
//    if (0 == (fNEvents % fUpdateRate))
    {
        // Re-calculate calibration parameters
//        for (Int_t im = 0; im < (fNofPMTs + fNof17); im++)
        for (Int_t im = 0; im < (2*fNofPMTs); im++)
        {
            if (fhData[im]->GetEntries() >= fMinStats)
            {
                CalculateParams(im);
                WriteContainer(im);
                fhData[im]->Reset();
            }
        }
        
//        Int_t runId = FairRuntimeDb::instance()->getCurrentRun()->getRunId();
        if (fStoreDB && fCal_Par)
        {
            fCal_Par->setChanged();
//            fCal_Par->Print();
//            fCal_Par->store(runId);
        }
    }
}

void R3BLandTcalFill::CreateContainers()
{
    if (0 == fNofPMTs && 0 == fNof17)
    {
        return;
    }
//    fhData = new TH1F* [fNofPMTs + fNof17];
//    fhTime = new TH1F* [fNofPMTs + fNof17];
    fhData = new TH1F* [2*fNofPMTs];
    fhData100 = new TH1F* [2*fNofPMTs];
    fhTime = new TH1F* [2*fNofPMTs];
    char str[20];
//    for (Int_t i = 0; i < (fNofPMTs + fNof17); i++)
    for (Int_t i = 0; i < (2*fNofPMTs); i++)
    {
        sprintf(str, "hTcalData_%d", i);
        fhData[i] = new TH1F(str, "", 4096, 0., 4096.);
        sprintf(str, "hTime_%d", i);
        fhTime[i] = new TH1F(str, "", 4096, 0., 4096.);
    }
}

void R3BLandTcalFill::CalculateParams(Int_t iModule)
{
    // Define range of channels
    fhData100[iModule] = (TH1F*)fhData[iModule]->Clone();
    fhData100[iModule]->Rebin(8);
    fhData100[iModule]->Fit(f1, "QNR");
    for (Int_t i = 256; i >= 0; i--)
    {
        if (fhData100[iModule]->GetBinContent(i + 1) < 0.1*f1->GetParameter(0))
        {
            iMin = i - 1;
            break;
        }
    }
    for (Int_t j = 256; j < 512; j++)
    {
        if (fhData100[iModule]->GetBinContent(j + 1) < 0.1*f1->GetParameter(0))
        {
            iMax = j + 1;
            break;
        }
    }
    if (iMax <= iMin)
    {
        LOG(ERROR) << "Error in definition of TCAL range" << FairLogger::endl;
        return;
    }
    iMin = Int_t(((Double_t)iMin-0.5)*8);
    iMax = Int_t(((Double_t)iMax+0.5)*8);
    if(iMin < 0 || iMax > 4095)
    {
        return;
    }
    LOG(INFO) << "R3BLandTcalFill::CalculateParams() : Range of channels: " << iMin << " - " << iMax << FairLogger::endl;

    Int_t nch = 0;
    Int_t ibin = iMin;
    Int_t group;
    Double_t prev_time = 0.;

    R3BLandTCalPar* pTCal = NULL;
    if (fStoreDB)
    {
        pTCal = new R3BLandTCalPar();
        pTCal->SetBarId(iModule);
    }

    Int_t incr = 0;
    while (ibin <= iMax)
    {
        // Iteratively compute parameter
        group = CalculateBin(iModule, prev_time, ibin, 1);
        // Fill time calibration parameter
        for (Int_t i1 = ibin; i1 < (ibin + group); i1++)
        {
            fhTime[iModule]->SetBinContent(i1 + 1, prev_time);
        }

        LOG(DEBUG) << " Module: " << iModule << " bin range: " << ibin << " : " << ibin + group << " dbin: " << group << " time set: " << prev_time << FairLogger::endl;

        if (pTCal)
        {
            pTCal->SetBinLowAt(ibin, incr);
            pTCal->SetBinUpAt(ibin + group - 1, incr);
            pTCal->SetTimeAt(prev_time, incr);
            pTCal->IncrementNofChannels();
        }

        // Next range of channels
        ibin += group;
        nch += 1;
        incr++;
    }

    if (fStoreDB)
        fCal_Par->AddTCalPar(pTCal);

    LOG(INFO) << "R3BLandTcalFill::CalculateParams() : Number of parameters: " << nch << FairLogger::endl;
}

Int_t R3BLandTcalFill::CalculateBin(Int_t iModule, Double_t& prev_time, Int_t ibin, Int_t ngroup)
{
    if ((ibin + ngroup) > iMax)
    {
        Double_t total = fhData[iModule]->Integral(1, 4096);
        Double_t itot = fhData[iModule]->Integral(1, (ibin+1) + ngroup);
        if (itot > 0. && total > 0.)
        {
            Double_t time = 24.99819 * itot / total; // time of channel in [ns]
            LOG(DEBUG) << "R3BLandTcalFill::CalculateBin() : bin=" << ibin << "  time=" << time << "  ngroup=" << ngroup << FairLogger::endl;
            prev_time = time;
        }
        return ngroup;
    }
    Double_t total = fhData[iModule]->Integral(1, 4096);
    Double_t itot = fhData[iModule]->Integral(1, (ibin+1) + ngroup);
    if (itot > 0. && total > 0.)
    {
        Double_t time = 24.99819 * itot / total; // time of channel in [ns]
        Double_t diff = time - prev_time;   // time difference to previous range
        if (diff * 1e3 < 70.)               // check if below resolution
        {
            // Next iteration
            return CalculateBin(iModule, prev_time, ibin, ngroup + 1);
        }
        else
        {
            // Finalize
            LOG(DEBUG) << "R3BLandTcalFill::CalculateBin() : bin=" << ibin << "  time=" << time << "  ngroup=" << ngroup << FairLogger::endl;
            prev_time = time;
            return ngroup;
        }
    }
    return ngroup;
}

void R3BLandTcalFill::WriteContainer(Int_t iModule)
{
    fhData[iModule]->Write();
    fhTime[iModule]->Write();

    LOG(INFO) << "R3BLandTcalFill : TCAL container " << iModule << " is saved" << FairLogger::endl << FairLogger::endl;
}

ClassImp(R3BLandTcalFill)
