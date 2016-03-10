// ------------------------------------------------------------
// -----               R3BNeulandMapped2Cal               -----
// -----          Created 22-04-2014 by D.Kresan          -----
// ------------------------------------------------------------

#include "R3BNeulandMapped2Cal.h"

#include "R3BTCalEngine.h"
#include "R3BNeulandMappedData.h"
#include "R3BNeulandCalData.h"
#include "R3BTCalPar.h"
#include "R3BEventHeader.h"

#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairRootManager.h"
#include "FairLogger.h"

#include "TClonesArray.h"
#include "TMath.h"
#include "TH1F.h"

Double_t wlk(Double_t x)
{
    Double_t y = 0;

    Double_t par1 = 1500.;                                                // +-0.2238
    Double_t par2 = 0.00075;                                              //+-2.355e-05
    y = par1 * TMath::Power(x, par2) - (par1 * TMath::Power(400., par2)); // Michael's

    // y=2.29083*log(x)-0.0870157*log(x)*log(x)-4.57824;  // mine

    return y;
    // return 0.;
}

R3BNeulandMapped2Cal::R3BNeulandMapped2Cal()
    : FairTask("NeulandMapped2Cal", 1)
    , fNEvents(0)
    , fPulserMode(kFALSE)
    , fWalkEnabled(kTRUE)
    , fMapPar()
    , fRawHit(NULL)
    , fPmt(new TClonesArray("NeulandCalData"))
    , fNPmt(0)
    , fTcalPar(NULL)
    , fTrigger(-1)
    , fMap17Seen()
    , fMapStopTime()
    , fMapStopClock()
    , fClockFreq(1. / TACQUILA_CLOCK_MHZ * 1000.)
{
}

R3BNeulandMapped2Cal::R3BNeulandMapped2Cal(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fNEvents(0)
    , fPulserMode(kFALSE)
    , fWalkEnabled(kTRUE)
    , fMapPar()
    , fRawHit(NULL)
    , fPmt(new TClonesArray("NeulandCalData"))
    , fNPmt(0)
    , fTcalPar(NULL)
    , fTrigger(-1)
    , fMap17Seen()
    , fMapStopTime()
    , fMapStopClock()
    , fClockFreq(1. / TACQUILA_CLOCK_MHZ * 1000.)
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
    LOG(INFO) << "R3BNeulandMapped2Cal::Init : read " << fTcalPar->GetNumModulePar() << " calibrated modules"
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

    mgr->Register("NeulandCalData", "Land", fPmt, kTRUE);

    fh_pulser_5_2 = new TH1F("h_pulser_5_2", "Single PMT resolution Bar 5 vs 2", 40000, -200., 200.);
    fh_pulser_105_2 = new TH1F("h_pulser_105_2", "Single PMT resolution Bar 105 vs 2", 40000, -200., 200.);

    return kSUCCESS;
}

