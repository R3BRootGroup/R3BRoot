// ----------------------------------------------------------------
// -----                      R3BLosTcalFill                  -----
// -----             Created 18-04-2014 by D.Kresan           -----
// ----------------------------------------------------------------

#include "R3BLosTcalFill.h"
#include "R3BLosRawHit.h"

#include "R3BLosCalPar.h"
#include "R3BLosTCalPar.h"

#include "FairRootManager.h"
#include "FairRuntimeDb.h"
#include "FairRunIdGenerator.h"
#include "FairRtdbRun.h"
#include "FairLogger.h"

#include "TClonesArray.h"
#include "TH1F.h"

#include <iostream>
#include <stdlib.h>

using namespace std;

R3BLosTcalFill::R3BLosTcalFill()
    : fUpdateRate(1000000)
    , fMinStats(100000)
    , fNofCh(0)
    , fhData(NULL)
    , fhTime(NULL)
    , fNEvents(0)
    , fStoreDB(kFALSE)
    , fCal_Par(NULL)
{
}

R3BLosTcalFill::R3BLosTcalFill(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fUpdateRate(1000000)
    , fMinStats(100000)
    , fNofCh(0)
    , fhData(NULL)
    , fhTime(NULL)
    , fNEvents(0)
    , fStoreDB(kFALSE)
    , fCal_Par(NULL)
{
}

R3BLosTcalFill::~R3BLosTcalFill()
{
    if (fhData)
    {
        for (Int_t i = 0; i < fNofCh; i++)
        {
            delete fhData[i];
        }
        delete fhData;
        fhData = NULL;
    }
    if (fhTime)
    {
        for (Int_t i = 0; i < fNofCh; i++)
        {
            delete fhTime[i];
        }
        delete fhTime;
        fhTime = NULL;
    }

    if (fCal_Par)
        delete fCal_Par;
}

InitStatus R3BLosTcalFill::Init()
{
    FairRootManager* rm = FairRootManager::Instance();
    if (!rm)
    {
        return kFATAL;
    }
    fHits = (TClonesArray*)rm->GetObject("LosRawHit");
    if (!fHits)
    {
        return kFATAL;
    }
    if (fStoreDB)
    {

        fCal_Par = (R3BLosCalPar*)FairRuntimeDb::instance()->getContainer("LosCalPar");
    }

    CreateContainers();

    return kSUCCESS;
}

void R3BLosTcalFill::Exec(Option_t* option)
{
    Int_t nHits = fHits->GetEntries();
    R3BLosRawHit* hit;
    Int_t iCh;
    Int_t channel;

    // Loop over mapped hits
    for (Int_t i = 0; i < nHits; i++)
    {
        hit = (R3BLosRawHit*)fHits->At(i);
        if (!hit)
        {
            continue;
        }

        // Check bar ID
        iCh = hit->GetChannel();
        if (iCh >= fNofCh)
        {
            LOG(ERROR) << "R3BLosTcalFill::Exec() : wrong channel: " << iCh << FairLogger::endl;
            continue;
        }

        channel = iCh;

        // Check validity of module
        if (channel < 0 || channel >= fNofCh)
        {
            FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "Illegal detector ID...");
            exit(-1);
        }

        // Fill TAC histogram
        fhData[channel]->Fill(hit->GetTdc());
    }

    // Increment events
    fNEvents += 1;
}

void R3BLosTcalFill::FinishEvent()
{
}

void R3BLosTcalFill::FinishTask()
{
//    if (0 == (fNEvents % fUpdateRate))
    {
        // Re-calculate calibration parameters
        for (Int_t im = 0; im < fNofCh; im++)
        {
            if (fhData[im]->GetEntries() >= fMinStats)
            {
                CalculateParams(im);
                WriteContainer(im);
                fhData[im]->Reset();
            }
        }
        
        Int_t runId = FairRuntimeDb::instance()->getCurrentRun()->getRunId();
        if (fStoreDB && fCal_Par)
        {
            fCal_Par->Print();
            fCal_Par->store(runId);
        }
    }
}

