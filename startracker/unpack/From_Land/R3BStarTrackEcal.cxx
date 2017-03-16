// ------------------------------------------------------------
// -----                    R3BStarTrackEcal              -----
// -----          Created 09-07-2014 by M.Labiche         -----
// ------------------------------------------------------------

#define MAX_TACQUILA_SAM 7     // 0 .. 7
#define MAX_TACQUILA_GTB 1     // 0 and 1
#define MAX_TACQUILA_MODULE 20 // 0 .. 20
#define TACQUILA_NUM_GEOM ((MAX_TACQUILA_SAM+1)*(MAX_TACQUILA_GTB+1)*(MAX_TACQUILA_MODULE+1))

#include "R3BStarTrackEcal.h"

#include "R3BStarTrackRawHitMapped.h"  //  To be defined !! // #include "R3BLandRawHitMapped.h"
#include "R3BStarTrackInfoHit.h"  //  equivalent to "R3BLandPmt.h"
#include "R3BStarTrackCalPar.h"  // equivalent to "R3BLandCalPar.h"

#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairRootManager.h"
#include "FairLogger.h"

#include "TClonesArray.h"
#include "TH1F.h"
#include "TH2F.h"

R3BStarTrackEcal::R3BStarTrackEcal()
    : FairTask("StraTrackEcal", 1)
    , fNEvents(0)
    , fMapPar()
    , fRawHit(NULL)
    , fAdc(new TClonesArray("R3BStarTrackInfoHit"))
    , fNAdc(0)
    , fEcalPar(NULL)
    , fIsBeam(kFALSE)
      //    , fMap17Seen()
    , fMapStopTime()
    , fMapStopClock()
{
}

R3BStarTrackEcal::R3BStarTrackEcal(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fNEvents(0)
    , fMapPar()
    , fRawHit(NULL)
    , fAdc(new TClonesArray("R3BStarTrackInfoHit"))
    , fNAdc(0)
    , fEcalPar(NULL)
    , fIsBeam(kFALSE)
      //    , fMap17Seen()
    , fMapStopTime()
    , fMapStopClock()
{
}

R3BStarTrackEcal::~R3BStarTrackEcal()
{
    if (fAdc)
    {
        delete fAdc;
        fAdc = NULL;
        fNAdc = 0;
    }
}

InitStatus R3BStarTrackEcal::Init()
{
    LOG(INFO) << "R3BStarTrackEcal::Init : read " << fEcalPar->GetNumECalPar() << " calibration channels" << FairLogger::endl;
    //fEcalPar->Print();
    R3BStarTrackECalPar* par;
    for(Int_t i = 0; i < fEcalPar->GetNumECalPar(); i++)
    {
        par = fEcalPar->GetECalParAt(i);
        fMapPar[par->GetModuleId()] = par;    // Marc's todo: do the same also for AsicId, side, stripId
    }

    FairRootManager* mgr = FairRootManager::Instance();
    if(NULL == mgr)
    {
        FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "FairRootManager not found");
    }
    
    fRawHit = (TClonesArray*) mgr->GetObject("StarTrackRawHitMapped_New");
    if(NULL == fRawHit)
    {
        FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "Branch StarTrackRawHitMapped not found");
    }
    
    mgr->Register("StraTrackerInfoHit", "StarTracker", fAdc, kTRUE);
    
    //fh_tres = new TH2F("h_tres", "Resolution", 4096, 0., 4096., 300, 0., 30.);
    
    fh_mult = new TH1F("h_mult", "Multiplicity", 300, -0.5, 299.5);
    
    return kSUCCESS;
}

void R3BStarTrackEcal::SetParContainers()
{
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb = ana->GetRuntimeDb();
    fEcalPar = (R3BStarTrackCalPar*)(rtdb->getContainer("StarTrackCalPar"));
}

InitStatus R3BStarTrackEcal::ReInit()
{
    SetParContainers();
    return kSUCCESS;
}

