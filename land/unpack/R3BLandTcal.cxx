// ------------------------------------------------------------
// -----                    R3BLandTcal                   -----
// -----          Created 22-04-2014 by D.Kresan          -----
// ------------------------------------------------------------

#define MAX_TACQUILA_SAM 7     // 0 .. 7
#define MAX_TACQUILA_GTB 1     // 0 and 1
#define MAX_TACQUILA_MODULE 20 // 0 .. 20
#define TACQUILA_NUM_GEOM ((MAX_TACQUILA_SAM + 1) * (MAX_TACQUILA_GTB + 1) * (MAX_TACQUILA_MODULE + 1))

#include "R3BLandTcal.h"

#include "R3BTCalEngine.h"
#include "R3BNeulandMappedItem.h"
#include "R3BLandPmt.h"
#include "R3BTCalPar.h"
#include "R3BEventHeader.h"

#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairRootManager.h"
#include "FairLogger.h"

#include "TClonesArray.h"

R3BLandTcal::R3BLandTcal()
    : FairTask("LandTcal", 1)
    , fNEvents(0)
    , fMapPar()
    , fRawHit(NULL)
    , fPmt(new TClonesArray("R3BLandPmt"))
    , fNPmt(0)
    , fTcalPar(NULL)
    , fTrigger(-1)
    , fMap17Seen()
    , fMapStopTime()
    , fMapStopClock()
    , fClockFreq(1. / TACQUILA_CLOCK_MHZ * 1000.)
{
}

R3BLandTcal::R3BLandTcal(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fNEvents(0)
    , fMapPar()
    , fRawHit(NULL)
    , fPmt(new TClonesArray("R3BLandPmt"))
    , fNPmt(0)
    , fTcalPar(NULL)
    , fTrigger(-1)
    , fMap17Seen()
    , fMapStopTime()
    , fMapStopClock()
    , fClockFreq(1. / TACQUILA_CLOCK_MHZ * 1000.)
{
}

R3BLandTcal::~R3BLandTcal()
{
    if (fPmt)
    {
        delete fPmt;
        fPmt = NULL;
        fNPmt = 0;
    }
}

InitStatus R3BLandTcal::Init()
{
    LOG(INFO) << "R3BLandTcal::Init : read " << fTcalPar->GetNumModulePar() << " calibrated modules"
              << FairLogger::endl;
    // fTcalPar->printParams();
    R3BTCalModulePar* par;
    for (Int_t i = 0; i < fTcalPar->GetNumModulePar(); i++)
    {
        par = fTcalPar->GetModuleParAt(i);
        fMapPar[par->GetModuleId()] = par;
        par->printParams();
    }

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
    {
        FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "FairRootManager not found");
    }

    header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");
    if (NULL == header)
    {
        FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "Branch R3BEventHeader not found");
    }

    fRawHit = (TClonesArray*)mgr->GetObject("LandRawHitMapped");
    if (NULL == fRawHit)
    {
        FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "Branch LandRawHitMapped not found");
    }

    mgr->Register("LandPmt", "Land", fPmt, kTRUE);

    return kSUCCESS;
}

void R3BLandTcal::SetParContainers()
{
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb = ana->GetRuntimeDb();
    fTcalPar = (R3BTCalPar*)(rtdb->getContainer("LandTCalPar"));
}

InitStatus R3BLandTcal::ReInit()
{
    SetParContainers();
    return kSUCCESS;
}

