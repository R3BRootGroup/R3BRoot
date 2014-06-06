// ------------------------------------------------------------
// -----                     R3BLosTcal                   -----
// -----          Created 29-04-2014 by D.Kresan          -----
// ------------------------------------------------------------

#include "R3BLosTcal.h"

#include "R3BLosRawHit.h"
#include "R3BLosHit.h"
#include "R3BLosCalPar.h"
#include "R3BLosTCalPar.h"

#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairRootManager.h"
#include "FairLogger.h"

#include "TClonesArray.h"

R3BLosTcal::R3BLosTcal()
    : FairTask("LosTcal", 1)
    , fRawHit(NULL)
    , fHit(new TClonesArray("R3BLosHit"))
    , fNHit(0)
    , fTcalPar(NULL)
{
}

R3BLosTcal::R3BLosTcal(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fRawHit(NULL)
    , fHit(new TClonesArray("R3BLosHit"))
    , fNHit(0)
    , fTcalPar(NULL)
{
}

R3BLosTcal::~R3BLosTcal()
{
    if (fHit)
    {
        delete fHit;
        fHit = NULL;
        fNHit = 0;
    }
}

InitStatus R3BLosTcal::Init()
{
    LOG(INFO) << "R3BLosTcal::Init : read " << fTcalPar->GetNumTCalPar() << " calibration channels" << FairLogger::endl;
    //fTcalPar->Print();

    FairRootManager* mgr = FairRootManager::Instance();
    if(NULL == mgr)
    {
        FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "FairRootManager not found");
    }
    
    fRawHit = (TClonesArray*) mgr->GetObject("LosRawHit");
    if(NULL == fRawHit)
    {
        FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "Branch LosRawHit not found");
    }
    
    mgr->Register("LosHit", "Los", fHit, kTRUE);
    
    return kSUCCESS;
}

void R3BLosTcal::SetParContainers()
{
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb = ana->GetRuntimeDb();
    fTcalPar = (R3BLosCalPar*)(rtdb->getContainer("LosCalPar"));
}

InitStatus R3BLosTcal::ReInit()
{
    SetParContainers();
    return kSUCCESS;
}

void R3BLosTcal::Exec(Option_t* option)
{
    Int_t nHits = fRawHit->GetEntriesFast();
    R3BLosRawHit* rawHit;
    Int_t channel;
    Int_t tdc;
    R3BLosTCalPar *par;
    Double_t time;
    Double_t time2;
    
    Bool_t stopSeen = kFALSE;
    Double_t stopTime;
    Int_t stopClock;
    for(Int_t ihit = 0; ihit < nHits; ihit++)
    {
        rawHit = (R3BLosRawHit*) fRawHit->At(ihit);
        if(NULL == rawHit)
        {
            continue;
        }
        
        channel = rawHit->GetChannel();
        if(15 != channel)
        {
            continue;
        }
        
        if(stopSeen)
        {
            LOG(ERROR) << "R3BLosTcal::Exec : stop signal seen more than once" << FairLogger::endl;
            continue;
        }
        
        // Convert TDC to [ns]
        if(channel < 0 || channel >= fNofModules)
        {
            LOG(ERROR) << "R3BLosTcal::Exec : wrong hardware channel: " << channel << FairLogger::endl;
            continue;
        }
        if(! FindChannel(channel, &par))
        {
            LOG(WARNING) << "R3BLosTcal::Exec : Tcal par not found, channel: " << channel << FairLogger::endl;
            continue;
        }
        
        tdc = rawHit->GetTdc();
        time = Interpolate(tdc, par);
        if(time < 0. || time > 5.)
        {
            LOG(ERROR) << "R3BLosTcal::Exec : error in time calibration: ch=" << channel << ", tdc=" << tdc << ", time=" << time << FairLogger::endl;
            continue;
        }
        
        stopTime = time;
        stopClock = rawHit->GetClock();
        stopSeen = kTRUE;
    }
    
    if(! stopSeen)
    {
        LOG(DEBUG) << "R3BLosTcal::Exec : stop signal missing" << FairLogger::endl;
        return;
    }
    
    for(Int_t ihit = 0; ihit < nHits; ihit++)
    {
        rawHit = (R3BLosRawHit*) fRawHit->At(ihit);
        if(NULL == rawHit)
        {
            continue;
        }
        
        channel = rawHit->GetChannel();
        if(channel >= 4 || channel < 0)
        {
            continue;
        }
        
        // Convert TDC to [ns]
        if(channel < 0 || channel >= fNofModules)
        {
            LOG(ERROR) << "R3BLosTcal::Exec : wrong hardware channel: " << channel << FairLogger::endl;
            continue;
        }
        if(! FindChannel(channel, &par))
        {
            LOG(WARNING) << "R3BLosTcal::Exec : Tcal par not found, channel: " << channel << FairLogger::endl;
            continue;
        }
        
        tdc = rawHit->GetTdc();
        time = Interpolate(tdc, par);
        if(time < 0. || time > 5.)
        {
            LOG(ERROR) << "R3BLosTcal::Exec : error in time calibration: ch=" << channel << ", tdc=" << tdc << ", time=" << time << FairLogger::endl;
            continue;
        }
        time2 = (rawHit->GetClock() - stopClock)*5. - (time - stopTime);
        new ((*fHit)[fNHit]) R3BLosHit(channel, time2);
        fNHit += 1;
    }
    
    if(fVerbose)
    {
        LOG(DEBUG) << "R3BLosTcal::Exec : " << fNHit << " hits in output" << FairLogger::endl;
    }
}

void R3BLosTcal::FinishEvent()
{
    if (fNHit)
    {
        fHit->Clear();
        fNHit = 0;
    }
}

void R3BLosTcal::FinishTask()
{
}

Double_t R3BLosTcal::Interpolate(Int_t tdc, R3BLosTCalPar* par)
{
    Double_t time;
    Int_t p = -1;
    Int_t b1, b2;
    Double_t slope;
    for(Int_t i = 0; i < 2000; i++)
    {
        if(tdc >= par->GetBinLowAt(i) && tdc <= par->GetBinUpAt(i))
        {
            p = i;
            break;
        }
    }
    if(-1 == p)
    {
        return 0.;
    }
    else if(0 == p)
    {
        time = 0.;
    }
    else if(par->GetTimeAt(p) > 4.99)
    {
        time = 5.;
    }
    else
    {
        b1 = (Double_t)(par->GetBinLowAt(p-1) + par->GetBinUpAt(p-1)) / 2.;
        b2 = (Double_t)(par->GetBinLowAt(p) + par->GetBinUpAt(p)) / 2.;
        slope = (Double_t)(par->GetTimeAt(p) - par->GetTimeAt(p-1)) / (b2 - b1);
        time = (Double_t)(tdc - par->GetBinLowAt(p)) * slope + par->GetTimeAt(p-1);
    }
    return time;
}

Bool_t R3BLosTcal::FindChannel(Int_t channel, R3BLosTCalPar** par)
{
    for(Int_t i = 0; i < fTcalPar->GetNumTCalPar(); i++) {
        (*par) = fTcalPar->GetTCalParAt(i);
        if(channel == (*par)->GetChannel()) {
            return kTRUE;
        }
    }
    return kFALSE;
}

ClassImp(R3BLosTcal)