void R3BStarTrackEcal::Exec(Option_t* option)
{
    Int_t nHits = fRawHit->GetEntriesFast();
    if(nHits > (fNofADCs/2)) // Marc's Todo: adapt to Si tracker
    {
        return;
    }
    
    R3BStarTrackRawHitMapped* hit;
    Int_t iModule;
    Int_t iAsic;
    Int_t iStrip;
    Int_t iSide;
    Int_t iTime;
    Int_t channel;
    Int_t adc;  
    R3BStarTrackECalPar *par;
    Double_t energy;
    //Int_t gtb;
    //Int_t tacAddr;
    //Int_t index;
    
    for(Int_t ihit = 0; ihit < nHits; ihit++)
    {
        hit = (R3BStarTrackRawHitMapped*) fRawHit->At(ihit);
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
        
        iModule = hit->GetModuleId();  // module=Ladder id
        iAsic = hit->GetAsicId();
        iStrip = hit->GetStripId();
        iSide = hit->GetSide();
        iTime = hit->GetTime();


	  // iModule: 0 to 3
	  // iStrip= 0 to 127
	  // iAsic= 0 to 15
	  // iSide=0 : front side ; iside=1 : back side
	  channel = (iAsic + (iSide*16))*128 + (iStrip+1) ; // Channel number for 1 ladder(module=0): Ch=0-2048 <=> front side, Ch:2037-4096 = back side

	  //Globalchannel = iModule*4096 + (iAsic + (iSide*16))*128  + (iStrip+1)); // Channel number for 4 ladder(=module): 
	                                                                          // Ch:0-4096 = 1st ladder
	                                                                          // Ch:4097-8192 = 2nd ladder
	                                                                          // Ch:8193-12288 = 3rd ladder
	                                                                          // Ch:12289-16334 = 4th ladder


         // Convert ADC to [keV]
        if(channel < 0 || channel >= fNofADCs)
        {
            LOG(ERROR) << "R3BStarTrackEcal::Exec : wrong hardware channel: " << channel << FairLogger::endl;
            continue;
        }
        if(! FindChannel(channel, &par))
        {
            LOG(WARNING) << "R3BStarTrackEcal::Exec : Tcal par not found, channel: " << channel << FairLogger::endl;
            continue;
        }
        
        adc = hit->GetTacData();  // Marc's todo
        energy = Interpolate(adc, par); 
  

	new ((*fAdc)[fNAdc]) R3BStarTrackInfoHit(iModule, iAsic, iSide, iStrip, energy, iTime);
 	// new ((*fPmt)[fNPmt]) R3BLandPmt(iBar, iSide, time2, hit->GetQdcData());
        // fNPmt += 1;


    }


}

void R3BStarTrackEcal::FinishEvent()
{
    if(fVerbose && 0 == (fNEvents%1000))
    {
        LOG(INFO) << "R3BStarTrackEcal::Exec : event=" << fNEvents << " nADCs=" << fNAdc << FairLogger::endl;
    }
    
    fh_mult->Fill(fNAdc);

    if (fAdc)
    {
        fAdc->Clear();
        fNAcd = 0;
    }
    fMap17Seen.clear();
    fMapStopTime.clear();
    fMapStopClock.clear();
    
    fNEvents += 1;
}

void R3BStarTrackEcal::FinishTask()
{
    fh_tres->Write();
    fh_mult->Write();
}

Double_t R3BStarTrackEcal::Interpolate(Int_t adc, R3BStarTrackECalPar* par)
{
    Double_t energy;
    Int_t p = -1;
    Int_t b1, b2;
    Double_t slope;
    for(Int_t i = 0; i < 500; i++)
    {
        if(adc >= par->GetBinLowAt(i) && adc <= par->GetBinUpAt(i))
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
        slope = (Double_t)(par->GetEnergyAt(p+1) - par->GetEnergyAt(p)) / (b2 - b1);
        energy = (Double_t)(adc - par->GetBinLowAt(p)) * slope;
    }
    /*    else if(par->GetTimeAt(p) > 24.99)
    {
        time = 24.99819;
	}*/
    else
    {
        b1 = (Double_t)(par->GetBinLowAt(p-1) + par->GetBinUpAt(p-1)) / 2.;
        b2 = (Double_t)(par->GetBinLowAt(p) + par->GetBinUpAt(p)) / 2.;
        if(b2 < b1)
        {
            FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "%d %d", b2, b1);
        }
        slope = (Double_t)(par->GetEnergyAt(p) - par->GetEnergyAt(p-1)) / (b2 - b1);
        energy = (Double_t)(adc - par->GetBinLowAt(p)) * slope + par->GetEnergyAt(p-1);
    }
    return energy;
}

Bool_t R3BStarTrackEcal::FindChannel(Int_t channel, R3BStarTrackECalPar** par)
{
    (*par) = fMapPar[channel];
    if(NULL == (*par))
    {
        return kFALSE;
    }
    return kTRUE;
}

ClassImp(R3BStarTrackEcal)