void R3BLandTcal::Exec(Option_t* option)
{
    if (fTrigger >= 0)
    {
        if (header->GetTrigger() != fTrigger)
        {
            return;
        }
    }

    Int_t nHits = fRawHit->GetEntriesFast();
    if (nHits > (fNofPMTs / 2))
    {
        return;
    }

    R3BNeulandMappedItem* hit;
    R3BNeulandMappedItem* hit2;
    Int_t iBar;
    Int_t iSide;
    Int_t channel;
    Int_t tdc;
    R3BTCalModulePar* par;
    Double_t time;
    Double_t time2;
    Int_t gtb;
    Int_t tacAddr;
    Int_t index;
    Int_t index2;

    for (Int_t ihit = 0; ihit < nHits; ihit++)
    {
        hit = (R3BNeulandMappedItem*)fRawHit->At(ihit);
        if (NULL == hit)
        {
            continue;
        }
        if (hit->Is17())
        {
            // 17-th channel
            channel = fNofPMTs + hit->GetGtb() * 20 + hit->GetTacAddr();
        }
        else
        {
            continue;
        }
        // Convert TDC to [ns]
        if (channel < 0 || channel >= (fNofPMTs + fNof17))
        {
            LOG(ERROR) << "R3BLandTcal::Exec : wrong hardware channel: " << channel << FairLogger::endl;
            continue;
        }
        if (!FindChannel(channel, &par))
        {
            LOG(WARNING) << "R3BLandTcal::Exec : Tcal par not found, channel: " << channel << FairLogger::endl;
            continue;
        }

        tdc = hit->GetTacData();
        time = par->GetTimeTacquila(tdc);
        if (time < -1000.)
        {
            continue;
        }
        if (time < 0. || time > fClockFreq)
        {
            LOG(ERROR) << "R3BLandTcal::Exec : error in time calibration: ch=" << channel << ", tdc=" << tdc
                       << ", time=" << time << FairLogger::endl;
            continue;
        }

        gtb = hit->GetGtb();
        tacAddr = hit->GetTacAddr();
        index = hit->GetSam() * (MAX_TACQUILA_MODULE + 1) * (MAX_TACQUILA_GTB + 1) + gtb * (MAX_TACQUILA_MODULE + 1) +
                tacAddr;
        if (fMap17Seen.find(index) != fMap17Seen.end() && fMap17Seen[index])
        {
            LOG(WARNING) << "R3BLandTcal::Exec : multiple stop signal for: GTB=" << gtb << ", TacAddr=" << tacAddr
                         << FairLogger::endl;
            //            continue;
        }
        fMap17Seen[index] = kTRUE;
        fMapStopTime[index] = time;
        fMapStopClock[index] = hit->GetClock();

        for (Int_t khit = 0; khit < ihit; khit++)
        {
            hit2 = (R3BNeulandMappedItem*)fRawHit->At(khit);
            if (NULL == hit2)
            {
                continue;
            }

            iBar = hit2->GetBarId();
            iSide = hit2->GetSide();
            if (hit2->Is17())
            {
                // 17-th channel
                continue;
            }
            else
            {
                // PMT signal
                channel = fNofPMTs / 2 * (iSide - 1) + iBar - 1;
            }

            gtb = hit2->GetGtb();
            tacAddr = hit2->GetTacAddr();
            index2 = hit2->GetSam() * (MAX_TACQUILA_MODULE + 1) * (MAX_TACQUILA_GTB + 1) +
                     gtb * (MAX_TACQUILA_MODULE + 1) + tacAddr;
            if (index != index2)
            {
                continue;
            }

            // Convert TDC to [ns]
            if (channel < 0 || channel >= (fNofPMTs + fNof17))
            {
                LOG(ERROR) << "R3BLandTcal::Exec : wrong hardware channel: " << channel << FairLogger::endl;
                continue;
            }
            if (!FindChannel(channel, &par))
            {
                LOG(DEBUG) << "R3BLandTcal::Exec : Tcal par not found, barId: " << iBar << ", side: " << iSide
                           << FairLogger::endl;
                continue;
            }

            tdc = hit2->GetTacData();
            time2 = par->GetTimeTacquila(tdc);
            if (time2 < -1000.)
            {
                continue;
            }
            if (time2 < 0. || time2 > fClockFreq)
            {
                LOG(ERROR) << "R3BLandTcal::Exec : error in time calibration: ch=" << channel << ", tdc=" << tdc
                           << ", time=" << time2 << FairLogger::endl;
                continue;
            }

            if (fMap17Seen.find(index2) == fMap17Seen.end())
            {
                LOG(ERROR) << "R3BLandTcal::Exec : NO stop signal for: GTB=" << gtb << ", TacAddr=" << tacAddr
                           << FairLogger::endl;
                continue;
            }
            time2 = time2 - fMapStopTime[index2] + hit2->GetClock() * fClockFreq;
            new ((*fPmt)[fNPmt]) R3BLandPmt(iBar, iSide, time2, hit2->GetQdcData());
            fNPmt += 1;
        }

        fMap17Seen.clear();
        fMapStopTime.clear();
        fMapStopClock.clear();
    }
}

void R3BLandTcal::FinishEvent()
{
    if (fVerbose && 0 == (fNEvents % 1000))
    {
        LOG(INFO) << "R3BLandTcal::Exec : event=" << fNEvents << " nPMTs=" << fNPmt << FairLogger::endl;
    }

    if (fPmt)
    {
        fPmt->Clear();
        fNPmt = 0;
    }
    fMap17Seen.clear();
    fMapStopTime.clear();
    fMapStopClock.clear();

    fNEvents += 1;
}

void R3BLandTcal::FinishTask()
{
}

Bool_t R3BLandTcal::FindChannel(Int_t channel, R3BTCalModulePar** par)
{
    (*par) = fMapPar[channel];
    if (NULL == (*par))
    {
        return kFALSE;
    }
    return kTRUE;
}

ClassImp(R3BLandTcal)