void R3BLosTcalFill::CreateContainers()
{
    if (0 == fNofCh)
    {
        return;
    }
    fhData = new TH1F* [fNofCh];
    fhTime = new TH1F* [fNofCh];
    char str[20];
    for (Int_t i = 0; i < fNofCh; i++)
    {
        sprintf(str, "hLosTcalData_%d", i);
        fhData[i] = new TH1F(str, "", 4096, 0., 4096.);
        sprintf(str, "hLosTime_%d", i);
        fhTime[i] = new TH1F(str, "", 4096, 0., 4096.);
    }
}

void R3BLosTcalFill::CalculateParams(Int_t iModule)
{
    // Define range of channels
    for (Int_t i = 0; i < 4096; i++)
    {
        if (fhData[iModule]->GetBinContent(i + 1) > 0)
        {
            iMin = i;
            break;
        }
    }
    for (Int_t j = 4095; j >= 0; j--)
    {
        if (fhData[iModule]->GetBinContent(j + 1) > 0)
        {
            iMax = j + 1;
            break;
        }
    }
    if (iMax <= iMin)
    {
        return;
    }
    LOG(DEBUG) << "R3BLosTcalFill::CalculateParams() : Range of channels: " << iMin << " - " << iMax << FairLogger::endl;

    Int_t nch = 0;
    Int_t ibin = iMin;
    Int_t group;
    Double_t prev_time = 0.;

    R3BLosTCalPar* pTCal = NULL;
    if (fStoreDB)
    {
        pTCal = new R3BLosTCalPar();
        pTCal->SetChannel(iModule);
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

        cout << " Module: " << iModule << " bin range: " << ibin << " : " << ibin + group << " dbin: " << group << " time set: " << prev_time << endl;

        if (pTCal)
        {
            pTCal->SetBinLowAt(ibin, incr);
            pTCal->SetBinUpAt(ibin + group - 1, incr);
            pTCal->SetTimeAt(prev_time, incr);
        }

        // Next range of channels
        ibin += group;
        nch += 1;
        incr++;
    }

    if (fStoreDB)
        fCal_Par->AddTCalPar(pTCal);

    LOG(INFO) << "R3BLosTcalFill::CalculateParams() : Number of parameters: " << nch << FairLogger::endl;
}

Int_t R3BLosTcalFill::CalculateBin(Int_t iModule, Double_t& prev_time, Int_t ibin, Int_t ngroup)
{
    if ((ibin + ngroup) > iMax)
    {
        Double_t total = fhData[iModule]->Integral(1, 4096);
        Double_t itot = fhData[iModule]->Integral(1, (ibin+1) + ngroup);
        if (itot > 0. && total > 0.)
        {
            Double_t time = 5. * itot / total; // time of channel in [ns]
            LOG(DEBUG) << "R3BLosTcalFill::CalculateBin() : bin=" << ibin << "  time=" << time << "  ngroup=" << ngroup << FairLogger::endl;
            prev_time = time;
        }
        return ngroup;
    }
    Double_t total = fhData[iModule]->Integral(1, 4096);
    Double_t itot = fhData[iModule]->Integral(1, (ibin+1) + ngroup);
    if (itot > 0. && total > 0.)
    {
        Double_t time = 5. * itot / total;  // time of channel in [ns]
        Double_t diff = time - prev_time;   // time difference to previous range
        if (diff * 1e3 < 5.)                // check if below resolution
        {
            // Next iteration
            return CalculateBin(iModule, prev_time, ibin, ngroup + 1);
        }
        else
        {
            // Finalize
            LOG(DEBUG) << "R3BLosTcalFill::CalculateBin() : bin=" << ibin << "  time=" << time << "  ngroup=" << ngroup << FairLogger::endl;
            prev_time = time;
            return ngroup;
        }
    }
    return ngroup;
}

void R3BLosTcalFill::WriteContainer(Int_t iModule)
{
    fhData[iModule]->Write();
    fhTime[iModule]->Write();

    LOG(INFO) << "R3BLosTcalFill : TCAL container " << iModule << " is saved" << FairLogger::endl;
}

ClassImp(R3BLosTcalFill)
