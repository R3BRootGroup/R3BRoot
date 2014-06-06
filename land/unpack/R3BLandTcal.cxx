// ------------------------------------------------------------
// -----                    R3BLandTcal                   -----
// -----          Created 22-04-2014 by D.Kresan          -----
// ------------------------------------------------------------

#define MAX_TACQUILA_SAM 7     // 0 .. 7
#define MAX_TACQUILA_GTB 1     // 0 and 1
#define MAX_TACQUILA_MODULE 20 // 0 .. 20
#define TACQUILA_NUM_GEOM ((MAX_TACQUILA_SAM+1)*(MAX_TACQUILA_GTB+1)*(MAX_TACQUILA_MODULE+1))

#include "R3BLandTcal.h"

#include "R3BLandRawHitMapped.h"
#include "R3BLandPmt.h"
#include "R3BLandCalPar.h"

#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairRootManager.h"
#include "FairLogger.h"

#include "TClonesArray.h"
#include "TH1F.h"
#include "TH2F.h"

R3BLandTcal::R3BLandTcal()
    : FairTask("LandTcal", 1)
    , fNEvents(0)
    , fMapPar()
    , fRawHit(NULL)
    , fPmt(new TClonesArray("R3BLandPmt"))
    , fNPmt(0)
    , fTcalPar(NULL)
    , fIsBeam(kFALSE)
    , fMap17Seen()
    , fMapStopTime()
    , fMapStopClock()
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
    , fIsBeam(kFALSE)
    , fMap17Seen()
    , fMapStopTime()
    , fMapStopClock()
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
    LOG(INFO) << "R3BLandTcal::Init : read " << fTcalPar->GetNumTCalPar() << " calibration channels" << FairLogger::endl;
    //fTcalPar->Print();
    R3BLandTCalPar* par;
    for(Int_t i = 0; i < fTcalPar->GetNumTCalPar(); i++)
    {
        par = fTcalPar->GetTCalParAt(i);
        fMapPar[par->GetBarId()] = par;
    }

    FairRootManager* mgr = FairRootManager::Instance();
    if(NULL == mgr)
    {
        FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "FairRootManager not found");
    }
    
    fRawHit = (TClonesArray*) mgr->GetObject("LandRawHitMapped_New");
    if(NULL == fRawHit)
    {
        FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "Branch LandRawHitMapped not found");
    }
    
    mgr->Register("LandPmt", "Land", fPmt, kTRUE);
    
    fh_tres = new TH2F("h_tres", "Resolution", 4096, 0., 4096., 300, 0., 30.);
    
    fh_mult = new TH1F("h_mult", "Multiplicity", 300, -0.5, 299.5);
    
    return kSUCCESS;
}

void R3BLandTcal::SetParContainers()
{
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb = ana->GetRuntimeDb();
    fTcalPar = (R3BLandCalPar*)(rtdb->getContainer("LandCalPar"));
}

InitStatus R3BLandTcal::ReInit()
{
    SetParContainers();
    return kSUCCESS;
}