void R3BNeulandMapped2Cal::SetParContainers()
{
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb = ana->GetRuntimeDb();
    fTcalPar = (R3BTCalPar*)(rtdb->getContainer("LandTCalPar"));
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

    Int_t nHits = fRawHit->GetEntriesFast();
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

    R3BNeulandMappedData* hit;
    R3BNeulandMappedData* hit2;
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
        hit = (R3BNeulandMappedData*)fRawHit->At(ihit);
        if (NULL == hit)
        {
            continue;
        }
        if (hit->Is17())
        {
            // 17-th channel
            channel = fNofPMTs + hit->GetSam() * (MAX_TACQUILA_MODULE + 1) * (MAX_TACQUILA_GTB + 1) + hit->GetGtb() * (MAX_TACQUILA_MODULE + 1) + hit->GetTacAddr();
        }
        else
        {
            continue;
        }
        // Convert TDC to [ns]
        if (channel < 0 || channel >= (2*fNofPMTs))
        {
            LOG(ERROR) << "R3BNeulandMapped2Cal::Exec : wrong hardware channel: " << channel << FairLogger::endl;
            continue;
        }
        if (!FindChannel(channel, &par))
        {
            LOG(WARNING) << "R3BNeulandMapped2Cal::Exec : Tcal par not found, channel: " << channel << FairLogger::endl;
            continue;
        }

        tdc = hit->GetTacData();
        time = par->GetTimeTacquila(tdc);
        if (time < 0. || time > fClockFreq)
        {
            LOG(ERROR) << "R3BNeulandMapped2Cal::Exec : error in time calibration: ch=" << channel << ", tdc=" << tdc
                       << ", time=" << time << FairLogger::endl;
            continue;
        }

        gtb = hit->GetGtb();
        tacAddr = hit->GetTacAddr();
        index = hit->GetSam() * (MAX_TACQUILA_MODULE + 1) * (MAX_TACQUILA_GTB + 1) + gtb * (MAX_TACQUILA_MODULE + 1) + tacAddr;
        if (fMap17Seen.find(index) != fMap17Seen.end() && fMap17Seen[index])
        {
            LOG(WARNING) << "R3BNeulandMapped2Cal::Exec : multiple stop signal for: GTB=" << gtb << ", TacAddr=" << tacAddr
                         << FairLogger::endl;
            //            continue;
        }
        fMap17Seen[index] = kTRUE;
        fMapStopTime[index] = time;
        fMapStopClock[index] = hit->GetClock();
    }

    for (Int_t khit = 0; khit < nHits; khit++)
    {
        hit2 = (R3BNeulandMappedData*)fRawHit->At(khit);
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

        // Convert TDC to [ns]
        if (channel < 0 || channel >= (2*fNofPMTs))
        {
            LOG(ERROR) << "R3BNeulandMapped2Cal::Exec : wrong hardware channel: " << channel << FairLogger::endl;
            continue;
        }
        if (!FindChannel(channel, &par))
        {
            LOG(DEBUG) << "R3BNeulandMapped2Cal::Exec : Tcal par not found, barId: " << iBar << ", side: " << iSide
            << FairLogger::endl;
            continue;
        }

        tdc = hit2->GetTacData();
        time2 = par->GetTimeTacquila(tdc);
        if (time2 < 0. || time2 > fClockFreq)
        {
            LOG(ERROR) << "R3BNeulandMapped2Cal::Exec : error in time calibration: ch=" << channel << ", tdc=" << tdc
            << ", time=" << time2 << FairLogger::endl;
            continue;
        }

        gtb = hit2->GetGtb();
        tacAddr = hit2->GetTacAddr();
        index2 = hit2->GetSam() * (MAX_TACQUILA_MODULE + 1) * (MAX_TACQUILA_GTB + 1) + gtb * (MAX_TACQUILA_MODULE + 1) + tacAddr;

        if (fMap17Seen.find(index2) == fMap17Seen.end())
        {
            LOG(ERROR) << "R3BNeulandMapped2Cal::Exec : NO stop signal for: GTB=" << gtb << ", TacAddr=" << tacAddr
            << FairLogger::endl;
            continue;
        }
        time2 = time2 - fMapStopTime[index2] + hit2->GetClock() * fClockFreq;
        if (fWalkEnabled)
        {
            time2 += wlk(hit2->GetQdcData());
        }
        new ((*fPmt)[fNPmt]) R3BNeulandCalData(iBar, iSide, time2, hit2->GetQdcData());
        fNPmt += 1;
    }

    if (fPulserMode)
    {
        R3BNeulandCalData* pmt1;
        Double_t time1;
        for (Int_t i = 0; i < fNPmt; i++)
        {
            pmt1 = (R3BNeulandCalData*)fPmt->At(i);
            if (pmt1->GetBarId() == 2 && pmt1->GetSide() == 1)
            {
                time1 = pmt1->GetTime();
                break;
            }
        }
        for (Int_t i = 0; i < fNPmt; i++)
        {
            pmt1 = (R3BNeulandCalData*)fPmt->At(i);
            if (pmt1->GetBarId() == 5 && pmt1->GetSide() == 1)
            {
                fh_pulser_5_2->Fill(pmt1->GetTime() - time1);
            }
            if (pmt1->GetBarId() == 105 && pmt1->GetSide() == 1)
            {
                fh_pulser_105_2->Fill(pmt1->GetTime() - time1);
            }
        }
    }
}

void R3BNeulandMapped2Cal::FinishEvent()
{
    if (fVerbose && 0 == (fNEvents % 1000))
    {
        LOG(INFO) << "R3BNeulandMapped2Cal::Exec : event=" << fNEvents << " nPMTs=" << fNPmt << FairLogger::endl;
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

void R3BNeulandMapped2Cal::FinishTask()
{
    fh_pulser_5_2->Write();
    fh_pulser_105_2->Write();
}

Bool_t R3BNeulandMapped2Cal::FindChannel(Int_t channel, R3BTCalModulePar** par)
{
    (*par) = fMapPar[channel];
    if (NULL == (*par))
    {
        return kFALSE;
    }
    return kTRUE;
}

ClassImp(R3BNeulandMapped2Cal)
