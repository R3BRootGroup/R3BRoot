// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                            R3BLandRawAna                          -----
// -----                    Created @ 01.2014 by Madalin Cherciu           -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#include <iostream>
using namespace std;

#include "TClonesArray.h"
#include "TH1F.h"
#include "TH2F.h"

#include "FairRootManager.h"
#include "FairRunOnline.h"
#include "FairLogger.h"

#include "R3BEventHeader.h"
#include "R3BNeulandRawItem.h"
#include "R3BNeulandMappedItem.h"
#include "R3BLosRawHit.h"
#include "R3BLandRawAna.h"

R3BLandRawAna::R3BLandRawAna()
    : fnEvents(0)
    , fHeader(NULL)
    , fLandRawHit(NULL)
    , fLandRawHitMapped(NULL)
    , fLosRawHit(NULL)
{
}

R3BLandRawAna::R3BLandRawAna(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fnEvents(0)
    , fHeader(NULL)
    , fLandRawHit(NULL)
    , fLandRawHitMapped(NULL)
    , fLosRawHit(NULL)
{
}

R3BLandRawAna::~R3BLandRawAna()
{
}

InitStatus R3BLandRawAna::Init()
{
    FairRootManager* fMan = FairRootManager::Instance();
    fHeader = (R3BEventHeader*)fMan->GetObject("R3BEventHeader");
    fLandRawHit = (TClonesArray*)fMan->GetObject("LandRawHit");
    fLandRawHitMapped = (TClonesArray*)fMan->GetObject("LandRawHitMapped");
    fLosRawHit = (TClonesArray*)fMan->GetObject("LosRawHit");
    CreateHistos();

    return kSUCCESS;
}

void R3BLandRawAna::Exec(Option_t* option)
{
    if(fHeader)
    {
        fh_trigger->Fill(fHeader->GetTrigger());
    }

    if (fLandRawHit)
    {
        Int_t nLandRawHits = fLandRawHit->GetEntries();
        R3BNeulandRawItem* hit;
        for (Int_t i = 0; i < nLandRawHits; i++)
        {
            hit = (R3BNeulandRawItem*)fLandRawHit->At(i);
            fh_land_raw_sam->Fill(hit->GetSam());
            fh_land_raw_gtb->Fill(hit->GetGtb());
            fh_land_raw_tacaddr->Fill(hit->GetTacAddr());
            fh_land_raw_tacch->Fill(hit->GetTacCh());
            fh_land_raw_cal->Fill(hit->GetCal());
            fh_land_raw_clock->Fill(hit->GetClock());
            fh_land_raw_tac->Fill(hit->GetTacData());
            if(16 != hit->GetTacCh())
            {
                fh_land_raw_qdc->Fill(hit->GetQdcData());
            }
        }
    }

    if (fLandRawHitMapped)
    {
        Int_t nLandRawHitsMapped = fLandRawHitMapped->GetEntries();
        R3BNeulandMappedItem* hitmapped;
        for (Int_t i = 0; i < nLandRawHitsMapped; i++)
        {
            hitmapped = (R3BNeulandMappedItem*)fLandRawHitMapped->At(i);
            fh_land_mapped_is17->Fill(hitmapped->Is17());
            if(! hitmapped->Is17())
            {
                fh_land_mapped_barid->Fill(hitmapped->GetBarId());
                fh_land_mapped_side->Fill(hitmapped->GetSide());
                fh_land_mapped_clock->Fill(hitmapped->GetClock());
                fh_land_mapped_tac->Fill(hitmapped->GetTacData());
                fh_land_mapped_qdc->Fill(hitmapped->GetQdcData());
            }
        }
    }

    if (fLosRawHit)
    {
        Int_t nLosRawHits = fLosRawHit->GetEntries();
        R3BLosRawHit* loshit;
        for (Int_t i = 0; i < nLosRawHits; i++)
        {
            loshit = (R3BLosRawHit*)fLosRawHit->At(i);
            fh_los_raw_ch->Fill(loshit->GetChannel());
            fh_los_raw_tdc->Fill(loshit->GetTdc());
            fh_los_raw_clock->Fill(loshit->GetClock());
        }
    }

    fnEvents += 1;
    if(0 == (fnEvents%1000))
    {
        LOG(INFO) << "R3BLandRawAna : " << fnEvents << " events collected" << FairLogger::endl;
    }
}

void R3BLandRawAna::FinishTask()
{
}

void R3BLandRawAna::CreateHistos()
{
    fh_trigger = new TH1F("h_trigger", "Trigger", 10, -0.5, 9.5);
    
    fh_land_raw_sam = new TH1F("h_land_raw_sam", "SAM", 20, -0.5, 19.5);
    fh_land_raw_gtb = new TH1F("h_land_raw_gtb", "GTB", 10, -0.5, 9.5);
    fh_land_raw_tacaddr = new TH1F("h_land_raw_tacaddr", "TAC module", 30, -0.5, 29.5);
    fh_land_raw_tacch = new TH1F("h_land_raw_tacch", "TAC channel", 20, -0.5, 19.5);
    fh_land_raw_cal = new TH1F("h_land_raw_cal", "Trigger", 20, -0.5, 19.5);
    fh_land_raw_clock = new TH1F("h_land_raw_clock", "Clock count", 70, -0.5, 69.5);
    fh_land_raw_tac = new TH1F("h_land_raw_tac", "TAC data", 500, 0., 5000.);
    fh_land_raw_qdc = new TH1F("h_land_raw_qdc", "QDC data", 100, 0., 1000.);

    fh_land_mapped_is17 = new TH1F("h_land_mapped_is17", "Is 17", 4, -0.5, 3.5);
    fh_land_mapped_barid = new TH1F("h_land_mapped_barid", "Bar ID", 500, -0.5, 499.5);
    fh_land_mapped_side = new TH1F("h_land_mapped_side", "Side", 5, -0.5, 4.5);
    fh_land_mapped_clock = new TH1F("h_land_mapped_clock", "Clock count", 70, -0.5, 69.5);
    fh_land_mapped_tac = new TH1F("h_land_mapped_tac", "TAC data", 500, 0., 5000.);
    fh_land_mapped_qdc = new TH1F("h_land_mapped_qdc", "QDC data", 100, 0., 1000.);

    fh_los_raw_ch = new TH1F("h_los_raw_ch", "Channel", 20, -0.5, 19.5);
    fh_los_raw_tdc = new TH1F("h_los_raw_tdc", "TDC data", 200, 0., 2000.);
    fh_los_raw_clock = new TH1F("h_los_raw_clock", "Clock count", 1000, 0., 10000.);

    FairRunOnline* run = FairRunOnline::Instance();
    
    run->AddObject(fh_trigger);
    
    run->AddObject(fh_land_raw_sam);
    run->AddObject(fh_land_raw_gtb);
    run->AddObject(fh_land_raw_tacaddr);
    run->AddObject(fh_land_raw_tacch);
    run->AddObject(fh_land_raw_cal);
    run->AddObject(fh_land_raw_clock);
    run->AddObject(fh_land_raw_tac);
    run->AddObject(fh_land_raw_qdc);

    run->AddObject(fh_land_mapped_is17);
    run->AddObject(fh_land_mapped_barid);
    run->AddObject(fh_land_mapped_side);
    run->AddObject(fh_land_mapped_clock);
    run->AddObject(fh_land_mapped_tac);
    run->AddObject(fh_land_mapped_qdc);

    run->AddObject(fh_los_raw_ch);
    run->AddObject(fh_los_raw_tdc);
    run->AddObject(fh_los_raw_clock);
}

void R3BLandRawAna::WriteHistos()
{
}

ClassImp(R3BLandRawAna)
