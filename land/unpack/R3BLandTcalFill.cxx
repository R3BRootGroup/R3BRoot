// ----------------------------------------------------------------
// -----                     R3BLandTcalFill                  -----
// -----             Created 04-04-2014 by D.Kresan           -----
// ----------------------------------------------------------------

#include "R3BLandTcalFill.h"
#include "R3BLandRawHitMapped.h"

#include "FairRootManager.h"
#include "FairLogger.h"

#include "TClonesArray.h"
#include "TH1F.h"

R3BLandTcalFill::R3BLandTcalFill()
    : fUpdateRate(1000000)
    , fMinStats(100000)
    , fNofPMTs(0)
    , fNof17(0)
    , fhData(NULL)
    , fhTime(NULL)
    , fNEvents(0)
{
}

R3BLandTcalFill::R3BLandTcalFill(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fUpdateRate(1000000)
    , fMinStats(100000)
    , fNofPMTs(0)
    , fNof17(0)
    , fhData(NULL)
    , fhTime(NULL)
    , fNEvents(0)
{
}

R3BLandTcalFill::~R3BLandTcalFill()
{
    if (fhData)
    {
        for (Int_t i = 0; i < (fNofPMTs + fNof17); i++)
        {
            delete fhData[i];
        }
        delete fhData;
        fhData = NULL;
    }
    if (fhTime)
    {
        for (Int_t i = 0; i < (fNofPMTs + fNof17); i++)
        {
            delete fhTime[i];
        }
        delete fhTime;
        fhTime = NULL;
    }
}

InitStatus R3BLandTcalFill::Init()
{
    FairRootManager* rm = FairRootManager::Instance();
    if (!rm)
    {
        return kFATAL;
    }
    fHits = (TClonesArray*)rm->GetObject("LandRawHitMapped");
    if (!fHits)
    {
        return kFATAL;
    }

    CreateContainers();

    return kSUCCESS;
}

void R3BLandTcalFill::Exec(Option_t* option)
{
    Int_t nHits = fHits->GetEntriesFast();
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
        if ((iBar - 1) >= (fNofPMTs / 2))
        {
            LOG(ERROR) << "R3BLandTcalFill::Exec() : wrong bar ID: " << iBar << FairLogger::endl;
            continue;
        }

        if (hit->Is17())
        {
            // 17-th channel
            channel = fNofPMTs + hit->GetTacAddr();
        }
        else
        {
            // PMT signal
            iBar = hit->GetBarId();
            iSide = hit->GetSide();
            channel = fNofPMTs / 2 * (iSide - 1) + iBar - 1;
        }

        // Check validity of module
        if (channel < 0 || channel >= (fNofPMTs + fNof17))
        {
            FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "Illegal detector ID...");
            exit(-1);
        }

        // Fill TAC histogram
        fhData[channel]->Fill(hit->GetTacData());
    }

    // Increment events
    fNEvents += 1;
}

void R3BLandTcalFill::FinishEvent()
{
    if (0 == (fNEvents % fUpdateRate))
    {
        // Re-calculate calibration parameters
        for (Int_t im = 0; im < (fNofPMTs + fNof17); im++)
        {
            if (fhData[im]->GetEntries() >= fMinStats)
            {
                CalculateParams(im);
                WriteContainer(im);
                fhData[im]->Reset();
            }
        }
    }
}

void R3BLandTcalFill::FinishTask()
{
}

void R3BLandTcalFill::CreateContainers()
{
    if (0 == fNofPMTs && 0 == fNof17)
    {
        return;
    }
    fhData = new TH1F* [fNofPMTs + fNof17];
    fhTime = new TH1F* [fNofPMTs + fNof17];
    char str[20];
    for (Int_t i = 0; i < (fNofPMTs + fNof17); i++)
    {
        sprintf(str, "hTcalData_%d", i);
        fhData[i] = new TH1F(str, "", 4096, 0., 4095.);
        sprintf(str, "hTime_%d", i);
        fhTime[i] = new TH1F(str, "", 4096, 0., 4095.);
    }
}

void R3BLandTcalFill::CalculateParams(Int_t iModule)
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
            iMax = j;
            break;
        }
    }
    if (iMax <= iMin)
    {
        return;
    }
    LOG(DEBUG) << "R3BLandTcalFill::CalculateParams() : Range of channels: " << iMin << " - " << iMax << FairLogger::endl;

    Int_t nch = 0;
    Int_t ibin = iMin;
    Int_t group;
    Double_t prev_time = 0.;
    while (ibin <= iMax)
    {
        // Iteratively compute parameter
        group = CalculateBin(iModule, prev_time, ibin, 1);
        // Fill time calibration parameter
        for (Int_t i1 = ibin; i1 < (ibin + group); i1++)
        {
            fhTime[iModule]->SetBinContent(i1 + 1, prev_time);
        }
        // Next range of channels
        ibin += group;
        nch += 1;
    }

    LOG(INFO) << "R3BLandTcalFill::CalculateParams() : Number of parameters: " << nch << FairLogger::endl;
}

Int_t R3BLandTcalFill::CalculateBin(Int_t iModule, Double_t& prev_time, Int_t ibin, Int_t ngroup)
{
    if ((ibin + ngroup) > iMax)
    {
        return ngroup;
    }
    Double_t total = fhData[iModule]->Integral();
    Double_t itot = fhData[iModule]->Integral(1, ibin + ngroup);
    if (itot > 0)
    {
        Double_t time = 25. * itot / total; // time of channel in [ns]
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
    LOG(INFO) << "R3BLandTcalFill : TCAL container " << iModule << " is saved" << FairLogger::endl;
}

ClassImp(R3BLandTcalFill)
