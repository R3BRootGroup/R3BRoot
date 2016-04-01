
// ------------------------------------------------------------
// -----                  R3BNeuLandTcal                  -----
// -----            Created 27-01-2015 by M.Heil          -----
// ------------------------------------------------------------


#include "R3BNeulandTcal.h"

#include "R3BTCalEngine.h"
#include "R3BNeulandTamexMappedItem.h"
#include "R3BNeulandPmt.h"
#include "R3BTCalPar.h"
#include "R3BEventHeader.h"

#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairRootManager.h"
#include "FairLogger.h"

#include "TClonesArray.h"

R3BNeulandTcal::R3BNeulandTcal()
    : FairTask("LandTcal", 1)
    , fNEvents(0)
    , fMapPar()
    , fMappedHit(NULL)
    , fPmt(new TClonesArray("R3BNeulandPmt"))
    , fNPmt(0)
    , fTcalPar(NULL)
    , fTrigger(-1)
//    , fMap17Seen()
//    , fMapStopTime()
//    , fMapStopClock()
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
{
}

R3BNeulandTcal::R3BNeulandTcal(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fNEvents(0)
    , fMapPar()
    , fMappedHit(NULL)
    , fPmt(new TClonesArray("R3BNeulandPmt"))
    , fNPmt(0)
    , fTcalPar(NULL)
    , fTrigger(-1)
//    , fMap17Seen()
//    , fMapStopTime()
//    , fMapStopClock()
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
{
}

R3BNeulandTcal::~R3BNeulandTcal()
{
    if (fPmt)
    {
        delete fPmt;
        fPmt = NULL;
        fNPmt = 0;
    }
}

InitStatus R3BNeulandTcal::Init()
{
    LOG(INFO) << "R3BNeulandTcal::Init : read " << fTcalPar->GetNumModulePar() << " calibrated modules"
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
/*
    header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");
    if (NULL == header)
    {
        FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "Branch R3BEventHeader not found");
    }
*/
    fMappedHit = (TClonesArray*)mgr->GetObject("NeulandTamexMappedItem");
    if (NULL == fMappedHit)
    {
        FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "Branch R3BNeulandTamexMappedItem not found");
    }

    mgr->Register("NeulandPmt", "Land", fPmt, kTRUE);

    return kSUCCESS;
}

void R3BNeulandTcal::SetParContainers()
{
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb = ana->GetRuntimeDb();
    fTcalPar = (R3BTCalPar*)(rtdb->getContainer("LandTCalPar"));
}

InitStatus R3BNeulandTcal::ReInit()
{
    SetParContainers();
    return kSUCCESS;
}

void R3BNeulandTcal::Exec(Option_t*)
{
	/*
    if (fTrigger >= 0)
    {
        if (header->GetTrigger() != fTrigger)
        {
            return;
        }
    }
*/
    Int_t nHits = fMappedHit->GetEntriesFast();
/*
    if (nHits > (fNofPMTs / 2))
    {
        return;
    }
*/
    R3BNeulandTamexMappedItem* hit;
    Int_t iPlane;
    Int_t iBar;
    Int_t iSide;
    Int_t channel;
    Int_t tdc;
    R3BTCalModulePar* par;
    Double_t timeLE;
    Double_t timeTE;

    for (Int_t ihit = 0; ihit < nHits; ihit++)
    {
       hit = (R3BNeulandTamexMappedItem*)fMappedHit->At(ihit);
       if (NULL == hit)
       {
          continue;
       }
       iPlane = hit->GetPlaneId();
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
           channel = iPlane * fNofBars*4 + (iBar-1)*4 + (iSide)*2;
       }
       

       // Convert TDC to [ns]
       if (channel < 0 || channel >= (fNofPlanes*fNofBars*4))
       {
           LOG(ERROR) << "R3BNeulandTcal::Exec : wrong hardware channel: " << channel << FairLogger::endl;
           continue;
       }
       if (!FindChannel(channel, &par))
       {
           LOG(DEBUG) << "R3BNeulandTcal::Exec : Tcal par not found, barId: " << iBar << ", side: " << iSide
                      << FairLogger::endl;
           continue;
       }
       

       tdc = hit->GetFineTimeLE();
       timeLE = par->GetTimeVFTX(tdc);

       channel+=1;
       if (!FindChannel(channel, &par))
       {
           LOG(DEBUG) << "R3BNeulandTcal::Exec : Tcal par not found, barId: " << iBar << ", side: " << iSide
                      << FairLogger::endl;
           continue;
       }
      
       tdc = hit->GetFineTimeTE();
       timeTE = par->GetTimeVFTX(tdc);
       

/*            
       if (timeLE < -1000.)
       {
           continue;
       }
*/
       if (timeLE < 0. || timeLE > fClockFreq || timeTE < 0. || timeTE > fClockFreq)
       {
           LOG(ERROR) << "R3BNeulandTcal::Exec : error in time calibration: ch= " << channel << ", tdc= " << tdc
                      << ", time leading edge = " << timeLE << ", time trailing edge = " << timeTE << FairLogger::endl;
           continue;
       }

       timeLE = fClockFreq-timeLE + hit->GetCoarseTimeLE() * fClockFreq;
       timeTE = fClockFreq-timeTE + hit->GetCoarseTimeTE() * fClockFreq;
       
       
       new ((*fPmt)[fNPmt]) R3BNeulandPmt(iPlane, iBar, iSide, timeLE, timeTE-timeLE);
       fNPmt += 1;
       

    }
}

void R3BNeulandTcal::FinishEvent()
{
    if (fVerbose && 0 == (fNEvents % 1))
    {
        LOG(INFO) << "R3BNeulandTcal::Exec : event=" << fNEvents << " nPMTs=" << fNPmt << FairLogger::endl;
    }

    if (fPmt)
    {
        fPmt->Clear();
        fNPmt = 0;
    }

    fNEvents += 1;
}

void R3BNeulandTcal::FinishTask()
{
}

Bool_t R3BNeulandTcal::FindChannel(Int_t channel, R3BTCalModulePar** par)
{
    (*par) = fMapPar[channel];
    if (NULL == (*par))
    {
        return kFALSE;
    }
    return kTRUE;
}

ClassImp(R3BNeulandTcal)