void R3BLandTcal::Exec(Option_t* option)
{
    Int_t nHits = fRawHit->GetEntriesFast();
    if(nHits > (fNofPMTs/2))
    {
        return;
    }
    
    R3BLandRawHitMapped* hit;
    Int_t iBar;
    Int_t iSide;
    Int_t channel;
    Int_t tdc;
    R3BLandTCalPar *par;
    Double_t time;
    Int_t gtb;
    Int_t tacAddr;
    Int_t index;
    
    for(Int_t ihit = 0; ihit < nHits; ihit++)
    {
        hit = (R3BLandRawHitMapped*) fRawHit->At(ihit);
        if(NULL == hit)
        {
            continue;
        }
        
        if(fIsBeam)
        {
            if(hit->GetCal() != 1)
            {
                continue;
            }
        }
        else
        {
            if(hit->GetCal() != 1 && hit->GetCal() != 2)
            {
                continue;
            }
        }
        
        iBar = hit->GetBarId();
        iSide = hit->GetSide();
        if (hit->Is17())
        {
            // 17-th channel
            channel = fNofPMTs + hit->GetGtb()*20 + hit->GetTacAddr();
        }
        else
        {
            continue;
        }
        // Convert TDC to [ns]
        if(channel < 0 || channel >= (fNofPMTs+fNof17))
        {
            LOG(ERROR) << "R3BLandTcal::Exec : wrong hardware channel: " << channel << FairLogger::endl;
            continue;
        }
        if(! FindChannel(channel, &par))
        {
            LOG(WARNING) << "R3BLandTcal::Exec : Tcal par not found, channel: " << channel << FairLogger::endl;
            continue;
        }
        
        tdc = hit->GetTacData();
        time = Interpolate(tdc, par);
        if(time < -1000.)
        {
            continue;
        }
        if(time < 0. || time > 24.99819)
        {
            LOG(ERROR) << "R3BLandTcal::Exec : error in time calibration: ch=" << channel << ", tdc=" << tdc << ", time=" << time << FairLogger::endl;
            continue;
        }
        
        gtb = hit->GetGtb();
        tacAddr = hit->GetTacAddr();
        index = hit->GetSam()*(MAX_TACQUILA_MODULE+1)*(MAX_TACQUILA_GTB+1) + gtb*(MAX_TACQUILA_MODULE+1) + tacAddr;
        if(fMap17Seen[index])
        {
            LOG(ERROR) << "R3BLandTcal::Exec : multiple stop signal for: GTB=" << gtb << ", TacAddr=" << tacAddr << FairLogger::endl;
            continue;
        }
        fMap17Seen[index] = kTRUE;
        fMapStopTime[index] = time;
        fMapStopClock[index] = hit->GetClock();
    }

    Double_t time2;
    for(Int_t ihit = 0; ihit < nHits; ihit++)
    {
        hit = (R3BLandRawHitMapped*) fRawHit->At(ihit);
        if(NULL == hit)
        {
            continue;
        }
    
        if(fIsBeam)
        {
            if(hit->GetCal() != 1)
            {
                continue;
            }
        }
        else
        {
            if(hit->GetCal() != 1 && hit->GetCal() != 2)
            {
                continue;
            }
        }
        
        iBar = hit->GetBarId();
        iSide = hit->GetSide();
        if (hit->Is17())
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
        if(channel < 0 || channel >= (fNofPMTs+fNof17))
        {
            LOG(ERROR) << "R3BLandTcal::Exec : wrong hardware channel: " << channel << FairLogger::endl;
            continue;
        }
        
        if(! FindChannel(channel, &par))
        {
            LOG(WARNING) << "R3BLandTcal::Exec : Tcal par not found, channel: " << channel << FairLogger::endl;
            continue;
        }
        
        tdc = hit->GetTacData();
        time = Interpolate(tdc, par);
        if(time < -1000.)
        {
            continue;
        }
        if(time < 0. || time > 24.99819)
        {
            LOG(ERROR) << "R3BLandTcal::Exec : error in time calibration: ch=" << channel << ", tdc=" << tdc << ", time=" << time << FairLogger::endl;
            continue;
        }
        
        if(1 == iBar && 1 == iSide)
        {
            fh_tres->Fill(tdc, time);
        }
        
        gtb = hit->GetGtb();
        tacAddr = hit->GetTacAddr();
        index = hit->GetSam()*(MAX_TACQUILA_MODULE+1)*(MAX_TACQUILA_GTB+1) + gtb*(MAX_TACQUILA_MODULE+1) + tacAddr;
        if(!fMap17Seen[index])
        {
            LOG(ERROR) << "R3BLandTcal::Exec : NO stop signal for: GTB=" << gtb << ", TacAddr=" << tacAddr << FairLogger::endl;
            continue;
        }
        time2 = time - fMapStopTime[index] + hit->GetClock()*24.99819;
        new ((*fPmt)[fNPmt]) R3BLandPmt(iBar, iSide, time2, hit->GetQdcData());
        fNPmt += 1;
    }
}

void R3BLandTcal::FinishEvent()
{
    if(fVerbose && 0 == (fNEvents%1000))
    {
        LOG(INFO) << "R3BLandTcal::Exec : event=" << fNEvents << " nPMTs=" << fNPmt << FairLogger::endl;
    }
    
    fh_mult->Fill(fNPmt);

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
    fh_tres->Write();
    fh_mult->Write();
}

Double_t R3BLandTcal::Interpolate(Int_t tdc, R3BLandTCalPar* par)
{
    Double_t time;
    Int_t p = -1;
    Int_t b1, b2;
    Double_t slope;
    for(Int_t i = 0; i < 500; i++)
    {
        if(tdc >= par->GetBinLowAt(i) && tdc <= par->GetBinUpAt(i))
        {
            p = i;
            break;
        }
    }
    if(-1 == p)
    {
        return -10000.;
    }
    else if(0 == p)
    {
        b1 = (Double_t)(par->GetBinLowAt(p) + par->GetBinUpAt(p)) / 2.;
        b2 = (Double_t)(par->GetBinLowAt(p+1) + par->GetBinUpAt(p+1)) / 2.;
        if(b2 < b1)
        {
            FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "%d %d", b2, b1);
        }
        slope = (Double_t)(par->GetTimeAt(p+1) - par->GetTimeAt(p)) / (b2 - b1);
        time = (Double_t)(tdc - par->GetBinLowAt(p)) * slope;
    }
    else if(par->GetTimeAt(p) > 24.99)
    {
        time = 24.99819;
    }
    else
    {
        b1 = (Double_t)(par->GetBinLowAt(p-1) + par->GetBinUpAt(p-1)) / 2.;
        b2 = (Double_t)(par->GetBinLowAt(p) + par->GetBinUpAt(p)) / 2.;
        if(b2 < b1)
        {
            FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "%d %d", b2, b1);
        }
        slope = (Double_t)(par->GetTimeAt(p) - par->GetTimeAt(p-1)) / (b2 - b1);
        time = (Double_t)(tdc - par->GetBinLowAt(p)) * slope + par->GetTimeAt(p-1);
    }
    return time;
}

Bool_t R3BLandTcal::FindChannel(Int_t channel, R3BLandTCalPar** par)
{
    (*par) = fMapPar[channel];
    if(NULL == (*par))
    {
        return kFALSE;
    }
    return kTRUE;
}

ClassImp(R3BLandTcal)
