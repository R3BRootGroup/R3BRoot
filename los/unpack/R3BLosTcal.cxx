// ------------------------------------------------------------
// -----                     R3BLosTcal                   -----
// -----          Created 29-04-2014 by D.Kresan          -----
// ------------------------------------------------------------

#include "R3BLosTcal.h"

#include "R3BTCalEngine.h"
#include "R3BLosRawHit.h"
#include "R3BLosHit.h"
#include "R3BTCalPar.h"
#include "R3BTCalModulePar.h"

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
    , fClockFreq(1./VFTX_CLOCK_MHZ*1000.)
{
}

R3BLosTcal::R3BLosTcal(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fRawHit(NULL)
    , fHit(new TClonesArray("R3BLosHit"))
    , fNHit(0)
    , fTcalPar(NULL)
    , fClockFreq(1./VFTX_CLOCK_MHZ*1000.)
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
    LOG(INFO) << "R3BLosTcal::Init : read " << fTcalPar->GetNumModulePar() << " calibration channels" << FairLogger::endl;
    //fTcalPar->printParams();
    R3BTCalModulePar* par;
    for(Int_t i = 0; i < fTcalPar->GetNumModulePar(); i++)
    {
        par = fTcalPar->GetModuleParAt(i);
        fMapPar[par->GetModuleId()] = par;
        par->printParams();
    }

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
    fTcalPar = (R3BTCalPar*)(rtdb->getContainer("LosTCalPar"));
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
    R3BTCalModulePar *par;
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
        time = par->GetTimeVFTX(tdc);
        if(time < 0. || time > fClockFreq)
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
        time = par->GetTimeVFTX(tdc);
        if(time < 0. || time > fClockFreq)
        {
            LOG(ERROR) << "R3BLosTcal::Exec : error in time calibration: ch=" << channel << ", tdc=" << tdc << ", time=" << time << FairLogger::endl;
            continue;
        }
        time2 = (rawHit->GetClock() - stopClock)*fClockFreq - (time - stopTime);
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

Bool_t R3BLosTcal::FindChannel(Int_t channel, R3BTCalModulePar** par)
{
    (*par) = fMapPar[channel];
    if(NULL == (*par))
    {
        return kFALSE;
    }
    return kTRUE;
}

ClassImp(R3BLosTcal)
